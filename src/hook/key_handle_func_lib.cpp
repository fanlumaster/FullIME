#include "./key_handle_func_lib.h"

#include "./ime_hook.h"

void toggleIMEState() {
    IMEState = !IMEState;
    if (IMEStateToast == "中") {
        IMEStateToast = "英";
        candidateVec.clear();
        curCandidateVec.clear();
    } else {
        IMEStateToast = "中";
    }
    // 展示输入法现在的状态
    std::cout << IMEStateToast << '\n';
}

void handleEsc() {
    charVec.clear();
    fanyHideWindow(gHwnd);
}

void handleBackspace() {}

void handleAlpha(char c) {
    charVec.push_back(std::tolower(c));
    int curSize = charVec.size();
    // 处理所有符合的字符
    if (curSize > 0 && curSize <= 2) {
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0) {
            curCandidateVec = candidateVec[0];
        } else {
            candidateVec.clear();
            curCandidateVec.clear();
        }
    } else if (curSize > 2 && curSize <= 4) {
        std::string hanKey(charVec.begin(), charVec.end());
        if (charVec[2] == '[') {
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        } else {
            candidateVec = queryTwoPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
    } else if (curSize > 4) {
        std::string hanKey(charVec.begin(), charVec.end());

        if (charVec[2] == '[') {
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        } else {
            candidateVec = queryMultiPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
    }
    if (charVec.size() > 0) {
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    }
}

void handleBackSpace() {
    int curSize = charVec.size();
    if (curSize > 0 && curSize <= 3) {
        charVec.pop_back();
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0) {
            curCandidateVec = candidateVec[0];
        } else {
            candidateVec.clear();
            curCandidateVec.clear();
        }
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    } else if (curSize >= 4 && curSize <= 5) {
        if (charVec[2] == '[') {
            charVec.pop_back();
            std::string hanKey(charVec.begin(), charVec.end());
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
            // 把当前合的候选框给打印出来
            printOneDVector(curCandidateVec);
        } else {
            charVec.pop_back();
            std::string hanKey(charVec.begin(), charVec.end());
            candidateVec = queryTwoPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0) {
                curCandidateVec = candidateVec[0];
            } else {
                candidateVec.clear();
                curCandidateVec.clear();
            }
            // 把当前合的候选框给打印出来
            printOneDVector(curCandidateVec);
        }
    } else if (curSize >= 6) {
        charVec.pop_back();
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryMultiPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0) {
            curCandidateVec = candidateVec[0];
        } else {
            candidateVec.clear();
            curCandidateVec.clear();
        }
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    }
}

void commitCandidate(char c, int canSize, int cInt) {
    // 在控制台打印测试
    // std::cout << curCandidateVec[cInt - 1].first << '\n';
    // 输送到光标所在的地方
    std::wstring wstr = converter.from_bytes(curCandidateVec[cInt - 1].first);
    sendStringToCursor(wstr);
    // fany: test
    std::string curPinyin(charVec.begin(), charVec.end());
    std::cout << curPinyin << '\t' << curCandidateVec[cInt - 1].first << '\t' << curCandidateVec[cInt - 1].second << '\n';
    // 更新权重
    updateItemWeightInDb(db, curPinyin, curCandidateVec[cInt - 1].first, curCandidateVec[cInt - 1].second);

    // 上屏了之后要把 candidateVec 给清除掉
    candidateVec.clear();
    curCandidateVec.clear();
    charVec.clear();
    pageNo = 0;
    fanyHideWindow(gHwnd);
}

void handleShiftDigit(char c) {
    if (c == '1') {
        sendStringToCursor(converter.from_bytes("！"));
    } else if (c == '2') {
        sendStringToCursor(converter.from_bytes("@"));
    } else if (c == '3') {
        sendStringToCursor(converter.from_bytes("#"));
    } else if (c == '4') {
        sendStringToCursor(converter.from_bytes("￥"));
    } else if (c == '5') {
        sendStringToCursor(converter.from_bytes("%"));
    } else if (c == '6') {
        sendStringToCursor(converter.from_bytes("……"));
    } else if (c == '7') {
        sendStringToCursor(converter.from_bytes("&"));
    } else if (c == '8') {
        sendStringToCursor(converter.from_bytes("*"));
    } else if (c == '9') {
        sendStringToCursor(converter.from_bytes("（"));
    } else if (c == '0') {
        sendStringToCursor(converter.from_bytes("）"));
    }
}
