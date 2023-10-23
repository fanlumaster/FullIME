#pragma once

#include <Windows.h>
#include <oleacc.h>

#include <utility>

#pragma comment(lib, "oleacc.lib")

/*
    通过 GUIThreadInfo 获取 caret 的坐标
*/
std::pair<int, int> getCaretPosByGUIThreadInfo();

/*
    通过 Accessible 相关的组件获取
*/
std::pair<int, int> getCaretPosByAcc();

/*
    通过系统函数获取 cursor 坐标
*/
std::pair<int, int> getCursorPosBySys();

/*
    获取当前窗口所在的屏幕的中心点的坐标
*/
std::pair<int, int> getCenterPointPosOfCurScreen();

/*
    整合的、通用的获取 caret 坐标的函数

    如果通过前两种方法无法获取到 caret 的坐标，那么，就使用 cursor 坐标来代替
*/
std::pair<int, int> getGeneralCaretPos();
