#pragma once
#include <windows.h>
#include <string>

struct Config {
    int Enabled = 1;

    void Load(HMODULE hModule);
};

extern Config g_Config;
