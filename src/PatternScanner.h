#pragma once
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>

class PatternScanner {
public:
    static uintptr_t FindPattern(const std::string& pattern) {
        MODULEINFO moduleInfo = { 0 };
        HMODULE hModule = GetModuleHandleA(nullptr);
        if (!hModule) return 0;

        if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO))) {
            return 0;
        }

        uint8_t* pBase = (uint8_t*)moduleInfo.lpBaseOfDll;
        size_t imageSize = (size_t)moduleInfo.SizeOfImage;

        std::vector<int> signature;

        // Check if pattern is continuous (no spaces) or space-separated
        if (pattern.find(' ') != std::string::npos) {
            std::stringstream ss(pattern);
            std::string byteStr;
            while (ss >> byteStr) {
                if (byteStr == "?" || byteStr == "??") {
                    signature.push_back(-1);
                } else {
                    signature.push_back(std::stoi(byteStr, nullptr, 16));
                }
            }
        } else {
            // Continuous format, e.g. "538bdc??896c"
            for (size_t i = 0; i < pattern.length(); i += 2) {
                if (i + 1 >= pattern.length()) break;
                if (pattern[i] == '?' && pattern[i + 1] == '?') {
                    signature.push_back(-1);
                } else if (pattern[i] == '?') {
                    signature.push_back(-1);
                    i--; // single '?' step
                } else {
                    std::string byteStr = pattern.substr(i, 2);
                    signature.push_back(std::stoi(byteStr, nullptr, 16));
                }
            }
        }

        if (signature.empty() || imageSize < signature.size()) {
            return 0;
        }

        uint8_t* pEnd = pBase + imageSize - signature.size();
        for (uint8_t* pCur = pBase; pCur <= pEnd; ++pCur) {
            bool match = true;
            for (size_t i = 0; i < signature.size(); ++i) {
                if (signature[i] != -1 && (uint8_t)signature[i] != pCur[i]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return (uintptr_t)pCur;
            }
        }

        return 0;
    }
};
