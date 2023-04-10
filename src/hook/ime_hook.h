#pragma once

#include <Windows.h>

#include <cctype>
#include <codecvt>
#include <locale>
#include <string>
#include <unordered_map>
#include <vector>

/*
    全局变量部分
*/
// 全局变量，用来捕捉键盘的字符
extern std::vector<char> charVec;
// 分页 map
extern std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, long>>>> sqlPageMap;
// 候选字的存储位置
extern std::vector<std::vector<std::pair<std::string, long>>> candidateVec;
extern std::vector<std::pair<std::string, long>> curCandidateVec;
// 候选框中的序号
extern int pageNo;
// 左右引号的标志
extern bool quoteFlag;

// 整体输入法状态的一个控制
// 默认是 0，也就是英文状态
extern bool IMEState;
extern std::string IMEStateToast;

// 转换字符串
extern std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

/*
    函数部分
*/
// 钩子处理函数
LRESULT CALLBACK KBDHook(int nCode, WPARAM wParam, LPARAM lParam);
// 模拟按下字符串中的每个字符
void sendStringToCursor(const std::wstring &str);
