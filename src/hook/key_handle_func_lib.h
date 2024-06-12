#include "sqlite/sqlite_wrapper.h"
#include "ui/cand_ui.h"

void toggleIMEState();

void handleEsc();

void handleBackspace();

void handleAlpha(char c);
// 这是给造词的时候，已经上屏前面的，然后后面的准备的
void handleAlphaByChars();
void handleAlphaByChars(char c);
// 处理有大写的情况
void handleAlphaByCharsWithCapital(char c);

// void handleBackSpace();
void handleBackSpaceByChars();

// 处理 Enter 键，直接上屏英文字符串
void handleEnterByChars();

void commitCandidate(char c, int canSize, int cInt);

// 处理空格键，把当前的第一个选项给上屏，并且，不考虑造词
void handleSpaceSimply();

// 处理空格键，把当前的第一个选项给上屏
void handleSpace();

void handleShiftDigit(char c);

void clearCandRelative(std::string curStr, std::string hanKey);
