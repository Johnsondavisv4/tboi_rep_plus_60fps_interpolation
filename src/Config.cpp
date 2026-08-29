#include "Config.h"

Config g_Config;

void Config::Load(HMODULE hModule) {
    char dllPath[MAX_PATH];
    GetModuleFileNameA(hModule, dllPath, MAX_PATH);
    std::string path(dllPath);
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos) {
        path = path.substr(0, pos + 1) + "interpol.ini";
    } else {
        path = "interpol.ini";
    }

    DWORD attrib = GetFileAttributesA(path.c_str());
    if (attrib == INVALID_FILE_ATTRIBUTES) {
        WritePrivateProfileStringA("Interpolation", "Enabled", "1", path.c_str());
        Enabled = 1;
    } else {
        Enabled = GetPrivateProfileIntA("Interpolation", "Enabled", 1, path.c_str());
    }
}
