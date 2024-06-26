#include "utils/caret_helper.h"
#include "hook/ime_hook.h"

#include <UIAutomation.h>
#include <atlbase.h>
#include <atlsafe.h>
#include <oleacc.h>

#include <iostream>

#pragma comment(lib, "Oleacc.lib")

/*
    通过 GUIThreadInfo 获取 caret 的坐标
*/
std::pair<int, int> getCaretPosByGUIThreadInfo()
{
    std::pair<int, int> caretPos(0, 0);
    HWND target_window = GetForegroundWindow();
    GUITHREADINFO info;
    info.cbSize = sizeof(GUITHREADINFO);
    BOOL result = GetGUIThreadInfo(GetWindowThreadProcessId(target_window, NULL), &info) && info.hwndCaret;
    if (!result)
    {
        // TODO: error log
    }
    POINT pt;
    pt.x = info.rcCaret.left;
    pt.y = info.rcCaret.top;
    ClientToScreen(info.hwndCaret, &pt); // 转化成以整个屏幕为坐标系的坐标
    if (pt.x == 0 && pt.y == 0)
    {
        return caretPos;
    }
    caretPos.first = pt.x + 8;
    caretPos.second = pt.y + 30;
    return caretPos;
}

/*
    通过 Accessible 相关的组件获取
*/
std::pair<int, int> getCaretPosByAcc()
{
    std::pair<int, int> caretPos(0, 0);
    typedef struct
    {
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
    if (SUCCEEDED(AccessibleObjectFromWindow(info.hwndFocus, OBJID_CARET, IID_IAccessible, (void **)&object)))
    {
        Rect rect;

        VARIANT varCaret;
        // varCaret.n1.n2.vt = VT_I4;
        // varCaret.n1.n2.n3.lVal = CHILDID_SELF;
        /*
            这里代码提示会报错，但是其实这个代码是正确且可以正常编译运行的
        */
        varCaret.vt = VT_I4;
        varCaret.lVal = CHILDID_SELF;
        if (SUCCEEDED(object->accLocation(&rect.x, &rect.y, &rect.w, &rect.h, varCaret)))
        {
            /*
                加上这个 8 主要是为了解决 VSCode 的光标捕捉在使用 vim 插件的情况下有时会不准确的问题
            */
            if (rect.x != 0 && rect.y != 0)
            {
                caretPos.first = rect.x + 8;
                caretPos.second = rect.y + rect.h;
            }
        }

        object->Release();
    }

    // CoUninitialize();  // <-- add this to release COM
    return caretPos;
}

/*
    通过系统函数获取 cursor 坐标
*/
std::pair<int, int> getCursorPosBySys()
{
    std::pair<int, int> cursorPos(0, 0);
    POINT cursorPoint;
    if (GetCursorPos(&cursorPoint))
    {
        cursorPos.first = cursorPoint.x;
        cursorPos.second = cursorPoint.y;
    }
    return cursorPos;
}

/*
    通过 IUIAutomation 的方式来获取 caret 的坐标
*/
std::pair<int, int> GetCaretPosByIUIAutomation()
{
    std::pair<int, int> caretPos(0, 0);
    long curX = 0, curY = 0, curW = 0, curH = 0;
    long *pX = &curX;
    long *pY = &curY;
    long *pW = &curW;
    long *pH = &curH;
    caretPos.first = static_cast<int>(*pX + *pW);
    caretPos.second = static_cast<int>(*pY + *pH);
    CComPtr<IUIAutomation> uia;
    CComPtr<IUIAutomationElement> eleFocus;
    CComPtr<IUIAutomationValuePattern> valuePattern;
    if (S_OK != uia.CoCreateInstance(CLSID_CUIAutomation) || uia == nullptr)
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    if (S_OK != uia->GetFocusedElement(&eleFocus) || eleFocus == nullptr)
    {
        goto useAccLocation;
    }
    if (S_OK == eleFocus->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&valuePattern)) &&
        valuePattern != nullptr)
    {
        BOOL isReadOnly;
        if (S_OK == valuePattern->get_CurrentIsReadOnly(&isReadOnly) && isReadOnly)
        {
            caretPos.first = *pX + *pW;
            caretPos.second = *pY + *pH;
            return caretPos;
        }
    }
