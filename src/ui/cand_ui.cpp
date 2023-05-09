#include "./cand_ui.h"

#include "../hook/ime_hook.h"

HWND gHwnd;                                  // 窗口的句柄，提出来作为全局变量，方便后面的处理
ID2D1Factory *g_pD2DFactory = nullptr;       // 全局的 d2d 工厂
IDWriteFactory *g_pDWriteFactory = nullptr;  // 全局的 DWrite 工厂
IDWriteTextFormat *g_pDWriteTextFormat = nullptr;
ID2D1HwndRenderTarget *g_pRenderTarget = nullptr;  // 全局的 hwndRenderTarget
ID2D1SolidColorBrush *g_pBrush;                    // 全局的笔刷

std::wstring wText = L"ni'hc\n1.你好\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.可是";

// 自定义消息码
INT WM_FANY_HIDEWINDOW = RegisterWindowMessage(TEXT("MyHideWindowMessage"));
INT WM_FANY_SHOWWINDOW = RegisterWindowMessage(TEXT("MyShowWindowMessage"));
INT WM_FANY_REDRAW = RegisterWindowMessage(TEXT("MyRedrawWindowMessage"));  // 重绘

// 转换字符串
extern std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
            // case WM_PAINT:
            // FanyDrawText(hWnd, wText);
            // break;

        case WM_DESTROY:
            Cleanup();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 绘制文本
void FanyDrawText(HWND hwnd, std::wstring wText) {
    /*
        我们在重绘的时候，得重新获取一下客户区的信息，以及重新定义一下字体的信息？
        不然，字体的拉伸会导致字体的质量变得很差。
    */
    g_pDWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"zh-cn", &g_pDWriteTextFormat);
    // 获取可以绘制的客户区的长方形区域
    RECT rc;
    GetClientRect(hwnd, &rc);
    g_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &g_pRenderTarget);

    // 清除背景为白色
    // pRenderTarget->BeginDraw();
    g_pRenderTarget->BeginDraw();
    // pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    // 如果想改变输入法候选框的背景颜色，可以在这里修改
    // g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Green));
    g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // 设置绘制参数
    g_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
    g_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);

    // 绘制文本
    // ISO C++11 does not allow conversion from string literal to 'wchar_t *'
    // wchar_t *szText = L"1.你好\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.测试";
    // std::wstring wText = L"ni'hc\n1.你好\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.测试";
    wchar_t *szText = const_cast<wchar_t *>(wText.c_str());

    RECT rect;
    GetClientRect(hwnd, &rect);
    D2D1_RECT_F layoutRect = D2D1::RectF(static_cast<FLOAT>(rect.left), static_cast<FLOAT>(rect.top), static_cast<FLOAT>(rect.right), static_cast<FLOAT>(rect.bottom));
    g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &g_pBrush);
    g_pRenderTarget->DrawText(szText, wcslen(szText), g_pDWriteTextFormat, layoutRect, g_pBrush);

    // 结束绘制
    HRESULT hr = g_pRenderTarget->EndDraw();
    if (FAILED(hr)) {
        // 错误处理
        MessageBox(NULL, TEXT("结束绘制出错！"), L"error", MB_ICONERROR);
    }
    // 释放资源就不在这里操作了，而是专门在程序结束的时候统一释放
}

// 创建资源
void CreateDWResource(HWND hwnd) {
    // if (!g_pRenderTarget)
    // {
    HRESULT hr;
    // 创建单线程的 d2d 工厂
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create D2D factory failed!", L"Error", 0);
        return;
    }

    // 初始化字体
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&g_pDWriteFactory));
    g_pDWriteFactory->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 17.0f, L"zh-cn", &g_pDWriteTextFormat);

    // 获取可以绘制的客户区的长方形区域
    RECT rc;
    GetClientRect(hwnd, &rc);

    hr = g_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &g_pRenderTarget);

    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create render target failed!", L"Error", 0);
    }
    hr = g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &g_pBrush);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create brush failed!", L"Error", 0);
    }
    // }
}

// 释放资源
void Cleanup() {
    SAFE_RELEASE(g_pRenderTarget);
    SAFE_RELEASE(g_pBrush);
    SAFE_RELEASE(g_pDWriteTextFormat);
    SAFE_RELEASE(g_pDWriteFactory);
    SAFE_RELEASE(g_pD2DFactory);
}

/*
    在候选框中打印当前的候选项
*/
void printOneDVector(std::vector<std::pair<std::string, long>> myVec) {
    if (charVec.size() == 0) {
        fanyHideWindow(gHwnd);
        return;
    }
    std::string pinyinStr(charVec.begin(), charVec.end());
    wText = L"";
    wText = wText + converter.from_bytes(pinyinStr + "\n");
    for (int i = 0; i < myVec.size(); i++) {
        // std::cout << i + 1 << "." << myVec[i].first << ' ';
        wText = wText + std::to_wstring(i + 1) + L"." + converter.from_bytes(myVec[i].first);
        if (i != myVec.size() - 1) {
            wText += L"\n";
        }
    }
    PostMessage(gHwnd, WM_FANY_REDRAW, 0, 0);
    // std::cout << '\n';
}

// 隐藏窗口
void fanyHideWindow(HWND hWnd) { PostMessage(gHwnd, WM_FANY_HIDEWINDOW, 0, 0); }

// 计算新的候选框的 size
std::pair<int, int> calcCandSize(int fontSize, int charCnt) {
    int charNum = 0;
    if (charVec.size() % 2 == 0) {
        charNum = charVec.size() / 2;
    } else {
        charNum = (charVec.size() + 1) / 2;
    }
    std::pair<int, int> size;
    int width = charNum * 24 + 28;
    // we can use array to hard code this
    // int height = 288;
    // int height = (8 + 1) * (24 + 5 * 2);
    int height = 35 + (31 * curCandidateVec.size()) + 5;
    size.first = width;
    size.second = height;
    return size;
}
