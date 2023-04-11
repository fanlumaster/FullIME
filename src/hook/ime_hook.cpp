#include "./ime_hook.h"

#include "../sqlite/sqlite_wrapper.h"
#include "../ui/candUI.h"

// 全局变量，用来捕捉键盘的字符
std::vector<char> charVec;
// 分页 map
std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, long>>>> sqlPageMap;
// 候选字的存储位置
std::vector<std::vector<std::pair<std::string, long>>> candidateVec;
std::vector<std::pair<std::string, long>> curCandidateVec;
// 候选框中的序号
int pageNo = 0;
// 左右引号的标志
bool quoteFlag = true;
// 数据库
sqlite3 *db;

// 整体输入法状态的一个控制
// 默认是 0，也就是英文状态
bool IMEState = false;
std::string IMEStateToast = "英";

// 转换字符串
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

/*
    钩子的逻辑处理
*/
LRESULT CALLBACK KBDHook(int nCode, WPARAM wParam, LPARAM lParam) {
    // 取出钩子
    KBDLLHOOKSTRUCT *s = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    switch (wParam) {
        case WM_KEYDOWN: {
            char c = MapVirtualKey(s->vkCode, MAPVK_VK_TO_CHAR);

            /*
                获取 Shift 键的状态，这个在处理一些中文的标点或者字符的时候都是需要的
            */
            BOOL fShiftDown = GetAsyncKeyState(VK_SHIFT) & 0x8000;
            BOOL fCtrlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;

            // 切换中英文的快捷键换成 Ctrl + Space
            if (fCtrlDown && s->vkCode == VK_SPACE) {
                IMEState = !IMEState;
                if (IMEStateToast == "中") {
                    IMEStateToast = "英";
                    candidateVec.clear();
                    curCandidateVec.clear();
                } else {
                    IMEStateToast = "中";
                }
                // TODO: 展示输入法现在的状态
                return 1;
            }

            if (IMEState) {
                /*
                    处理 Esc 键
                */
                if (s->vkCode == VK_ESCAPE) {
                    charVec.clear();
                    fanyHideWindow(gHwnd);
                    return 1;
                }

                /*
                    处理退格键，backspace
                */
                if (s->vkCode == VK_BACK) {
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
                        // charVec.clear();
                        // 把当前合的候选框给打印出来
                        printOneDVector(curCandidateVec);
                        return 1;
                    } else if (curSize >= 4 && curSize <= 5) {
                        charVec.pop_back();
                        std::string hanKey(charVec.begin(), charVec.end());
                        candidateVec = queryTwoPinyinInPage(db, hanKey);
                        if (candidateVec.size() > 0) {
                            curCandidateVec = candidateVec[0];
                        } else {
                            candidateVec.clear();
                            curCandidateVec.clear();
                        }
                        // charVec.clear();
                        // 把当前合的候选框给打印出来
                        printOneDVector(curCandidateVec);
                        return 1;
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
                        // charVec.clear();
                        // 把当前合的候选框给打印出来
                        printOneDVector(curCandidateVec);
                        return 1;
                    } else {
                        break;
                    }
                }

                /*
                    拼音码的处理
                */
                // 字母键
                if (std::isalpha(c)) {
                    if (fShiftDown) {
                        char upperC = std::toupper(c);
                        std::string newStr{upperC};
                        sendStringToCursor(converter.from_bytes(newStr));
                        return 1;
                    }
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
                        // charVec.clear();
                    } else if (curSize > 2 && curSize <= 4) {
                        std::string hanKey(charVec.begin(), charVec.end());
                        candidateVec = queryTwoPinyinInPage(db, hanKey);
                        if (candidateVec.size() > 0) {
                            curCandidateVec = candidateVec[0];
                        } else {
                            candidateVec.clear();
                            curCandidateVec.clear();
                        }
                        // charVec.clear();
                    } else if (curSize > 4) {
                        std::string hanKey(charVec.begin(), charVec.end());
                        candidateVec = queryMultiPinyinInPage(db, hanKey);
                        if (candidateVec.size() > 0) {
                            curCandidateVec = candidateVec[0];
                        } else {
                            candidateVec.clear();
                            curCandidateVec.clear();
                        }
                        // charVec.clear();
                    }
                    if (charVec.size() > 0) {
                        // 把当前合的候选框给打印出来
                        printOneDVector(curCandidateVec);
                    }
                    return 1;  // 1 表示不向下传递给其他钩子函数
                }

                /*
                    处理数字键和空格键上屏的问题
                    关于数字键，有些 Shift 键按下之后的中文符号的上屏也要处理

                    一些需要处理的数字键：
                    1 -> ！
                    4 -> ￥
                    6 -> ……
                    9 -> （
                    0 -> ）
                */
                // 数字键
                // fany: 这里似乎要重构一下，针对每一个键进行重构
                if (std::isdigit(c)) {
                    if (!fShiftDown) {
                        // 上屏候选项
                        if (!candidateVec.empty() && candidateVec[0].size()) {
                            // int canSize = candidateVec[0].size();
                            int canSize = curCandidateVec.size();
                            int cInt = c - '0';
                            if (cInt > canSize || cInt == 0) {
                                // 后续可以使用一个特定的状态来控制在输入法候选框出现的时候
                                // 才代替系统来处理数字键
                                return 1;
                            }
                            // 在控制台打印测试
                            // std::cout << curCandidateVec[cInt - 1].first << '\n';
                            // 输送到光标所在的地方
                            std::wstring wstr = converter.from_bytes(curCandidateVec[cInt - 1].first);
                            sendStringToCursor(wstr);
                            // 上屏了之后要把 candidateVec 给清除掉
                            candidateVec.clear();
                            curCandidateVec.clear();
                            charVec.clear();
                            pageNo = 0;
                            fanyHideWindow(gHwnd);
                            return 1;
                        }
                    } else {
                        // 一些中文标点和符号的处理
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
                        return 1;
                    }
                }

                // 空格键，只有当候选框存在的时候，空格键才会被捕捉
                if (s->vkCode == VK_SPACE) {
                    if (!candidateVec.empty() && !curCandidateVec.empty()) {
                        // 在控制台打印测试
                        // std::cout << curCandidateVec[0].first << '\n';
                        // 输送到光标所在的地方
                        std::wstring wstr = converter.from_bytes(curCandidateVec[0].first);
                        sendStringToCursor(wstr);
                        // 上屏了之后要把 candidateVec 给清除掉
                        candidateVec.clear();
                        curCandidateVec.clear();
                        // 存储拼音的 vector 也要清掉
                        charVec.clear();
                        pageNo = 0;
                        fanyHideWindow(gHwnd);
                        return 1;
                    }
                }

                /*
                    处理 + 和 - 翻页
                */
                // +
                if (s->vkCode == VK_OEM_PLUS || s->vkCode == VK_TAB) {
                    if (s->vkCode == VK_OEM_PLUS && fShiftDown) {
                        sendStringToCursor(converter.from_bytes("+"));
                        return 1;
                    }
                    if (candidateVec.size() > 1 && pageNo < candidateVec.size() - 1) {
                        pageNo += 1;
                        // std::cout << "pageNo: " << pageNo << '\n';
                        curCandidateVec = candidateVec[pageNo];
                        printOneDVector(curCandidateVec);
                    }
                    return 1;
                }

                // -
                // 同时要处理中文的破折号，——
                if (s->vkCode == VK_OEM_MINUS) {
                    if (candidateVec.size() > 1 && pageNo > 0) {
                        pageNo -= 1;
                        // std::cout << "pageNo: " << pageNo << '\n';
                        curCandidateVec = candidateVec[pageNo];
                        printOneDVector(curCandidateVec);
                        return 1;
                    }
                    if (candidateVec.size() >= 1 && pageNo == 0) {
                        curCandidateVec = candidateVec[pageNo];
                        printOneDVector(curCandidateVec);
                        return 1;
                    }
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("——"));
                    } else {
                        sendStringToCursor(converter.from_bytes("-"));
                    }
                    return 1;
                }

                /*
                    处理中文的标点符号
                */
                // 左书名号和逗号
                if (s->vkCode == VK_OEM_COMMA) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("《"));
                    } else {
                        sendStringToCursor(converter.from_bytes("，"));
                    }
                    return 1;
                }

                // 右书名号和句号
                if (s->vkCode == VK_OEM_PERIOD) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("》"));
                    } else {
                        sendStringToCursor(converter.from_bytes("。"));
                    }
                    return 1;
                }

                // 分号和冒号
                if (s->vkCode == VK_OEM_1) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("："));
                    } else {
                        sendStringToCursor(converter.from_bytes("；"));
                    }
                    return 1;
                }

                // 单引号和双引号(single-quote and double-quote)， “” 和 ‘’
                // 这里要注意左引号和右引号的处理
                if (s->vkCode == VK_OEM_7) {
                    if (quoteFlag == true) {
                        if (fShiftDown) {
                            sendStringToCursor(converter.from_bytes("“"));
                        } else {
                            sendStringToCursor(converter.from_bytes("‘"));
                        }
                    } else {
                        if (fShiftDown) {
                            sendStringToCursor(converter.from_bytes("”"));
                        } else {
                            sendStringToCursor(converter.from_bytes("’"));
                        }
                    }
                    quoteFlag = !quoteFlag;
                    return 1;
                }

                // 问号
                if (s->vkCode == VK_OEM_2) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("？"));
                    } else {
                        sendStringToCursor(converter.from_bytes("/"));
                    }
                    return 1;
                }

                // 顿号
                if (s->vkCode == VK_OEM_5) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("|"));
                    } else {
                        sendStringToCursor(converter.from_bytes("、"));
                    }
                    return 1;
                }

                // 左中括号和花括号
                if (s->vkCode == VK_OEM_4) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("{"));
                    } else {
                        sendStringToCursor(converter.from_bytes("【"));
                    }
                    return 1;
                }

                // 右中括号和花括号
                if (s->vkCode == VK_OEM_6) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("}"));
                    } else {
                        sendStringToCursor(converter.from_bytes("】"));
                    }
                    return 1;
                }

                // · 这个符号
                if (s->vkCode == VK_OEM_3) {
                    if (fShiftDown) {
                        sendStringToCursor(converter.from_bytes("~"));
                    } else {
                        sendStringToCursor(converter.from_bytes("·"));
                    }
                    return 1;
                }
            }

            break;
        }
        default:
            break;
    }

    // 继续钩子的调用链
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 模拟按下字符串中的每个字符
void sendStringToCursor(const std::wstring &str) {
    for (wchar_t ch : str) {
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = ch;
        input.ki.time = 0;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));

        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}

// 检测当前是否只有一个键被按下
// TODO: 暂时无用
bool isOnlyOneKeydown() {
    BYTE keys[256];
    int count = 0;
    bool isOnlyOne = false;

    GetKeyboardState(keys);

    for (int i = 0; i < 256; ++i) {
        if (keys[i] & 0x80) {
            ++count;
            if (count > 1) {
                break;
            }
        }
    }

    if (count == 1) {
        isOnlyOne = true;
    }

    return isOnlyOne;
}