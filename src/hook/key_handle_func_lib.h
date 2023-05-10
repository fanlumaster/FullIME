#include "../sqlite/sqlite_wrapper.h"
#include "../ui/cand_ui.h"

void toggleIMEState();

void handleEsc();

void handleBackspace();

void handleAlpha(char c);
// 这是给造词的时候，已经上屏前面的，然后后面的准备的
void handleAlphaByChars();
void handleAlphaByChars(char c);

void handleBackSpace();
void handleBackSpaceByChars();

//
void handleEnterByChars();

void commitCandidate(char c, int canSize, int cInt);

void handleSpace();
void handleShiftDigit(char c);

void clearCandRelative(std::string curStr, std::string hanKey);