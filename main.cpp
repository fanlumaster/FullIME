/*
    输入法的默认状态是中文
*/

// toggle debug console
// #define FANY_DEBUG

#ifndef UNICODE
#define UNICODE
#endif

// we need commctrl v6 for LoadIconMetric()
#pragma comment(                                                                                                       \
    linker,                                                                                                            \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <fstream>

#include "resource.h"
#include <shellapi.h>
#include <commctrl.h>
#include <strsafe.h>

#include <string>

#include "./InitConsole.h"
#include "./src/hook/ime_hook.h"
#include "./src/hook/key_handle_func_lib.h"
#include "./src/sqlite/sqlite_wrapper.h"
#include "./src/ui/cand_ui.h"
#include "./src/utils/caret_helper.h"
#include "./src/utils/constants.h"
#include "./uiaccess.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dwmapi.lib")

//~ start system tray
HINSTANCE g_sysTray_hInst = NULL;

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
UINT const WMAPP_HIDEFLYOUT = WM_APP + 2;

wchar_t const szWindowClass[] = L"NotificationIconTest";
wchar_t const szFlyoutWindowClass[] = L"NotificationFlyout";

// Use a guid to uniquely identify our icon
class __declspec(uuid("9D0B8B92-4E1C-488e-A1E1-2331AFCE2CB6")) FullIMEIcon;

// Forward declarations of functions included in this code module:
void RegisterWindowClass(PCWSTR pszClassName, PCWSTR pszMenuName, WNDPROC lpfnWndProc);
LRESULT CALLBACK WndProcForSysTray(HWND, UINT, WPARAM, LPARAM);
void ShowContextMenu(HWND hwnd, POINT pt);
BOOL AddNotificationIcon(HWND hwnd);
BOOL UpdateNotificationIcon(HWND hwnd);
BOOL UpdateBackNotificationIcon(HWND hwnd);
BOOL DeleteNotificationIcon();
BOOL ShowLowInkBalloon();
BOOL ShowNoInkBalloon();
BOOL ShowPrintJobBalloon();
BOOL RestoreTooltip();

//~ end system tray
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Windos topmost
    // PrepareForUIAccess();
    // 测试是否有内存泄露
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(szCmdLine);
    UNUSED(iCmdShow); // This param is used

// start debug console
#ifdef FANY_DEBUG
    // create the console
    if (AllocConsole())
    {
        FILE *pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        // SetConsoleTitleW("Debug Console");
        SetConsoleTitleW(L"Fany Debug");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }
    // set std::cout to use my custom streambuf
    outbuf ob;
    std::streambuf *sb = std::cout.rdbuf(&ob);
    // do some work here
    printf("IME started successfully!\n");
    // make sure to restore the original so we don't get a crash on close!
    std::cout.rdbuf(sb);
    // end debug console
