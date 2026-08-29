#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include "Config.h"
#include "InterpolationHook.h"

typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
typedef HRESULT(WINAPI* DllCanUnloadNow_t)(void);
typedef HRESULT(WINAPI* DllGetClassObject_t)(REFCLSID, REFIID, LPVOID*);
typedef HRESULT(WINAPI* DllRegisterServer_t)(void);
typedef HRESULT(WINAPI* DllUnregisterServer_t)(void);
typedef LPCDIDATAFORMAT(WINAPI* GetdfDIJoystick_t)(void);

static DirectInput8Create_t oDirectInput8Create = nullptr;
static DllCanUnloadNow_t oDllCanUnloadNow = nullptr;
static DllGetClassObject_t oDllGetClassObject = nullptr;
static DllRegisterServer_t oDllRegisterServer = nullptr;
static DllUnregisterServer_t oDllUnregisterServer = nullptr;
static GetdfDIJoystick_t oGetdfDIJoystick = nullptr;

static HMODULE g_hModule = nullptr;
static HMODULE g_hOrigDinput8 = nullptr;

static void LoadOriginalDinput8() {
    if (g_hOrigDinput8) return;

    char sysPath[MAX_PATH];
    GetSystemDirectoryA(sysPath, MAX_PATH);
    strcat_s(sysPath, "\\dinput8.dll");

    g_hOrigDinput8 = LoadLibraryA(sysPath);
    if (g_hOrigDinput8) {
        oDirectInput8Create   = (DirectInput8Create_t)GetProcAddress(g_hOrigDinput8, "DirectInput8Create");
        oDllCanUnloadNow      = (DllCanUnloadNow_t)GetProcAddress(g_hOrigDinput8, "DllCanUnloadNow");
        oDllGetClassObject   = (DllGetClassObject_t)GetProcAddress(g_hOrigDinput8, "DllGetClassObject");
        oDllRegisterServer    = (DllRegisterServer_t)GetProcAddress(g_hOrigDinput8, "DllRegisterServer");
        oDllUnregisterServer  = (DllUnregisterServer_t)GetProcAddress(g_hOrigDinput8, "DllUnregisterServer");
        oGetdfDIJoystick      = (GetdfDIJoystick_t)GetProcAddress(g_hOrigDinput8, "GetdfDIJoystick");
    }
}

extern "C" {

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
    LoadOriginalDinput8();
    if (oDirectInput8Create) {
        return oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
    return E_FAIL;
}

HRESULT WINAPI DllCanUnloadNow(void) {
    LoadOriginalDinput8();
    if (oDllCanUnloadNow) return oDllCanUnloadNow();
    return S_FALSE;
}

HRESULT WINAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    LoadOriginalDinput8();
    if (oDllGetClassObject) return oDllGetClassObject(rclsid, riid, ppv);
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllRegisterServer(void) {
    LoadOriginalDinput8();
    if (oDllRegisterServer) return oDllRegisterServer();
    return E_FAIL;
}

HRESULT WINAPI DllUnregisterServer(void) {
    LoadOriginalDinput8();
    if (oDllUnregisterServer) return oDllUnregisterServer();
    return E_FAIL;
}

LPCDIDATAFORMAT WINAPI GetdfDIJoystick(void) {
    LoadOriginalDinput8();
    if (oGetdfDIJoystick) return oGetdfDIJoystick();
    return nullptr;
}

}

static DWORD WINAPI InitThread(LPVOID lpParam) {
    Sleep(500);

    g_Config.Load(g_hModule);
    InitializeHooks();

    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        LoadOriginalDinput8();
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    } else if (dwReason == DLL_PROCESS_DETACH) {
        UninitializeHooks();
    }
    return TRUE;
}
