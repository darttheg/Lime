#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

std::unordered_map<std::string, std::string> parseArgs(int argc, char** argv) {
    std::unordered_map<std::string, std::string> args;

    for (int i = 1; i < argc; ++i) {
        std::string key = argv[i];
        std::string value = "true";

        auto eqPos = key.find('=');
        if (eqPos != std::string::npos) {
            value = key.substr(eqPos + 1);
            key = key.substr(0, eqPos);
        }

        if (key.rfind("--", 0) == 0)
            key = key.substr(2);
        else if (key.rfind("-", 0) == 0)
            key = key.substr(1);

        if (eqPos == std::string::npos && i + 1 < argc && argv[i + 1][0] != '-')
            value = std::string(argv[++i]);

        // Trim quotes
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        args[key] = value;
    }

    return args;
}
