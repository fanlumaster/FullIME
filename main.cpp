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

#include "./InitConsole.h"
#include "./flypytblsqlite.h"
#include "./hook/ime_hook.h"
#include "./hook/key_handle_func_lib.h"
#include "./sqlite/sqlite_wrapper.h"
#include "./ui/cand_ui.h"
#include "./uiaccess.h"
#include "./utils/caret_helper.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    // Windos topmost
    PrepareForUIAccess();
    // 测试是否有内存泄露
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(szCmdLine);
    UNUSED(iCmdShow);  // This param is used
    // create the console
    if (AllocConsole()) {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        // SetConsoleTitleW("Debug Console");
        SetConsoleTitleW(L"Fany Debug");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }
    // set std::cout to use my custom streambuf
    outbuf ob;
    std::streambuf* sb = std::cout.rdbuf(&ob);
    // do some work here
    printf("IME started successfully!\n");
    // std::cout << "nShowCmd = " << nShowCmd << std::endl;
    // std::cout << "Now making my first Windows window!" << std::endl;
    // make sure to restore the original so we don't get a crash on close!
    std::cout.rdbuf(sb);

    // 设置钩子
    HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &KBDHook, 0, 0);

    // 注册一个快捷键
    if (0 == RegisterHotKey(NULL, 1, MOD_SHIFT, NULL)) {
        std::cout << "shift activated." << '\n';
    }

    // 初始化小鹤双拼的码表，纯双拼二码
    // std::string dbPath = "../../src/flyciku.db";
    std::string dbPath = "./db/flyciku.db";
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
            SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (msg.message == WM_FANY_SHOWWINDOW) {
            // ShowWindow(gHwnd, SW_SHOW);
            // SetWindowPos(gHwnd, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
            // SetWindowPos(gHwnd, HWND_DESKTOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (msg.message == WM_FANY_REDRAW) {
            // wText = L"ni'hc\n1.还行\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.可是";
            std::pair<int, int> candSize = calcCandSize(17, 2);
            // std::pair<int, int> caretPos = fanyGetCaretPos();
            std::pair<int, int> caretPos = getGeneralCaretPos();
            // SetWindowPos(gHwnd, NULL, caretPos.first, caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            // SetWindowPos(gHwnd, HWND_TOPMOST, caretPos.first, caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            // SetWindowPos(gHwnd, HWND_TOPMOST, caretPos.first, caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            SetWindowPos(gHwnd, HWND_TOPMOST, caretPos.first, caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
            // SetWindowPos(gHwnd, HWND_DESKTOP, caretPos.first, caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            FanyDrawText(gHwnd, wText);
        } else if (msg.message == WM_HOTKEY) {
            if (1 == msg.wParam) {
                if (charVec.size() == 0) {
                    IMEState = !IMEState;
                    if (IMEStateToast == "中") {
                        IMEStateToast = "英";
                        candidateVec.clear();
                        curCandidateVec.clear();
                    } else {
                        IMEStateToast = "中";
                    }
                } else if (charVec.size() > 0) {
                    handleEnterByChars();
                    toggleIMEState();
                }
                // 展示输入法现在的状态
                std::cout << IMEStateToast << '\n';
            }
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
