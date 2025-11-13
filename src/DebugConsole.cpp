#include "DebugConsole.h"

#include "IrrManagers.h"

void DebugConsole::makeConsole() {
    AllocConsole();
    FILE* consoleOut;
    freopen_s(&consoleOut, "CONIN$", "r", stdin);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!out.empty() && hConsole != INVALID_HANDLE_VALUE) {
        DWORD written;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteConsoleA(hConsole, out.c_str(), (DWORD)out.size(), &written, nullptr);
    }
}

const char* DebugConsole::getTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_c);
    std::stringstream ss;
    ss << std::put_time(&now_tm, "[%H:%M:%S]");
    static std::string timeStr;
    timeStr = ss.str();
    return timeStr.c_str();
}

void DebugConsole::postError(std::string err) {
    if (endOnError) {
        dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);

        err = "Lime encountered an error:\n" + err;

        std::wstring wStr = std::wstring(err.begin(), err.end());
        const wchar_t* wCharStr = wStr.c_str();

        MessageBox(nullptr, wStr.c_str(), TEXT("Lime Runtime Error"), MB_ICONEXCLAMATION);

        irrHandler->end();
    } else {
        std::string out = "Lime encountered an error: ";
        out += err;
        dConsole.sendMsg(out.c_str(), MESSAGE_TYPE::WARNING);
    }

    return;
}

void DebugConsole::abruptEnd() {
}

void DebugConsole::sendMsg(const char* msg, MESSAGE_TYPE m = MESSAGE_TYPE::NORMAL) {
    std::string time = getTime();
    std::string full = time + " " + msg;

    if (doOutput) {
        out = out + full + "\n";
    }

    if (!enabled) {
        return; // ignore if not enabled
    }

    WORD cur = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    if ((int)m == 1) {
        cur = FOREGROUND_RED;
    } else if((int)m == 2) {
        cur = FOREGROUND_GREEN;
    } else if ((int)m == 3) {
        cur = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(hConsole, cur);

        DWORD written;
        WriteConsoleA(hConsole, full.c_str(), strlen(full.c_str()), &written, nullptr);
        WriteConsoleA(hConsole, "\n", 1, &written, nullptr);
    }
}

void DebugConsole::writeOutput() {
    std::ofstream outFile("output.log");
    if (outFile) {
        outFile << DebugConsole::out << std::endl;
    }

    outFile.close();
}