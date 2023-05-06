/*
    注意！！！Caution!!!
        这个输入法一开始的状态是英文！需要使用 Ctrl + Space 切换一下状态才可以正常输入中文！！！
*/
#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <cctype>
#include <codecvt>
#include <locale>
#include <string>
#include <cstdio>

#include "./flypytblsqlite.h"
#include "./hook/ime_hook.h"
#include "./sqlite/sqlite_wrapper.h"
#include "./ui/candUI.h"
#include "./utils/caret_helper.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    

    // 设置钩子
    HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &KBDHook, 0, 0);
    // 初始化小鹤双拼的码表，纯双拼二码
    std::string dbPath = "../../src/flyciku.db";
    // sqlPageMap = transTableToMap(dbPath, 8);  // 如果把这个放到钩子函数里面会导致程序很慢的
    db = openSqlite(dbPath);
    // 初始化 COM
    CoInitialize(nullptr);  // <-- add this to init COM
    // 创建一个窗口的基本操作
    WNDCLASSEX winClass;

    winClass.lpszClassName = L"Direct2D";
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = WndProc;
    winClass.hInstance = hInstance;
    winClass.hIcon = NULL;
    winClass.hIconSm = NULL;
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = NULL;
    winClass.lpszMenuName = NULL;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;

    // 注册窗口
    if (!RegisterClassEx(&winClass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), L"error", MB_ICONERROR);
        return 0;
    }

    // 创建窗口
    // 这个 WS_EX_TOOLWINDOW 是为了去掉任务栏区的图标的，这样它就更像一个输入法窗口了
    // WS_EX_NOACTIVATE 程序执行之后，窗口不会自动获取焦点
    gHwnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                           L"Direct2D",        // window class name
                           L"Draw Rectangle",  // window caption
                           WS_POPUP,           // window style
                           20,                 // 初始的 x 坐标
                           10,                 // 初始的 y 坐标
                           78,                 // initial x size
                           306,                // initial y size
                           NULL,               // parent window handle
                           NULL,               // window menu handle
                           hInstance,          // program instance handle
                           NULL);              // creation parameters

    CreateDWResource(gHwnd);

    // 这里可以修改输入法候选框背景的透明度
    SetLayeredWindowAttributes(gHwnd, 0, 255, LWA_ALPHA);

    // 创建和更新窗口，这是固定操作
    // ShowWindow(hwnd, iCmdShow);
    ShowWindow(gHwnd, SW_SHOW);
    UpdateWindow(gHwnd);

    MSG msg;
    // 将这个区域的内容先作一下清空
    ZeroMemory(&msg, sizeof(msg));

    // 程序的主体消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_FANY_HIDEWINDOW) {
            // ShowWindow(gHwnd, SW_HIDE);
            SetWindowPos(gHwnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (msg.message == WM_FANY_SHOWWINDOW) {
            // ShowWindow(gHwnd, SW_SHOW);
            SetWindowPos(gHwnd, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (msg.message == WM_FANY_REDRAW) {
            // wText = L"ni'hc\n1.还行\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.可是";
            std::pair<int, int> candSize = calcCandSize(17, 2);
            // std::pair<int, int> caretPos = fanyGetCaretPos();
            std::pair<int, int> caretPos = getGeneralCaretPos();
            SetWindowPos(gHwnd, NULL, caretPos.first, caretPos.second, candSize.first, candSize.second,
                         SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            FanyDrawText(gHwnd, wText);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 注销钩子
    UnhookWindowsHookEx(kbd);

    // 释放 COM
    CoUninitialize();  // <-- add this to release COM

    // 固定操作
    return msg.wParam;
}
