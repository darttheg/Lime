#include "DownloadGet.h"
#include "miniz.h"
#include <filesystem>
#include "DebugConsole.h"

void DownloadGet::stop() {
    cancelled = true;
}

void DownloadGet::pushGet(int code, std::string content) {
    std::lock_guard<std::mutex> lk(mu);
    queue.push_back({ false, code, std::move(content) });
}

void DownloadGet::pushDownloadComplete(int code, std::string path) {
    std::lock_guard<std::mutex> lk(mu);
    queue.push_back({ true, code, std::move(path) });
}

void DownloadGet::drain(std::vector<Item>& out) {
    std::lock_guard<std::mutex> lk(mu);
    out.clear();
    out.swap(queue);
}

bool DownloadGet::splitUrl(const std::string& url, std::string& scheme, std::string& host, std::string& path) {
    auto p = url.find("://");
    if (p == std::string::npos) return false;
    scheme = url.substr(0, p);
    auto h = p + 3;
    auto s = url.find('/', h);
    host = s == std::string::npos ? url.substr(h) : url.substr(h, s - h);
    path = s == std::string::npos ? "/" : url.substr(s);
    return !scheme.empty() && !host.empty();
}

void DownloadGet::get(const std::string& url) {
    cancelled = false;
    std::thread([this, url] {
        std::string scheme, host, path;
        if (!splitUrl(url, scheme, host, path)) { pushGet(-1, {}); return; }

        httplib::SSLClient cli(host.c_str());
        cli.set_follow_location(true);
        cli.set_connection_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);
        cli.set_read_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);
        cli.set_write_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);

        int code = -1;
        std::string content;

        auto res = cli.Get(
            path.c_str(),
            [&](const char* data, size_t len) {
                content.append(data, len);
                return !cancelled.load();
            }
        );

        if (res) code = res->status;
        if (!cancelled.load()) pushGet(code, std::move(content));
    }).detach();
}

bool DownloadGet::extract(const std::string& zipPath, const std::string& outDir) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    mz_zip_archive zip{};
    memset(&zip, 0, sizeof(zip));

    if (!mz_zip_reader_init_file(&zip, zipPath.c_str(), 0)) {
        mz_zip_reader_end(&zip);
        return false;
    }

    int fileCount = (int)mz_zip_reader_get_num_files(&zip);
    if (fileCount <= 0) {
        mz_zip_reader_end(&zip);
        return false;
    }

    std::error_code ec;
    std::filesystem::create_directories(outDir, ec);

    int extracted = 0;

    for (int i = 0; i < fileCount; ++i) {
        mz_zip_archive_file_stat st{};
        if (!mz_zip_reader_file_stat(&zip, i, &st))
            continue;

        std::string name = st.m_filename ? st.m_filename : "";
        if (name.empty())
            continue;

        bool isDir = mz_zip_reader_is_file_a_directory(&zip, i);
        std::filesystem::path dst = std::filesystem::path(outDir) / name;

        if (isDir) {
            std::filesystem::create_directories(dst, ec);
            continue;
        }

        if ((st.m_bit_flag & 1) || st.m_method == 99)
            continue;

        std::filesystem::create_directories(dst.parent_path(), ec);

        unsigned int sz = 0;
        void* p = mz_zip_reader_extract_to_heap(&zip, i, (size_t*)&sz, 0);
        if (!p)
            continue;

        std::ofstream out(dst, std::ios::binary);
        if (out.is_open()) {
            out.write((const char*)p, sz);
            out.close();
            ++extracted;
        }

        mz_free(p);

        float percent = ((i + 1) / (float)fileCount) * 100.0f;
        progress = percent;
    }

    mz_zip_reader_end(&zip);
    progress = 100.0f;
    return extracted > 0;
}

void DownloadGet::downloadFile(const std::string& url, const std::string& outPath) {
    if (downloading) return;
    downloading = true;
    cancelled = false;
    progress = 0.0f;
    speed = 0.0f;

    std::thread([this, url, outPath] {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string scheme, host, path;
        if (!splitUrl(url, scheme, host, path)) {
            downloading = false;
            return;
        }

        httplib::SSLClient cli(host.c_str());
        cli.set_follow_location(true);
        cli.set_connection_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);
        cli.set_read_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);
        cli.set_write_timeout(timeoutMs / 1000, (timeoutMs % 1000) * 1000);

        httplib::Headers headers = {
            {"User-Agent", "Mozilla/5.0"},
            {"Accept", "*/*"},
            {"Connection", "close"}
        };

        std::filesystem::path outDir = std::filesystem::path(outPath).parent_path();
        std::filesystem::path downloadPath = outPath;
        std::error_code ec;

        bool isDirectoryTarget = false;
        if (std::filesystem::exists(outPath) && std::filesystem::is_directory(outPath)) {
            isDirectoryTarget = true;
            std::filesystem::create_directories(outPath, ec);
            downloadPath = std::filesystem::path(outPath) / "temp.zip";
        }
        else if (outPath.size() > 4 && outPath.substr(outPath.size() - 4) == ".zip") {
            std::filesystem::create_directories(outDir, ec);
        }
        else {
            isDirectoryTarget = true;
            std::filesystem::create_directories(outPath, ec);
            downloadPath = std::filesystem::path(outPath) / "temp.zip";
        }

        std::ofstream file(downloadPath, std::ios::binary);
        if (!file.is_open()) {
            downloading = false;
            return;
        }

        auto start = std::chrono::steady_clock::now();
        unsigned int bytesWritten = 0;
        float lastUpdate = 0.0f;
        unsigned int lastBytes = 0;
        auto lastCheck = start;

        auto write = [&](const char* data, size_t len) {
            if (cancelled) return false;
            file.write(data, len);
            bytesWritten += (unsigned int)len;

            float elapsed = std::chrono::duration<float>(
                std::chrono::steady_clock::now() - start
            ).count();

            if (elapsed - lastUpdate > 0.25f) {
                speed = bytesWritten / elapsed;
                lastUpdate = elapsed;
            }
            return true;
            };

        auto res = cli.Get(path.c_str(), headers, write);
        file.close();

        float stallTimeout = 3.0f;
        while (res && res->status == 200 && !cancelled) {
            auto now = std::chrono::steady_clock::now();
            float stall = std::chrono::duration<float>(now - lastCheck).count();
            if (bytesWritten == lastBytes) {
                if (stall > stallTimeout)
                    break;
            }
            else {
                lastBytes = bytesWritten;
                lastCheck = now;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        int status = res ? res->status : -1;

        if (!cancelled && status == 200 && isDirectoryTarget) {
            std::ifstream f(downloadPath, std::ios::binary);
            unsigned char sig[4]{};
            f.read(reinterpret_cast<char*>(sig), 4);
            bool isZip = (f.gcount() == 4 && sig[0] == 'P' && sig[1] == 'K');
            f.close();

            if (isZip) {
                speed = 0.0f;
                if (extract(downloadPath.string(), outPath)) {
                    std::error_code ec2;
                    std::filesystem::remove(downloadPath, ec2);
                }
            }
        }

        if (cancelled)
            std::remove(downloadPath.string().c_str());

        downloading = false;
        pushDownloadComplete(status, isDirectoryTarget ? outPath : downloadPath.string());
    }).detach();
}