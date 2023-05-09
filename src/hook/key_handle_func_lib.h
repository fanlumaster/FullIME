#include "../sqlite/sqlite_wrapper.h"
#include "../ui/cand_ui.h"

void toggleIMEState();

void handleEsc();

void handleBackspace();

void handleAlpha(char c);
void handleAlphaByChars(char c);

void handleBackSpace();
void handleBackSpaceByChars();

void commitCandidate(char c, int canSize, int cInt);

void handleShiftDigit(char c);
