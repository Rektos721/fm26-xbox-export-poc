#include <windows.h>

#include <atomic>
#include <fstream>
#include <string>

namespace {
struct Il2CppDomain;
struct Il2CppThread;
struct Il2CppAssembly;
struct Il2CppImage;
struct Il2CppClass;
struct MethodInfo;

using il2cpp_domain_get_t = Il2CppDomain* (*)();
using il2cpp_thread_attach_t = Il2CppThread* (*)(Il2CppDomain*);
using il2cpp_thread_detach_t = void (*)(Il2CppThread*);
using il2cpp_domain_get_assemblies_t = const Il2CppAssembly** (*)(Il2CppDomain*, size_t*);
using il2cpp_assembly_get_image_t = const Il2CppImage* (*)(const Il2CppAssembly*);
using il2cpp_image_get_name_t = const char* (*)(const Il2CppImage*);
using il2cpp_class_from_name_t = Il2CppClass* (*)(const Il2CppImage*, const char*, const char*);
using il2cpp_class_get_method_from_name_t = const MethodInfo* (*)(Il2CppClass*, const char*, int);
using il2cpp_class_get_methods_t = const MethodInfo* (*)(Il2CppClass*, void**);
using il2cpp_method_get_name_t = const char* (*)(const MethodInfo*);
using il2cpp_method_get_param_count_t = uint32_t (*)(const MethodInfo*);

struct Il2CppApi {
    il2cpp_domain_get_t domain_get{};
    il2cpp_thread_attach_t thread_attach{};
    il2cpp_thread_detach_t thread_detach{};
    il2cpp_domain_get_assemblies_t domain_get_assemblies{};
    il2cpp_assembly_get_image_t assembly_get_image{};
    il2cpp_image_get_name_t image_get_name{};
    il2cpp_class_from_name_t class_from_name{};
    il2cpp_class_get_method_from_name_t class_get_method_from_name{};
    il2cpp_class_get_methods_t class_get_methods{};
    il2cpp_method_get_name_t method_get_name{};
    il2cpp_method_get_param_count_t method_get_param_count{};
};

std::atomic<bool> g_workerStarted = false;
std::wstring LogPath() { return L"C:\\Users\\Public\\fm26_payload.log"; }

void LogLine(const std::wstring& line) {
    std::wofstream file(LogPath(), std::ios::app);
    if (!file.is_open()) return;
    file << line << L"\r\n";
}

std::wstring ToWide(const char* text) {
    if (!text) return L"<null>";
    int needed = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    UINT cp = CP_UTF8;
    if (needed <= 0) {
        cp = CP_ACP;
        needed = MultiByteToWideChar(cp, 0, text, -1, nullptr, 0);
    }
    if (needed <= 0) return L"<decode-failed>";
    std::wstring result(needed - 1, L'\0');
    MultiByteToWideChar(cp, 0, text, -1, result.data(), needed);
    return result;
}

FARPROC RequireExport(HMODULE module, const char* name) {
    FARPROC proc = GetProcAddress(module, name);
    if (!proc) LogLine(L"missing export: " + ToWide(name));
    return proc;
}

bool BindIl2Cpp(Il2CppApi& api) {
    HMODULE gameAssembly = GetModuleHandleW(L"GameAssembly.dll");
    if (!gameAssembly) {
        LogLine(L"GameAssembly.dll not loaded");
        return false;
    }

    api.domain_get = reinterpret_cast<il2cpp_domain_get_t>(RequireExport(gameAssembly, "il2cpp_domain_get"));
    api.thread_attach = reinterpret_cast<il2cpp_thread_attach_t>(RequireExport(gameAssembly, "il2cpp_thread_attach"));
    api.thread_detach = reinterpret_cast<il2cpp_thread_detach_t>(RequireExport(gameAssembly, "il2cpp_thread_detach"));
    api.domain_get_assemblies = reinterpret_cast<il2cpp_domain_get_assemblies_t>(RequireExport(gameAssembly, "il2cpp_domain_get_assemblies"));
    api.assembly_get_image = reinterpret_cast<il2cpp_assembly_get_image_t>(RequireExport(gameAssembly, "il2cpp_assembly_get_image"));
    api.image_get_name = reinterpret_cast<il2cpp_image_get_name_t>(RequireExport(gameAssembly, "il2cpp_image_get_name"));
    api.class_from_name = reinterpret_cast<il2cpp_class_from_name_t>(RequireExport(gameAssembly, "il2cpp_class_from_name"));
    api.class_get_method_from_name = reinterpret_cast<il2cpp_class_get_method_from_name_t>(RequireExport(gameAssembly, "il2cpp_class_get_method_from_name"));
    api.class_get_methods = reinterpret_cast<il2cpp_class_get_methods_t>(RequireExport(gameAssembly, "il2cpp_class_get_methods"));
    api.method_get_name = reinterpret_cast<il2cpp_method_get_name_t>(RequireExport(gameAssembly, "il2cpp_method_get_name"));
    api.method_get_param_count = reinterpret_cast<il2cpp_method_get_param_count_t>(RequireExport(gameAssembly, "il2cpp_method_get_param_count"));

    return api.domain_get && api.thread_attach && api.thread_detach && api.domain_get_assemblies &&
           api.assembly_get_image && api.image_get_name && api.class_from_name && api.class_get_method_from_name &&
           api.class_get_methods && api.method_get_name && api.method_get_param_count;
}

void LogCandidateMethods(Il2CppApi& api, Il2CppClass* klass, const wchar_t* label, const char* contains) {
    if (!klass) return;
    LogLine(std::wstring(L"method survey: ") + label);
    void* iter = nullptr;
    while (const MethodInfo* method = api.class_get_methods(klass, &iter)) {
        const char* name = api.method_get_name(method);
        if (!name) continue;
        std::string s(name);
        if (s.find(contains) == std::string::npos) continue;
        LogLine(L"  " + ToWide(name) + L"(" + std::to_wstring(api.method_get_param_count(method)) + L")");
    }
}

void LogIl2CppSurvey(Il2CppApi& api) {
    Il2CppDomain* domain = api.domain_get();
    if (!domain) {
        LogLine(L"il2cpp_domain_get returned null");
        return;
    }
    Il2CppThread* thread = api.thread_attach(domain);
    if (!thread) {
        LogLine(L"il2cpp_thread_attach returned null");
        return;
    }

    size_t assemblyCount = 0;
    const Il2CppAssembly** assemblies = api.domain_get_assemblies(domain, &assemblyCount);
    LogLine(L"il2cpp assemblies=" + std::to_wstring(assemblyCount));

    const Il2CppImage* uiImage = nullptr;
    const Il2CppImage* coreImage = nullptr;
    for (size_t i = 0; assemblies && i < assemblyCount; ++i) {
        const Il2CppImage* image = api.assembly_get_image(assemblies[i]);
        const char* name = image ? api.image_get_name(image) : nullptr;
        std::wstring wide = ToWide(name);
        LogLine(L"image: " + wide);
        if (!uiImage && name && std::string(name).find("UIElements") != std::string::npos) uiImage = image;
        if (!coreImage && name && std::string(name).find("UnityEngine.CoreModule") != std::string::npos) coreImage = image;
    }

    if (uiImage) {
        Il2CppClass* uidocument = api.class_from_name(uiImage, "UnityEngine.UIElements", "UIDocument");
        Il2CppClass* visualElement = api.class_from_name(uiImage, "UnityEngine.UIElements", "VisualElement");
        Il2CppClass* scrollView = api.class_from_name(uiImage, "UnityEngine.UIElements", "ScrollView");
        LogLine(uidocument ? L"UIDocument class found" : L"UIDocument class missing");
        LogLine(visualElement ? L"VisualElement class found" : L"VisualElement class missing");
        LogLine(scrollView ? L"ScrollView class found" : L"ScrollView class missing");
        if (uidocument) {
            const MethodInfo* getter = api.class_get_method_from_name(uidocument, "get_rootVisualElement", 0);
            LogLine(getter ? L"UIDocument.get_rootVisualElement found" : L"UIDocument.get_rootVisualElement missing");
            LogCandidateMethods(api, uidocument, L"UIDocument", "get_");
        }
        if (visualElement) {
            LogCandidateMethods(api, visualElement, L"VisualElement", "get_");
            LogCandidateMethods(api, visualElement, L"VisualElement", "Q");
            LogCandidateMethods(api, visualElement, L"VisualElement", "ElementAt");
        }
        if (scrollView) {
            LogCandidateMethods(api, scrollView, L"ScrollView", "get_");
            LogCandidateMethods(api, scrollView, L"ScrollView", "set_");
        }
    } else {
        LogLine(L"UIElements image not found");
    }

    if (coreImage) {
        Il2CppClass* objectClass = api.class_from_name(coreImage, "UnityEngine", "Object");
        LogLine(objectClass ? L"UnityEngine.Object found" : L"UnityEngine.Object missing");
        if (objectClass) {
            const MethodInfo* findObjects = api.class_get_method_from_name(objectClass, "FindObjectsOfType", 1);
            LogLine(findObjects ? L"UnityEngine.Object.FindObjectsOfType<T> found" : L"UnityEngine.Object.FindObjectsOfType<T> missing");
            LogCandidateMethods(api, objectClass, L"UnityEngine.Object", "FindObjects");
            LogCandidateMethods(api, objectClass, L"UnityEngine.Object", "FindFirst");
        }
    } else {
        LogLine(L"UnityEngine.CoreModule image not found");
    }

    api.thread_detach(thread);
}

DWORD WINAPI HotkeyThread(LPVOID) {
    LogLine(L"hotkey thread start");
    SHORT prev = 0;
    for (;;) {
        SHORT state = GetAsyncKeyState(VK_F9);
        if ((state & 0x8000) && !(prev & 0x8000)) {
            LogLine(L"F9 detected by native payload");
        }
        prev = state;
        Sleep(100);
    }
}
}

extern "C" __declspec(dllexport) DWORD WINAPI StartPayload() {
    LogLine(L"payload start");
    LogLine(L"pid=" + std::to_wstring(GetCurrentProcessId()));

    Il2CppApi api;
    if (BindIl2Cpp(api)) {
        LogLine(L"il2cpp api bind ok");
        LogIl2CppSurvey(api);
    } else {
        LogLine(L"il2cpp api bind failed");
    }

    bool expected = false;
    if (g_workerStarted.compare_exchange_strong(expected, true)) {
        HANDLE thread = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
            LogLine(L"hotkey thread created");
        } else {
            LogLine(L"failed to create hotkey thread: " + std::to_wstring(GetLastError()));
        }
    }

    return 0;
}