#endif // DEBUG

    // PrepareForUIAccess();

    // 设置钩子
    HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &KBDHook, 0, 0);

    // 注册一个快捷键
    // TODO: 让 shift
    // 快捷键更加纯粹一点，或者，想一个办法，用钩子来实现这种纯粹的切换输入法的快捷键
    if (0 == RegisterHotKey(NULL, 1, MOD_SHIFT, NULL))
    {
        std::cout << "shift activated." << '\n';
    }

    // 初始化小鹤双拼的码表，纯双拼二码
    // std::string dbPath = "../../src/flyciku.db";
    // 如果是发布阶段，就用这个
    std::string dbPath = "./db/flyciku.db";
    // 如果是调试阶段，那么，就用下面这个
    // std::string dbPath = "./build/Debug/db/flyciku.db";
    // sqlPageMap = transTableToMap(dbPath, 8);  //
    // 如果把这个放到钩子函数里面会导致程序很慢的
    db = openSqlite(dbPath);
    // 初始化 COM
    CoInitialize(nullptr); // <-- add this to init COM
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
    if (!RegisterClassEx(&winClass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), L"error", MB_ICONERROR);
        return 0;
    }

    // 创建窗口
    // 这个 WS_EX_TOOLWINDOW
    // 是为了去掉任务栏区的图标的，这样它就更像一个输入法窗口了 WS_EX_NOACTIVATE
    // 程序执行之后，窗口不会自动获取焦点
    // TODO: 初始化窗口的 initial 尺寸需要结合实际的屏幕的分辨率来具体地去对待
    gHwnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                           winClass.lpszClassName, // window class name, must be consistant with
                                                   // winClass.lpszClassName
                           L"FullIME",             // window caption
                           WS_POPUP,               // window style
                           20,                     // 初始的 x 坐标
                           10,                     // 初始的 y 坐标
                           INITIAL_CAND_UI_WIDTH,  // initial x size
                           INITIAL_CAND_UI_HEIGHT,
                           NULL,      // parent window handle
                           NULL,      // window menu handle
                           hInstance, // program instance handle
                           NULL);     // creation parameters

    CreateDWResource(gHwnd);

    //~ start system tray
    g_sysTray_hInst = hInstance;
    RegisterWindowClass(szWindowClass, MAKEINTRESOURCE(IDC_NOTIFICATIONICON), WndProcForSysTray);

    // Create the main window. This could be a hidden window if you don't need
    // any UI other than the notification icon.
    WCHAR szTitle[100];
    LoadString(hInstance, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));
    HWND hwnd_sysTray = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 640, 480, NULL,
                                     NULL, g_sysTray_hInst, NULL);
    if (hwnd_sysTray)
    {
        // ShowWindow(hwnd, nCmdShow);
        ShowWindow(hwnd_sysTray, SW_HIDE);
    }

    // if (hwnd_sysTray)
    // {
    //     // Main message loop:
    //     MSG msg;
    //     while (GetMessage(&msg, NULL, 0, 0))
    //     {
    //         TranslateMessage(&msg);
    //         DispatchMessage(&msg);
    //     }
    // }
    //~ end system tray

    // 这里可以修改输入法候选框背景的透明度
    // SetLayeredWindowAttributes(gHwnd, 0, 255, LWA_ALPHA);
    SetLayeredWindowAttributes(gHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    // 创建和更新窗口，这是固定操作
    // ShowWindow(hwnd, iCmdShow);
    // ShowWindow(gHwnd, SW_SHOW);
    ShowWindow(gHwnd, 1);
    UpdateWindow(gHwnd);

    MSG msg;
    // 将这个区域的内容先作一下清空
    ZeroMemory(&msg, sizeof(msg));

    // 程序的主体消息循环
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_FANY_HIDEWINDOW)
        {
            // ShowWindow(gHwnd, SW_HIDE);
            SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_HIDEWINDOW |
            // SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (msg.message == WM_FANY_SHOWWINDOW)
        {
            // ShowWindow(gHwnd, SW_SHOW);
            // SetWindowPos(gHwnd, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE
            // | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW |
            // SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            // SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW |
            // SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            SetWindowPos(gHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
            // SetWindowPos(gHwnd, HWND_DESKTOP, 0, 0, 0, 0, SWP_SHOWWINDOW |
            // SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (msg.message == WM_FANY_REDRAW)
        {
            // wText =
            // L"ni'hc\n1.还行\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.可是";
            std::pair<int, int> candSize = calcCandSize(17, 2);
            // std::pair<int, int> caretPos = fanyGetCaretPos();
            std::pair<int, int> caretPos = getGeneralCaretPos();
            // SetWindowPos(gHwnd, NULL, caretPos.first, caretPos.second,
            // candSize.first, candSize.second, SWP_SHOWWINDOW | SWP_NOZORDER |
            // SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS); SetWindowPos(gHwnd,
            // HWND_TOPMOST, caretPos.first, caretPos.second, candSize.first,
            // candSize.second, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE |
            // SWP_ASYNCWINDOWPOS); SetWindowPos(gHwnd, HWND_TOPMOST,
            // caretPos.first, caretPos.second, candSize.first, candSize.second,
            // SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            SetWindowPos(gHwnd, HWND_TOPMOST, caretPos.first, caretPos.second, candSize.first, candSize.second,
                         SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
            // SetWindowPos(gHwnd, HWND_DESKTOP, caretPos.first,
            // caretPos.second, candSize.first, candSize.second, SWP_SHOWWINDOW
            // | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            FanyDrawText(gHwnd, wText);
        }
        else if (msg.message == WM_HOTKEY)
        {
            if (1 == msg.wParam)
            {
                if (charVec.size() == 0)
                {
                    IMEState = !IMEState;
                    if (IMEStateToast == "中")
                    {
                        IMEStateToast = "英";
                        candidateVec.clear();
                        curCandidateVec.clear();
                        UpdateNotificationIcon(hwnd_sysTray);
                    }
                    else
                    {
                        IMEStateToast = "中";
                        UpdateBackNotificationIcon(hwnd_sysTray);
                    }
                }
                else if (charVec.size() > 0)
                {
                    handleEnterByChars();
                    toggleIMEState();
                    if (IMEStateToast == "中")
                    {
                        UpdateBackNotificationIcon(hwnd_sysTray);
                    }
                    else
                    {
                        UpdateNotificationIcon(hwnd_sysTray);
                    }
                }
                // 展示输入法现在的状态
                std::cout << IMEStateToast << '\n';
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 注销钩子
    UnhookWindowsHookEx(kbd);

    // 释放 COM
    CoUninitialize(); // <-- add this to release COM

    // 固定操作
    return msg.wParam;
}

//~ start system tray
void RegisterWindowClass(PCWSTR pszClassName, PCWSTR pszMenuName, WNDPROC lpfnWndProc)
{
    WNDCLASSEX wcex = {sizeof(wcex)};
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = lpfnWndProc;
    wcex.hInstance = g_sysTray_hInst;
    wcex.hIcon = LoadIcon(g_sysTray_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = pszMenuName;
    wcex.lpszClassName = pszClassName;
    RegisterClassEx(&wcex);
}

BOOL AddNotificationIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.hWnd = hwnd;
    // add the icon, setting the icon, tooltip, and callback message.
    // the icon will be identified with the GUID
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    // LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), LIM_SMALL, &nid.hIcon);
    LoadIconMetric(g_sysTray_hInst, MAKEINTRESOURCE(IDI_CHINESEICON), LIM_SMALL, &nid.hIcon);
    LoadString(g_sysTray_hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL UpdateNotificationIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.hWnd = hwnd;
    // add the icon, setting the icon, tooltip, and callback message.
    // the icon will be identified with the GUID
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    // LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), LIM_SMALL, &nid.hIcon);
    LoadIconMetric(g_sysTray_hInst, MAKEINTRESOURCE(IDI_ENGLISHICON), LIM_SMALL, &nid.hIcon);
    LoadString(g_sysTray_hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
    Shell_NotifyIcon(NIM_MODIFY, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL UpdateBackNotificationIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.hWnd = hwnd;
    // add the icon, setting the icon, tooltip, and callback message.
    // the icon will be identified with the GUID
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    // LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), LIM_SMALL, &nid.hIcon);
    LoadIconMetric(g_sysTray_hInst, MAKEINTRESOURCE(IDI_CHINESEICON), LIM_SMALL, &nid.hIcon);
    LoadString(g_sysTray_hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
    Shell_NotifyIcon(NIM_MODIFY, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL DeleteNotificationIcon()
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

BOOL ShowLowInkBalloon()
{
    // Display a low ink balloon message. This is a warning, so show the appropriate system icon.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    // respect quiet time since this balloon did not come from a direct user action.
    nid.dwInfoFlags = NIIF_WARNING | NIIF_RESPECT_QUIET_TIME;
    LoadString(g_sysTray_hInst, IDS_LOWINK_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_sysTray_hInst, IDS_LOWINK_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL ShowNoInkBalloon()
{
    // Display an out of ink balloon message. This is a error, so show the appropriate system icon.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    nid.dwInfoFlags = NIIF_ERROR;
    LoadString(g_sysTray_hInst, IDS_NOINK_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_sysTray_hInst, IDS_NOINK_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL ShowPrintJobBalloon()
{
    // Display a balloon message for a print job with a custom icon
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
    LoadString(g_sysTray_hInst, IDS_PRINTJOB_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_sysTray_hInst, IDS_PRINTJOB_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    LoadIconMetric(g_sysTray_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), LIM_LARGE, &nid.hBalloonIcon);
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL RestoreTooltip()
{
    // After the balloon is dismissed, restore the tooltip.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(FullIMEIcon);
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void PositionFlyout(HWND hwnd, REFGUID guidIcon)
{
    // find the position of our printer icon
    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.guidItem = guidIcon;
    RECT rcIcon;
    HRESULT hr = Shell_NotifyIconGetRect(&nii, &rcIcon);
    if (SUCCEEDED(hr))
    {
        // display the flyout in an appropriate position close to the printer icon
        POINT const ptAnchor = {(rcIcon.left + rcIcon.right) / 2, (rcIcon.top + rcIcon.bottom) / 2};

        RECT rcWindow;
        GetWindowRect(hwnd, &rcWindow);
        SIZE sizeWindow = {rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top};

        if (CalculatePopupWindowPosition(&ptAnchor, &sizeWindow,
                                         TPM_VERTICAL | TPM_VCENTERALIGN | TPM_CENTERALIGN | TPM_WORKAREA, &rcIcon,
                                         &rcWindow))
        {
            // position the flyout and make it the foreground window
            SetWindowPos(hwnd, HWND_TOPMOST, rcWindow.left, rcWindow.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
        }
    }
}

void ShowContextMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = LoadMenu(g_sysTray_hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            // our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user
            // clicks away
            SetForegroundWindow(hwnd);

            // respect menu drop alignment
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
            {
                uFlags |= TPM_RIGHTALIGN;
            }
            else
            {
                uFlags |= TPM_LEFTALIGN;
            }

            TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
        }
        DestroyMenu(hMenu);
    }
}

LRESULT CALLBACK WndProcForSysTray(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND s_hwndFlyout = NULL;
    static BOOL s_fCanShowFlyout = TRUE;

    switch (message)
    {
    case WM_CREATE:
        // add the notification icon
        if (!AddNotificationIcon(hwnd))
        {
            MessageBox(hwnd, L"Please read the ReadMe.txt file for troubleshooting", L"Error adding icon", MB_OK);
            return -1;
        }
        break;
    case WM_COMMAND: {
        int const wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_LOWINK:
            ShowLowInkBalloon();
            break;

        case IDM_NOINK:
            ShowNoInkBalloon();
            break;

        case IDM_PRINTJOB:
            ShowPrintJobBalloon();
            break;

        case IDM_OPTIONS:
            // placeholder for an options dialog
            MessageBox(hwnd, L"Display the options dialog here.", L"Options", MB_OK);
            break;

        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;

        case IDM_FLYOUT:
            // s_hwndFlyout = ShowFlyout(hwnd);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    break;

    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam))
        {
        case NIN_SELECT: // 单击了通知区图标
            // for NOTIFYICON_VERSION_4 clients, NIN_SELECT is prerable to listening to mouse clicks and key presses
            // directly.
            if (IMEStateToast == "中") // 1 -> change to 0 -> English
            {
                UpdateNotificationIcon(hwnd);
                // ShowWindow(hwnd, SW_HIDE);
                IMEState = !IMEState;
                IMEStateToast = "英";
            }
            else
            {
                UpdateBackNotificationIcon(hwnd);
                // ShowWindow(hwnd, SW_SHOW);
                IMEState = !IMEState;
                IMEStateToast = "中";
            }
            // 展示输入法现在的状态
            std::cout << IMEStateToast << '\n';
            break;

        case NIN_BALLOONTIMEOUT:
            // RestoreTooltip();
            break;

        case NIN_BALLOONUSERCLICK:
            // RestoreTooltip();
            // placeholder for the user clicking on the balloon.
            MessageBox(hwnd, L"The user clicked on the balloon.", L"User click", MB_OK);
            break;

        case WM_CONTEXTMENU: { // 右键单击通知区的图标
            POINT const pt = {LOWORD(wParam), HIWORD(wParam)};
            ShowContextMenu(hwnd, pt);
        }
        break;
        }
        break;

    case WM_DESTROY:
        DeleteNotificationIcon();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
//~ end system tray
