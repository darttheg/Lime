#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <fstream>
#include <chrono>
#include <cstdio>

struct DownloadGet {
    struct Item {
        bool isDownload;
        int code;
        std::string str;
    };

    std::atomic<bool> downloading{ false };
    std::atomic<bool> cancelled{ false };
    std::atomic<double> progress{ 0.0 };
    std::atomic<double> speed{ 0.0 };
    std::atomic<int> timeoutMs{ 10000 };

    std::mutex mu;
    std::vector<Item> queue;

    void stop();
    void pushGet(int code, std::string content);
    void pushDownloadComplete(int code, std::string path);
    void drain(std::vector<Item>& out);
    bool splitUrl(const std::string& url, std::string& scheme, std::string& host, std::string& path);
    void get(const std::string& url);
    bool extract(const std::string& zipPath, const std::string& outDir);
    void downloadFile(const std::string& url, const std::string& outPath);

    bool isDownloading() { return downloading.load(); }
    float getProgress() { return std::floor((float)progress * 1000.0) / 1000.0; }
    float getSpeed() { return (float)speed.load(); }
    float setTimeout(float t) { timeoutMs.store((int)t); return t; }
};