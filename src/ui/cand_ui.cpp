#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "ui/cand_ui.h"
#include "hook/ime_hook.h"

#include <dwmapi.h>

HWND gHwnd;                            // 窗口的句柄，提出来作为全局变量，方便后面的处理
ID2D1Factory *g_pD2DFactory = nullptr; // 全局的 d2d 工厂
ID2D1HwndRenderTarget *g_pRenderTarget = nullptr; // 全局的 hwndRenderTarget
ID2D1SolidColorBrush *g_pBrush = nullptr;         // 全局的笔刷
IDWriteFactory *g_pDWriteFactory = nullptr;       // 全局的 DWrite 工厂
IDWriteTextFormat *g_pDWriteTextFormat = nullptr;
IDWriteTextLayout *g_pDWriteLayout = nullptr;

std::wstring wText = L"";

// 自定义消息码
INT WM_FANY_HIDEWINDOW = RegisterWindowMessage(TEXT("MyHideWindowMessage"));
INT WM_FANY_SHOWWINDOW = RegisterWindowMessage(TEXT("MyShowWindowMessage"));
INT WM_FANY_REDRAW = RegisterWindowMessage(TEXT("MyRedrawWindowMessage")); // 重绘

// 转换字符串
// extern std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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

void ClearCandUI()
{
    // 开始绘制
    g_pRenderTarget->BeginDraw();
    // 清除背景为透明，最后一个通道是 alpha 通道，设置为 0 的时候，就是全透明
    g_pRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));
    // 结束绘制
    g_pRenderTarget->EndDraw();
}

// 创建资源
void CreateDWResource(HWND hwnd)
{
    MARGINS mar = {-1};
    DwmExtendFrameIntoClientArea(gHwnd, &mar);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &g_pD2DFactory);

    g_pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                     D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(306, 406), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        &g_pRenderTarget);
    g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &g_pBrush);
    g_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown **>(&g_pDWriteFactory));
    // std::wstring fontname = L"微软雅黑";
    std::wstring fontname = L"思源黑体";
    g_pDWriteFactory->CreateTextFormat(fontname.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                       DWRITE_FONT_STRETCH_NORMAL, 17.0f, L"zh-cn", &g_pDWriteTextFormat);
}

// 绘制文本
void FanyDrawText(HWND hwnd, std::wstring wText)
{
    // 开始绘制
    g_pRenderTarget->BeginDraw();
    // 清除背景为透明，最后一个通道是 alpha 通道，设置为 0 的时候，就是全透明
    g_pRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

    RECT re;
    GetClientRect(hwnd, &re);
    FLOAT dpix, dpiy;
    dpix = static_cast<float>(re.right - re.left);
    dpiy = static_cast<float>(re.bottom - re.top);
    HRESULT res = g_pDWriteFactory->CreateTextLayout(wText.c_str(), wText.length() + 1, g_pDWriteTextFormat, dpix, dpiy,
                                                     &g_pDWriteLayout);

    if (SUCCEEDED(res))
    {
        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = wText.length();
        g_pDWriteLayout->SetFontSize(17, range);

        g_pBrush->SetColor(D2D1::ColorF(32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f));
        D2D1_RECT_F rect = D2D1::RectF(0.0f, 0.0f, 118.0f, 212.0f);
        D2D1_ROUNDED_RECT rounded_rect = D2D1::RoundedRect(rect, 8, 8);
        g_pRenderTarget->FillRoundedRectangle(rounded_rect, g_pBrush);

        g_pBrush->SetColor(D2D1::ColorF(229.0f / 255.0f, 229.0f / 255.0f, 229.0f / 255.0f, 1.0f));
        g_pRenderTarget->DrawTextLayout(D2D1::Point2F(10, 3), g_pDWriteLayout, g_pBrush);
        g_pDWriteLayout->Release();
        g_pDWriteLayout = NULL;
    }
    g_pRenderTarget->EndDraw();
    // 释放资源就不在这里操作了，而是专门在程序结束的时候统一释放，这是不行的，必须在这里释放，不然，memory
    // leak 有你好受的！ Cleanup();
    // SAFE_RELEASE(g_pDWriteTextFormat);
    // SAFE_RELEASE(g_pBrush);
    // SAFE_RELEASE(g_pRenderTarget);
}

// 释放资源
void Cleanup()
{
    SAFE_RELEASE(g_pRenderTarget);
    SAFE_RELEASE(g_pBrush);
    SAFE_RELEASE(g_pDWriteTextFormat);
    SAFE_RELEASE(g_pDWriteFactory);
    SAFE_RELEASE(g_pD2DFactory);
}

std::wstring formatPinyinString(std::wstring pinyin)
{
    std::wstring formattedPinyin = L"";

    int count = 0;
    for (int i = 0; i < pinyin.length() - 1; i++)
    {
        count += 1;
        formattedPinyin = formattedPinyin + pinyin[i];
        if (count % 2 == 0)
        {
            formattedPinyin += L"'";
        }
    }
    formattedPinyin += pinyin[count];
    return formattedPinyin;
}

/*
    在候选框中打印当前的候选项
*/
void printOneDVector(std::vector<std::pair<std::string, long>> myVec)
{
    if (charVec.size() == 0)
    {
        fanyHideWindow(gHwnd);
        return;
        // wText =
        // L"ni'hc\n1.你好\n2.世界\n3.毛笔\n4.量子\n5.笔画\n6.竟然\n7.什么\n8.可是";
    }
    else
    {
        std::string pinyinStr(charVec.begin(), charVec.end());
        wText = L""; // 注意，这也是一个全局变量，这里要 clear 一下

        wText += converter.from_bytes(pinyinStr);
        wText = formatPinyinString(wText);
        wText += L"\n";
        for (int i = 0; i < myVec.size(); i++)
        {
            // std::cout << i + 1 << "." << myVec[i].first << ' ';
            wText = wText + std::to_wstring(i + 1) + L"." + converter.from_bytes(myVec[i].first);
            if (i != myVec.size() - 1)
            {
                wText += L"\n";
            }
        }
    }
    PostMessage(gHwnd, WM_FANY_REDRAW, 0, 0);
    // std::cout << '\n';
}

// 隐藏窗口
void fanyHideWindow(HWND hWnd)
{
    PostMessage(gHwnd, WM_FANY_HIDEWINDOW, 0, 0);
    ClearCandUI();
}

// 计算新的候选框的 size
std::pair<int, int> calcCandSize(int fontSize, int charCnt)
{
    int charNum = 0;
    if (charVec.size() % 2 == 0)
    {
        charNum = charVec.size() / 2;
    }
    else
    {
        charNum = (charVec.size() + 1) / 2;
    }
    // std::cout << "first => " << charNum << '\n';
    for (int i = 0; i < curCandidateVec.size(); i++)
    {
        if (curCandidateVec[i].first.length() / 3 > charNum)
        {
            charNum = curCandidateVec[i].first.length() / 3;
        }
    }
    // std::cout << "second => " << charNum << '\n';
    std::pair<int, int> size;
    int width = charNum * 24 + 28;
    // we can use array to hard code this
    // int height = 288;
    // int height = (8 + 1) * (24 + 5 * 2);
    int height = 35 + (31 * curCandidateVec.size()) + 5;
    size.first = width;
    size.second = height;

    size.first = 306;
    size.second = 406;
    return size;
}
