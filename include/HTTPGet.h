#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <utility>

#pragma comment(lib, "winhttp.lib")

namespace TinyHTTP {
    inline std::wstring utf8_to_wide(const std::string& s) {
        if (s.empty()) return L"";
        int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
        std::wstring w(n, 0);
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], n);
        return w;
    }

    // Returns {status_code, body}. status_code==0 on failure.
    inline std::pair<DWORD, std::string> get(const std::string& url_utf8, DWORD timeout_ms = 30000) {
        std::pair<DWORD, std::string> out{ 0, {} };

        HINTERNET hSession = WinHttpOpen(L"TinyHTTP/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return out;
        WinHttpSetTimeouts(hSession, timeout_ms, timeout_ms, timeout_ms, timeout_ms);

        DWORD redirect = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
        WinHttpSetOption(hSession, WINHTTP_OPTION_REDIRECT_POLICY, &redirect, sizeof(redirect));

        std::wstring wurl = utf8_to_wide(url_utf8);
        URL_COMPONENTS uc{};
        wchar_t host[512]{}, path[4096]{};
        uc.dwStructSize = sizeof(uc);
        uc.lpszHostName = host;  uc.dwHostNameLength = _countof(host);
        uc.lpszUrlPath = path;  uc.dwUrlPathLength = _countof(path);
        uc.dwSchemeLength = (DWORD)-1;

        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc)) { WinHttpCloseHandle(hSession); return out; }

        INTERNET_PORT port = uc.nPort ? uc.nPort :
            (uc.nScheme == INTERNET_SCHEME_HTTPS ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT);

        HINTERNET hConnect = WinHttpConnect(hSession, host, port, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return out; }

        DWORD flags = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path, nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return out; }

        BOOL ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
            && WinHttpReceiveResponse(hRequest, nullptr);
        if (!ok) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return out; }

        DWORD status = 0, sz = sizeof(status);
        WinHttpQueryHeaders(hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &sz, WINHTTP_NO_HEADER_INDEX);
        out.first = status;

        std::string body;
        for (;;) {
            DWORD avail = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &avail) || avail == 0) break;
            body.resize(body.size() + avail);
            DWORD read = 0;
            if (!WinHttpReadData(hRequest, &body[body.size() - avail], avail, &read)) { body.clear(); break; }
            if (read < avail) body.resize(body.size() - (avail - read));
        }
        out.second = std::move(body);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return out;
    }
}