#include "./han_char_util.h"

int calc_han_count(std::string hanText)
{
    std::string curText = hanText;
    int hanCount = 0;
    for (int i = 0; i < curText.size();)
    {
        int cplen = 1;

        if ((curText[i] & 0xf8) == 0xf0) // 四码的汉字
        {
            cplen = 4;
        }
        else if ((curText[i] & 0xf0) == 0xe0) // 三码的汉字
        {
            cplen = 3;
        }
        else if ((curText[i] & 0xe0) == 0xc0)
        {
            cplen = 2;
        }
        if ((i + cplen) > curText.length()) // 非汉字，这里应该不会用到
        {
            cplen = 1;
        }
        // std::string curHan = curText.substr(i, cplen);
        hanCount += 1;
        i += cplen;
    }
    return hanCount;
}
