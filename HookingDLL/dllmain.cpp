#include "pch.h"
#include <iostream>
#include <windows.h>
#include "detours.h"


// Function prototypes
int HookedMessageBoxW(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
BOOL InstallHooks(VOID);
BOOL RemoveHooks(VOID);

// Pointer to the original function
static int (WINAPI* OriginalMessageBoxW) (
    HWND    hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT    uType
    ) = MessageBoxW;


// Hooked version of MessageBoxW API function
int HookedMessageBoxW(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    std::wcout << L"\n=> Call to MessageBoxW intercepted!" << std::endl;

    // Call the original func with different arguments
    OriginalMessageBoxW(NULL, L"⚠ Hello from HookingDLL!", L"HookingDLL", MB_OK);

    return IDOK;
}


BOOL InstallHooks() 
{
    std::wcout << L"[>] Original MessageBoxW address: " << static_cast<void*>(MessageBoxW) << "\n\n";
    std::wcout << L"[+] Installing hooks ..." << std::endl;

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach(&(PVOID&)OriginalMessageBoxW, HookedMessageBoxW);
    DetourTransactionCommit();

	std::wcout << L"[+] MessageBoxW function hooked" << std::endl;

    return TRUE;
}


BOOL RemoveHooks() 
{
    std::wcout << L"\n[+] Removing hooks ..." << std::endl;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)OriginalMessageBoxW, HookedMessageBoxW);
    DetourTransactionCommit();

    std::wcout << L"[+] MessageBoxW function restored " << std::endl;

    return TRUE;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) { InstallHooks(); }
    if (dwReason == DLL_PROCESS_DETACH) { RemoveHooks(); }

    return TRUE;
}

