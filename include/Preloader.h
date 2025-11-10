#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include <stb_image.h>
#include <irrKlang.h>

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <fstream>
#include <string>
#include <atomic>
#include <iterator>

struct Preloader {
    struct TextureJob {
        std::string name;
        irr::core::dimension2d<irr::u32> size;
        irr::video::ECOLOR_FORMAT fmt;
        std::vector<unsigned char> pixels;
    };

    struct MeshJob {
        std::string name;
        std::vector<unsigned char> fileBytes;
    };

    struct SoundJob {
        std::string name;
    };

    mutable std::mutex texMutex;
    mutable std::mutex meshMutex;
    mutable std::mutex sndMutex;
    std::queue<TextureJob> texQ;
    std::queue<MeshJob> meshQ;
    std::queue<SoundJob> sndQ;
    std::atomic<int> texInFlight{ 0 };
    std::atomic<int> meshInFlight{ 0 };
    std::atomic<int> sndInFlight{ 0 };

    std::thread soundThread;
    std::mutex preloadMutex;
    std::condition_variable preloadCv;
    std::queue<std::string> preloadQueue;
    std::atomic<bool> running{ false };
    std::mutex soundEngineMutex;

    irrklang::ISoundEngine* soundEngine = nullptr;

    bool enqueueTextures(sol::variadic_args va) {
        bool ok = true;
        for (sol::stack_object v : va) {
            auto s = v.as<sol::optional<std::string_view>>();
            if (!s) return false;
            std::string path(s->data(), s->size());

            texInFlight.fetch_add(1, std::memory_order_relaxed);
            std::thread([this, p = std::move(path)]() {
                int w, h, n;
                stbi_uc* data = stbi_load(p.c_str(), &w, &h, &n, 4);
                if (!data) { texInFlight.fetch_sub(1, std::memory_order_relaxed); return; }

                TextureJob job;
                job.name = p;
                job.size = { (irr::u32)w, (irr::u32)h };
                job.fmt = irr::video::ECF_A8R8G8B8;
                job.pixels.assign(data, data + (size_t)w * h * 4);
                stbi_image_free(data);

                for (size_t i = 0, e = job.pixels.size(); i < e; i += 4) {
                    unsigned char t = job.pixels[i + 0];
                    job.pixels[i + 0] = job.pixels[i + 2];
                    job.pixels[i + 2] = t;
                }

                { std::scoped_lock lk(texMutex); texQ.push(std::move(job)); }
                texInFlight.fetch_sub(1, std::memory_order_relaxed);
                }).detach();
        }

        return ok;
    }

    bool enqueueMeshes(sol::variadic_args va) {
        bool ok = true;
        for (sol::stack_object v : va) {
            auto s = v.as<sol::optional<std::string_view>>();
            if (!s) return false;
            std::string path(s->data(), s->size());

            meshInFlight.fetch_add(1, std::memory_order_relaxed);
            std::thread([this, p = std::move(path)]() {
                std::ifstream f(p, std::ios::binary);
                if (!f) { meshInFlight.fetch_sub(1, std::memory_order_relaxed); return; }

                std::vector<unsigned char> bytes(
                    (std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

                MeshJob job{ p, std::move(bytes) };
                { std::scoped_lock lk(meshMutex); meshQ.push(std::move(job)); }
                meshInFlight.fetch_sub(1, std::memory_order_relaxed);
                }).detach();
        }

        return ok;
    }

    bool enqueueSounds(sol::variadic_args va) {
        if (!soundEngine) return false;
        bool ok = true;

        {
            std::scoped_lock lk(preloadMutex);
            for (sol::stack_object v : va) {
                auto s = v.as<sol::optional<std::string_view>>();
                if (!s) return false;
                std::string path(s->data(), s->size());
                preloadQueue.push(std::move(path));
            }
        }
        preloadCv.notify_one();
        return ok;
    }

    void openSoundPreloading() {
        if (running) return;
        running = true;
        soundThread = std::thread([this]() {
            while (running) {
                std::unique_lock lk(preloadMutex);
                preloadCv.wait(lk, [this] { return !preloadQueue.empty() || !running; });
                if (!running) break;

                std::string path = std::move(preloadQueue.front());
                preloadQueue.pop();
                lk.unlock();

                sndInFlight.fetch_add(1, std::memory_order_relaxed);

                irrklang::ISoundSource* src = nullptr;
                {
                    std::scoped_lock se(soundEngineMutex);
                    src = soundEngine->addSoundSourceFromFile(path.c_str(), irrklang::ESM_NO_STREAMING, true);
                }

                if (src) {
                    std::scoped_lock ql(sndMutex);
                    sndQ.push({ path });
                }

                sndInFlight.fetch_sub(1, std::memory_order_relaxed);
            }
            });
    }

    void endSoundPreloading() {
        if (!running) return;
        {
            std::scoped_lock lk(preloadMutex);
            running = false;
        }
        preloadCv.notify_all();
        if (soundThread.joinable())
            soundThread.join();
    }

    void pump(irr::video::IVideoDriver* driver, irr::scene::ISceneManager* smgr, irr::io::IFileSystem* fs) {
        // Texture loading
        {
            std::scoped_lock lk(texMutex);
            while (!texQ.empty()) {
                TextureJob job = std::move(texQ.front()); texQ.pop();

                irr::video::IImage* img = driver->createImageFromData(
                    job.fmt, job.size, job.pixels.data(), false);

                if (img) {
                    driver->addTexture(job.name.c_str(), img);
                    img->drop();
                }
            }
        }

        // Mesh loading
        {
            std::scoped_lock lk(meshMutex);
            while (!meshQ.empty()) {
                MeshJob job = std::move(meshQ.front()); meshQ.pop();

                irr::io::IReadFile* mem = fs->createMemoryReadFile(
                    job.fileBytes.data(),
                    static_cast<irr::s32>(job.fileBytes.size()),
                    job.name.c_str(),
                    false
                );

                if (mem) {
                    smgr->getMesh(mem);
                    mem->drop();
                }
            }
        }

        // Sound loading cleanup
        {
            std::scoped_lock lk(sndMutex);
            while (!sndQ.empty()) sndQ.pop();
        }
    }

    bool texturesActive() const {
        std::scoped_lock lk(texMutex);
        return texInFlight.load(std::memory_order_relaxed) > 0 || !texQ.empty();
    }

    bool meshesActive() const {
        std::scoped_lock lk(meshMutex);
        return meshInFlight.load(std::memory_order_relaxed) > 0 || !meshQ.empty();
    }

    bool soundsActive() const {
        std::scoped_lock lk(sndMutex);
        return sndInFlight.load(std::memory_order_relaxed) > 0 || !sndQ.empty();
    }
};