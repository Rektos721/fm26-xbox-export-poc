#include <windows.h>
#include <psapi.h>

#include <fstream>
#include <string>
#include <vector>

namespace {
std::wstring GetModulePath(HMODULE module) {
    wchar_t path[MAX_PATH] = {};
    if (GetModuleFileNameW(module, path, MAX_PATH) == 0) {
        return L"";
    }
    return path;
}

std::wstring GetCurrentExePath() {
    return GetModulePath(nullptr);
}

std::wstring GetCurrentDir() {
    DWORD len = GetCurrentDirectoryW(0, nullptr);
    if (len == 0) {
        return L"";
    }
    std::wstring buf(len, L'\0');
    if (GetCurrentDirectoryW(len, buf.data()) == 0) {
        return L"";
    }
    if (!buf.empty() && buf.back() == L'\0') {
        buf.pop_back();
    }
    return buf;
}

void LogLine(const std::wstring& line) {
    std::wofstream file(L"C:\\Users\\Public\\fm26_payload.log", std::ios::app);
    if (!file.is_open()) {
        return;
    }
    file << line << L"\r\n";
}

void LogNamedModule(const wchar_t* name) {
    HMODULE module = GetModuleHandleW(name);
    if (!module) {
        LogLine(std::wstring(L"module missing: ") + name);
        return;
    }
    LogLine(std::wstring(L"module: ") + name + L" => " + GetModulePath(module));
}
}

extern "C" __declspec(dllexport) DWORD WINAPI StartPayload() {
    LogLine(L"payload start");
    LogLine(L"pid=" + std::to_wstring(GetCurrentProcessId()));
    LogLine(L"exe=" + GetCurrentExePath());
    LogLine(L"cwd=" + GetCurrentDir());
    LogNamedModule(L"GameAssembly.dll");
    LogNamedModule(L"UnityPlayer.dll");
    LogNamedModule(L"hostfxr.dll");
    LogNamedModule(L"coreclr.dll");
    LogNamedModule(L"clr.dll");
    LogNamedModule(L"bootstrap.dll");
    return 0;
}
