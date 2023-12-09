#include "./key_handle_func_lib.h"
#include "./ime_hook.h"
#include "../utils/han_char_util.h"

void toggleIMEState()
{
    IMEState = !IMEState;
    if (IMEStateToast == "中")
    {
        IMEStateToast = "英";
        candidateVec.clear();
        curCandidateVec.clear();
    }
    else
    {
        IMEStateToast = "中";
    }
    // 展示输入法现在的状态
    std::cout << IMEStateToast << '\n';
}

void handleEsc()
{
    charVec.clear();
    committedPinyin.clear();
    committedChars.clear();
    pageNo = 0; // 页码也要清零
    fanyHideWindow(gHwnd);
}

/*
    处理拼音码所对应的条目查询
    未把每个数量的字符一个一个的分开
*/
void handleAlpha(char c)
{
    charVec.push_back(std::tolower(c));
    int curSize = charVec.size();
    // 处理所有符合的字符
    // if (curSize == 1) {

    // }

    if (curSize > 0 && curSize <= 2)
    {
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0)
        {
            curCandidateVec = candidateVec[0];
        }
        else
        {
            candidateVec.clear();
            curCandidateVec.clear();
        }
    }
    else if (curSize > 2 && curSize <= 4)
    {
        std::string hanKey(charVec.begin(), charVec.end());
        if (charVec[2] == '[')
        {
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
        else
        {
            candidateVec = queryTwoPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
    }
    else if (curSize > 4)
    {
        std::string hanKey(charVec.begin(), charVec.end());

        if (charVec[2] == '[')
        {
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
        else
        {
            candidateVec = queryMultiPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
        }
    }
    if (charVec.size() > 0)
    {
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    }
}

/*
    处理拼音码所对应的条目查询
        把每个数量的字符一个一个的分开
*/
void handleAlphaByChars(char c)
{
    charVec.push_back(std::tolower(c));
    // 处理所有符合的字符
    std::string hanKey(charVec.begin(), charVec.end());
    // auto start = std::chrono::high_resolution_clock::now();
    candidateVec = queryCharsInPage(db, hanKey);
    // std::cout << "candidateVec capacity" << candidateVec.capacity() << '\n';
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end -
    // start).count(); 输出结果 std::cout << "Query execution time: " << duration
    // << "ms" << '\n';

    if (candidateVec.size() > 0)
    {
        curCandidateVec = candidateVec[0];
    }
    else
    {
        candidateVec.clear();
        curCandidateVec.clear();
    }
    // 把当前的候选框给打印出来
    // start = std::chrono::high_resolution_clock::now();
    printOneDVector(curCandidateVec);
    // end = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(end -
    // start).count(); 输出结果 std::cout << "UI execution time: " << duration <<
    // "ms" << '\n';
}

/*
    大写的情况
*/
void handleAlphaByCharsWithCapital(char c)
{
    // 本身 c 就是大写的
    charVec.push_back(c);
    // 处理所有符合的字符
    std::string hanKey(charVec.begin(), charVec.end());
    // auto start = std::chrono::high_resolution_clock::now();
    candidateVec = queryCharsInPage(db, hanKey);
    // std::cout << "candidateVec capacity" << candidateVec.capacity() << '\n';
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end -
    // start).count(); 输出结果 std::cout << "Query execution time: " << duration
    // << "ms" << '\n';

    if (candidateVec.size() > 0)
    {
        curCandidateVec = candidateVec[0];
    }
    else
    {
        candidateVec.clear();
        curCandidateVec.clear();
    }
    // 把当前的候选框给打印出来
    // start = std::chrono::high_resolution_clock::now();
    printOneDVector(curCandidateVec);
    // end = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(end -
    // start).count(); 输出结果 std::cout << "UI execution time: " << duration <<
    // "ms" << '\n';
}

void handleAlphaByChars()
{
    // 处理所有符合的字符
    std::string hanKey(charVec.begin(), charVec.end());
    candidateVec = queryCharsInPage(db, hanKey);
    if (candidateVec.size() > 0)
    {
        curCandidateVec = candidateVec[0];
    }
    else
    {
        candidateVec.clear();
        curCandidateVec.clear();
    }
    // 把当前的候选框给打印出来
    printOneDVector(curCandidateVec);
}

void handleBackspace()
{
    int curSize = charVec.size();
    if (curSize > 0 && curSize <= 3)
    {
        charVec.pop_back();
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0)
        {
            curCandidateVec = candidateVec[0];
        }
        else
        {
            candidateVec.clear();
            curCandidateVec.clear();
        }
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    }
    else if (curSize >= 4 && curSize <= 5)
    {
        if (charVec[2] == '[')
        {
            charVec.pop_back();
            std::string hanKey(charVec.begin(), charVec.end());
            candidateVec = queryPinyinWithHelperCodeInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
            // 把当前合的候选框给打印出来
            printOneDVector(curCandidateVec);
        }
        else
        {
            charVec.pop_back();
            std::string hanKey(charVec.begin(), charVec.end());
            candidateVec = queryTwoPinyinInPage(db, hanKey);
            if (candidateVec.size() > 0)
            {
                curCandidateVec = candidateVec[0];
            }
            else
            {
                candidateVec.clear();
                curCandidateVec.clear();
            }
            // 把当前合的候选框给打印出来
            printOneDVector(curCandidateVec);
        }
    }
    else if (curSize >= 6)
    {
        charVec.pop_back();
        std::string hanKey(charVec.begin(), charVec.end());
        candidateVec = queryMultiPinyinInPage(db, hanKey);
        if (candidateVec.size() > 0)
        {
            curCandidateVec = candidateVec[0];
        }
        else
        {
            candidateVec.clear();
            curCandidateVec.clear();
        }
        // 把当前合的候选框给打印出来
        printOneDVector(curCandidateVec);
    }
}

/*
    按照字符的数量，一个一个地来处理
    其实是直接交给了后面的 sqlite 包装程序
        这里不需要考虑 pinyin 字符串是否大于 0 的问题，在上层已经考虑好了
*/
void handleBackSpaceByChars()
{
    charVec.pop_back();
    std::string hanKey(charVec.begin(), charVec.end());
    // std::cout << "fany test backspace -> " << hanKey << '\n';
    candidateVec = queryCharsInPage(db, hanKey);
    if (candidateVec.size() > 0)
    {
        curCandidateVec = candidateVec[0];
    }
    else
    {
        candidateVec.clear();
        curCandidateVec.clear();
    }
    // 把当前合的候选框给打印出来
    printOneDVector(curCandidateVec);
}

/*

*/
void handleEnterByChars()
{
    std::string curPinyin(charVec.begin(), charVec.end());
    std::wstring wCurPinyin = converter.from_bytes(curPinyin);
    sendStringToCursor(wCurPinyin);
    // 清理
    candidateVec.clear();
    curCandidateVec.clear();
    pageNo = 0;
    charVec.clear();
    fanyHideWindow(gHwnd);
}

void commitCandidate(char c, int canSize, int cInt)
{
    // 输送到光标所在的地方
    std::wstring wstr = converter.from_bytes(curCandidateVec[cInt - 1].first);
    sendStringToCursor(wstr);
    std::string curPinyin(charVec.begin(), charVec.end());
    int index = 0;
    // 此处是更新一个更新权重的问题
    int cIntOfAllCand = 8 * pageNo + cInt;
    if (cIntOfAllCand - 1 > 2)
    {
        index = 2;
    }
    else if (cIntOfAllCand - 1 > 1)
    {
        index = 1;
    }
    else if (cIntOfAllCand - 1 > 0)
    {
        index = 0;
    }
    // 我取的应该是所有的查询结果里面的最前面的呀，嗯，暂时又没发现问题了。
    long weight = candidateVec[0][index].second + 1;
    updateItemWeightInDb(db, curPinyin, curCandidateVec[cInt - 1].first, weight);
    std::string curStr = curCandidateVec[cInt - 1].first;
    clearCandRelative(curStr, curPinyin);
}

void handleSpaceSimply()
{
    // 把候选框中的条目输送到光标所在的地方
    std::string curStr = curCandidateVec[0].first;
    std::string hanKey(charVec.begin(), charVec.end());
    std::wstring wstr = converter.from_bytes(curStr);
    sendStringToCursor(wstr);

    // 清理工作就直接在这里处理，而不是另外写函数
    candidateVec.clear();
    curCandidateVec.clear();
    pageNo = 0;
    // 清除存储拼音的 vector
    charVec.clear();
    fanyHideWindow(gHwnd);
}

void handleSpace()
{
    // 输送到光标所在的地方
    std::string curStr = curCandidateVec[0].first;
    std::string hanKey(charVec.begin(), charVec.end());
    std::wstring wstr = converter.from_bytes(curStr);
    sendStringToCursor(wstr);
    // 上屏了之后要把 candidateVec 给清除掉
    // std::cout << "fany here -1" << '\n';
    clearCandRelative(curStr, hanKey);
}

// 按下 shift 的时候，且是中文状态时的处理
void handleShiftDigit(char c)
{
    if (candidateVec.size() > 1)
    {
        handleSpaceSimply(); // 模拟空格，上屏第一个选项
    }

    if (c == '1')
    {
        sendStringToCursor(converter.from_bytes("！"));
    }
    else if (c == '2')
    {
        sendStringToCursor(converter.from_bytes("@"));
    }
    else if (c == '3')
    {
        sendStringToCursor(converter.from_bytes("#"));
    }
    else if (c == '4')
    {
        sendStringToCursor(converter.from_bytes("￥"));
    }
    else if (c == '5')
    {
        sendStringToCursor(converter.from_bytes("%"));
    }
    else if (c == '6')
    {
        sendStringToCursor(converter.from_bytes("……"));
    }
    else if (c == '7')
    {
        sendStringToCursor(converter.from_bytes("&"));
    }
    else if (c == '8')
    {
        sendStringToCursor(converter.from_bytes("*"));
    }
    else if (c == '9')
    {
        sendStringToCursor(converter.from_bytes("（"));
    }
    else if (c == '0')
    {
        sendStringToCursor(converter.from_bytes("）"));
    }
}

// 上屏之后的清理候选框相关的参数
/*
    上屏之后，有两种情况：
    - 1、清理原来的状态
    - 2、造词
*/
void clearCandRelative(std::string curStr, std::string hanKey)
{
    candidateVec.clear();
    curCandidateVec.clear();
    pageNo = 0;
    EntireHelpCodeFlag = 0; // 完整辅助码支持状态清零
    // 要注意，这里一个汉字的 size 是 3 或者 4!
    // 候选框的 (汉字的数量 * 2) 小于拼音字符的数量
    // 以及，三码不造字、五码也不造字
    // TODO: 目前这里只能插入三个字节的汉字，对于 unicode 表中靠后的一些字暂时没去处理
    if (calc_han_count(curStr) * 2 < hanKey.size() && hanKey.size() != 3 && hanKey.size() != 5 && hanKey.size() != 4)
    {
        committedChars.push_back(curStr);
        committedPinyin.push_back(hanKey);
        // 这个擦除以后可以自动把 size 变成缩小后的程度
        charVec.erase(charVec.begin(), charVec.begin() + curStr.size() / 3 * 2);
        handleAlphaByChars();
    }
    else if (hanKey.size() == 4 && EntireHelpCodeFlag)
    {
        // 存储拼音的 vector 也要清掉
        charVec.clear();
        fanyHideWindow(gHwnd);
    }
    else
    {
        if (committedChars.size() > 0)
        {
            committedChars.push_back(curStr);
            committedPinyin.push_back(hanKey);
            std::string preInsertStr = "";
            std::string preInsertPinyin = "";
            for (std::string eachStr : committedChars)
            {
                preInsertStr += eachStr;
            }
            // TODO: 插入新的条目，这里应该插入完整的拼音，而，
            // 像 ruhen -> 如何拿，这是不对的，这种情况通常是因为少了一个字母导致的
            // 解决方法：1. 可以在插入的时候进行校验。
            //           如果不是拼音和汉字之间不能成对应的关系的话，就不插入
            //           2. 可以在这里就进行修正，如果对应不上，而对应不上一定是因为少了一个字母
            //           怎么把那个缺失的字母给补上呢？
            insertItem(db, committedPinyin[0], preInsertStr);
            // 清空造词用到的 vector
            committedChars.clear();
            committedPinyin.clear();
        }
        // 存储拼音的 vector 也要清掉
        charVec.clear();
        fanyHideWindow(gHwnd);
    }
}
