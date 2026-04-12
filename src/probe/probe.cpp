#include <windows.h>
#include <fstream>

namespace {
constexpr wchar_t kMarkerPath[] = L"C:\\Users\\Public\\fm26_probe_loaded.txt";

void WriteMarker() {
    std::wofstream file(kMarkerPath, std::ios::app);
    if (!file.is_open()) {
        return;
    }

    file << L"probe.dll loaded into process id " << GetCurrentProcessId() << L"\r\n";
}
}  // namespace

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            WriteMarker();
            break;
        default:
            break;
    }

    return TRUE;
}
