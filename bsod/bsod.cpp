#include "RaiseHardError.h"

//TODO : Make a command-line argument to display the intended text on BSOD(THIS FUCKING THING PREVENTS ME FROM PUTTING A SPACE IN THE TEXT?????)
//TODO : Make a command-line argument to cause the custom error code

//TODO...? : Successful modification of the rest of 32-bit values

int main() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE); //It prevents the cmd window from appearing. But I'll deprecate this if I need to display available arguments for sure.

    auto hNtdll = LoadLibraryA("ntdll.dll"); //Loads the kernel driver in order to be ready to load BSOD. NtRaiseHardError or idk
    if (hNtdll == nullptr) {
        FreeLibrary(hNtdll);
        return ERROR_BAD_ENVIRONMENT; //When it failed to be loaded, or doesn't even exist(it should be impossible because without ntdll.dll how did you even boot your pc lmao)
    }

    auto RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
    BOOLEAN enabled; //RtlAdjustPrivilege will request "SeShutDownPrivilege" permission to shut down the entire system of your PC.
    if (RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &enabled) != 0) {
        FreeLibrary(hNtdll);
        return ERROR_ACCESS_DENIED; //If it got denied, then it will return this.
    }

    auto NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
    FreeLibrary(hNtdll); //Loads the NtRaiseHardError library
    
    //A remain of my shits lmfao
    ULONG_PTR parameters[1] = { 0 }; //These are just useless array variables. I'll find a way to clean these up later.
    parameters[0] = 0xC000021A; //It only defines the BSOD error code. The example code shown here is meaning "STATUS_SYSTEM_PROCESS_TERMINATED", but this will be customizable as mentioned before (in todo list)
    ULONG_PTR parameterd[1] = { 0 };
    parameterd[0] = (ULONG_PTR)"Test Message"; // This will set the custom text of the BSOD. IF THE TEXT EXCEEDS THE CERTAIN LENGTH OF MESSAGE IT WILL RETURN "Exception Message (with some error code and hex things blablabla)" INSTEAD OF YOUR INTENDED MESSAGE.
    HARDERROR_RESPONSE Response;


    //Let the show begin
    return NtRaiseHardError(parameters[0], 1, 0, parameterd, OptionShutdownSystem, &Response);
}
