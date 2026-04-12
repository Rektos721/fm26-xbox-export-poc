#include <windows.h>

#include <fstream>
#include <string>

namespace {
using PayloadEntry = DWORD(WINAPI*)();
std::wstring GetSelfDir(HMODULE module) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(module, path, MAX_PATH);
    std::wstring full = path;
    size_t pos = full.find_last_of(L"\\/");
    return pos == std::wstring::npos ? L"." : full.substr(0, pos);
}
void LogLine(const std::wstring& dir, const std::wstring& line) {
    std::wofstream file(dir + L"\\fm26_bootstrap.log", std::ios::app);
    if (!file.is_open()) return;
    file << line << L"\r\n";
}
DWORD WINAPI BootstrapThread(LPVOID param) {
    HMODULE self = static_cast<HMODULE>(param);
    const std::wstring dir = GetSelfDir(self);
    LogLine(dir, L"bootstrap thread start");
    LogLine(dir, L"pid=" + std::to_wstring(GetCurrentProcessId()));
    const std::wstring payloadPath = dir + L"\\payload_v3.dll";
    if (GetFileAttributesW(payloadPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        LogLine(dir, L"payload_v3.dll not found; bootstrap idle");
        return 0;
    }
    HMODULE payload = LoadLibraryW(payloadPath.c_str());
    if (!payload) {
        LogLine(dir, L"LoadLibraryW(payload_v3.dll) failed: " + std::to_wstring(GetLastError()));
        return 1;
    }
    auto entry = reinterpret_cast<PayloadEntry>(GetProcAddress(payload, "StartPayload"));
    if (!entry) {
        LogLine(dir, L"StartPayload export not found");
        return 2;
    }
    LogLine(dir, L"calling StartPayload");
    DWORD rc = entry();
    LogLine(dir, L"StartPayload returned " + std::to_wstring(rc));
    return rc;
}
}
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(nullptr, 0, BootstrapThread, module, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
