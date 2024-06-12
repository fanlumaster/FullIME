#pragma once

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <cctype>
#include <codecvt>
#include <locale>
#include <string>
#include <vector>

#define SAFE_RELEASE(P)                                                                                                \
    if (P)                                                                                                             \
    {                                                                                                                  \
        P->Release();                                                                                                  \
        P = NULL;                                                                                                      \
    }

/*
    全局变量部分
*/
extern HWND gHwnd;                       // 窗口的句柄，提出来作为全局变量，方便后面的处理
extern ID2D1Factory *g_pD2DFactory;      // 全局的 d2d 工厂
extern IDWriteFactory *g_pDWriteFactory; // 全局的 DWrite 工厂
extern IDWriteTextFormat *g_pDWriteTextFormat;
extern ID2D1HwndRenderTarget *g_pRenderTarget; // 全局的 hwndRenderTarget
extern ID2D1SolidColorBrush *g_pBrush;         // 全局的笔刷

extern std::wstring wText;

// 自定义消息码
extern INT WM_FANY_HIDEWINDOW;
extern INT WM_FANY_SHOWWINDOW;
extern INT WM_FANY_REDRAW; // 重绘的信息

/*
    函数部分
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 清空候选框页面的绘制
void ClearCandUI();

// 创建 DirectWrite 的资源
void CreateDWResource(HWND hwnd);

// 绘制文本
void FanyDrawText(HWND hwnd, std::wstring wText);

void Cleanup();

// 将拼音字符串格式化，使之带有单引号分隔
std::wstring formatPinyinString(std::wstring pinyin);

// 打印一维 vector
void printOneDVector(std::vector<std::pair<std::string, long>> myVec);

/*
    绘图响应的函数
*/
// 隐藏窗口
void fanyHideWindow(HWND hWnd);
// 重绘窗口

// 计算新的候选框的 size
std::pair<int, int> calcCandSize(int fontSize, int charCnt);

// 生产拼音字符串，用于候选框的第一行的拼音展示
std::wstring producePinyinStr(std::string pinyin);
