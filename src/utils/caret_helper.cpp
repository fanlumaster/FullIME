#include "./caret_helper.h"

/*
    获取文本光标(编辑光标)的坐标
*/
std::pair<int, int> getCaretPosByGUIThreadInfo() {
    std::pair<int, int> caretPos;
    HWND target_window = GetForegroundWindow();
    GUITHREADINFO info;
    info.cbSize = sizeof(GUITHREADINFO);
    BOOL result = GetGUIThreadInfo(GetWindowThreadProcessId(target_window, NULL), &info) && info.hwndCaret;
    if (!result) {
        // TODO: error log
    }
    POINT pt;
    pt.x = info.rcCaret.left;
    pt.y = info.rcCaret.top;
    ClientToScreen(info.hwndCaret, &pt);  // 转化成以整个屏幕为坐标系的坐标
    if (pt.x == 0 && pt.y == 0) {
        caretPos.first = 20;
        caretPos.second = 10;
        return caretPos;
    }
    caretPos.first = static_cast<int>(pt.x);
    caretPos.second = static_cast<int>(pt.y + 30);
    return caretPos;
}

std::pair<int, int> getCaretPosByAcc() {
    std::pair<int, int> pos = std::make_pair(100, 100);
    typedef struct {
        long x;
        long y;
        long w;
        long h;
    } Rect;

    HWND hwnd;
    DWORD pid;
    DWORD tid;

    // CoInitialize(nullptr);  // <-- add this to init COM

    GUITHREADINFO info;
    info.cbSize = sizeof(GUITHREADINFO);

    hwnd = GetForegroundWindow();
    tid = GetWindowThreadProcessId(hwnd, &pid);

    GetGUIThreadInfo(tid, &info);

    IAccessible *object = nullptr;
    if (SUCCEEDED(AccessibleObjectFromWindow(info.hwndFocus, OBJID_CARET, IID_IAccessible, (void **)&object))) {
        Rect rect;

        VARIANT varCaret;
        // varCaret.n1.n2.vt = VT_I4;
        // varCaret.n1.n2.n3.lVal = CHILDID_SELF;
        /*
            这里代码提示会报错，但是其实这个代码是正确且可以正常编译运行的
        */
        varCaret.vt = VT_I4;
        varCaret.lVal = CHILDID_SELF;
        if (SUCCEEDED(object->accLocation(&rect.x, &rect.y, &rect.w, &rect.h, varCaret))) {
            /*
                加上这个 8 主要是为了解决 VSCode 的光标捕捉在使用 vim 插件的情况下有时会不准确的问题
            */
            pos.first = rect.x + 8;
            pos.second = rect.y + rect.h;
        }

        object->Release();
    }

    // CoUninitialize();  // <-- add this to release COM
    return pos;
}