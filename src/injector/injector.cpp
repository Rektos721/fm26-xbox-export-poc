#include <windows.h>

#include <filesystem>
#include <iostream>
#include <string>

namespace {
void PrintLastError(const char* context) {
    const DWORD error = GetLastError();
    std::cerr << context << " failed. GetLastError=" << error << '\n';
}
}  // namespace

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::wcerr << L"Usage: injector.exe <pid> <absolute-path-to-dll>\n";
        return 1;
    }

    DWORD pid = wcstoul(argv[1], nullptr, 10);
    std::filesystem::path dllPath = argv[2];

    if (!dllPath.is_absolute()) {
        std::wcerr << L"DLL path must be absolute.\n";
        return 1;
    }

    if (!std::filesystem::exists(dllPath)) {
        std::wcerr << L"DLL not found: " << dllPath << L"\n";
        return 1;
    }

    HANDLE process = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
            PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE,
        pid);
    if (!process) {
        PrintLastError("OpenProcess");
        return 1;
    }

    const std::wstring dllPathString = dllPath.native();
    const SIZE_T bytes = (dllPathString.size() + 1) * sizeof(wchar_t);

    LPVOID remoteBuffer =
        VirtualAllocEx(process, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteBuffer) {
        PrintLastError("VirtualAllocEx");
        CloseHandle(process);
        return 1;
    }

    if (!WriteProcessMemory(process, remoteBuffer, dllPathString.c_str(), bytes, nullptr)) {
        PrintLastError("WriteProcessMemory");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC loadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");
    if (!loadLibraryW) {
        PrintLastError("GetProcAddress(LoadLibraryW)");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    HANDLE thread = CreateRemoteThread(
        process,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryW),
        remoteBuffer,
        0,
        nullptr);
    if (!thread) {
        PrintLastError("CreateRemoteThread");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    const DWORD wait = WaitForSingleObject(thread, 3000);
    if (wait != WAIT_OBJECT_0) {
        std::cerr << "Remote thread wait failed or timed out. Result=" << wait << '\n';
        CloseHandle(thread);
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    DWORD remoteExitCode = 0;
    if (!GetExitCodeThread(thread, &remoteExitCode)) {
        PrintLastError("GetExitCodeThread");
        CloseHandle(thread);
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    std::wcout << L"Injection finished. LoadLibraryW returned 0x" << std::hex << remoteExitCode
               << L'\n';

    CloseHandle(thread);
    VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(process);
    return remoteExitCode == 0 ? 2 : 0;
}