useAccLocation:
    // use IAccessible::accLocation
    GUITHREADINFO guiThreadInfo = {sizeof(guiThreadInfo)};
    HWND hwndFocus = GetForegroundWindow();
    GetGUIThreadInfo(GetWindowThreadProcessId(hwndFocus, nullptr), &guiThreadInfo);
    hwndFocus = guiThreadInfo.hwndFocus ? guiThreadInfo.hwndFocus : hwndFocus;
    CComPtr<IAccessible> accCaret;
    if (S_OK == AccessibleObjectFromWindow(hwndFocus, OBJID_CARET, IID_PPV_ARGS(&accCaret)) && accCaret != nullptr)
    {
        CComVariant varChild = CComVariant(0);
        if (S_OK == accCaret->accLocation(pX, pY, pW, pH, varChild))
        {
            caretPos.first = *pX + *pW;
            caretPos.second = *pY + *pH;
            return caretPos;
        }
    }
    if (eleFocus == nullptr)
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    // use IUIAutomationTextPattern2::GetCaretRange
    CComPtr<IUIAutomationTextPattern2> textPattern2;
    CComPtr<IUIAutomationTextRange> caretTextRange;
    CComSafeArray<double> rects;
    void *pVal = nullptr;
    BOOL IsActive = FALSE;
    if (S_OK != eleFocus->GetCurrentPatternAs(UIA_TextPattern2Id, IID_PPV_ARGS(&textPattern2)) ||
        textPattern2 == nullptr)
    {
        goto useGetSelection;
    }
    if (S_OK != textPattern2->GetCaretRange(&IsActive, &caretTextRange) || caretTextRange == nullptr || !IsActive)
    {
        goto useGetSelection;
    }
    if (S_OK == caretTextRange->GetBoundingRectangles(rects.GetSafeArrayPtr()) && rects != nullptr &&
        SUCCEEDED(SafeArrayLock(rects)) && rects.GetCount() >= 4)
    {
        *pX = long(rects[0]);
        *pY = long(rects[1]);
        *pW = long(rects[2]);
        *pH = long(rects[3]);
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
useGetSelection:
    CComPtr<IUIAutomationTextPattern> textPattern;
    CComPtr<IUIAutomationTextRangeArray> selectionRangeArray;
    CComPtr<IUIAutomationTextRange> selectionRange;
    if (textPattern2 == nullptr)
    {
        if (S_OK != eleFocus->GetCurrentPatternAs(UIA_TextPatternId, IID_PPV_ARGS(&textPattern)) ||
            textPattern == nullptr)
        {
            caretPos.first = *pX + *pW;
            caretPos.second = *pY + *pH;
            return caretPos;
        }
    }
    else
    {
        textPattern = textPattern2;
    }
    if (S_OK != textPattern->GetSelection(&selectionRangeArray) || selectionRangeArray == nullptr)
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    int length = 0;
    if (S_OK != selectionRangeArray->get_Length(&length) || length <= 0)
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    if (S_OK != selectionRangeArray->GetElement(0, &selectionRange) || selectionRange == nullptr)
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    if (S_OK != selectionRange->GetBoundingRectangles(rects.GetSafeArrayPtr()) || rects == nullptr ||
        FAILED(SafeArrayLock(rects)))
    {
        caretPos.first = *pX + *pW;
        caretPos.second = *pY + *pH;
        return caretPos;
    }
    if (rects.GetCount() < 4)
    {
        if (S_OK != selectionRange->ExpandToEnclosingUnit(TextUnit_Character))
        {
            caretPos.first = *pX + *pW;
            caretPos.second = *pY + *pH;
            return caretPos;
        }
        if (S_OK != selectionRange->GetBoundingRectangles(rects.GetSafeArrayPtr()) || rects == nullptr ||
            FAILED(SafeArrayLock(rects)) || rects.GetCount() < 4)
        {
            caretPos.first = *pX + *pW;
            caretPos.second = *pY + *pH;
            return caretPos;
        }
    }
    *pX = long(rects[0]);
    *pY = long(rects[1]);
    *pW = long(rects[2]);
    *pH = long(rects[3]);
    /* caretPos.first = *pX + *pW; */
    caretPos.first = *pX;
    caretPos.second = *pY + *pH;
    return caretPos;
}

std::pair<int, int> getCenterPointPosOfCurScreen()
{
    // 获取当前活动窗口句柄
    HWND activeWindow = GetForegroundWindow();

    std::pair<int, int> centerPos;

    if (activeWindow)
    {
        // 获取窗口的边界矩形坐标
        RECT windowRect;
        GetWindowRect(activeWindow, &windowRect);

        // 确定窗口所在的显示器
        HMONITOR hMonitor = MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        // 计算显示器的中心坐标
        int centerX = monitorInfo.rcMonitor.left + (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) / 2;
        int centerY = monitorInfo.rcMonitor.top + (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) / 2;

        centerPos.first = centerX;
        centerPos.second = centerY;
    }
    return centerPos;
}

/*
    整合的、通用的获取 caret 坐标的函数

    如果通过前两种方法无法获取到 caret 的坐标，那么，就使用 cursor 坐标来代替，
    不对，这里是使用当前屏幕的中心点的坐标来替代

    TODO: 处理屏幕边缘的相关逻辑
*/
std::pair<int, int> getGeneralCaretPos()
{
    std::pair<int, int> caretPos;
    caretPos = getCaretPosByGUIThreadInfo();
    if (caretPos.first == 0 && caretPos.second == 0)
    {
        caretPos = getCaretPosByAcc();
    }
    if (caretPos.first == 0 && caretPos.second == 0)
    {
        caretPos = GetCaretPosByIUIAutomation();
    }
    if (caretPos.first == 0 && caretPos.second == 0)
    {
        // caretPos = getCursorPosBySys();
        caretPos = getCenterPointPosOfCurScreen(); // 既然是中心点的坐标，那么，就不需要做多余的处理
        // caretPos.first += 20;
        // caretPos.second += 30;
    }

    // std::cout << caretPos.first << '\t' << caretPos.second << '\n';
    // 在我这台 pc 上，这里其实可以根据 x 坐标判断是在哪一块屏幕
    if (caretPos.first < 3840 && caretPos.second > 1866)
    {
        caretPos.second -= 35 + (31 * curCandidateVec.size()) + 5 + 35;
    }

    if (caretPos.first > 3840 && caretPos.second > 1533)
    {
        caretPos.second -= 35 + (31 * curCandidateVec.size()) + 5 + 35;
    }
    return caretPos;
}
