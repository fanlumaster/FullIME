#include <string>

/*
    计算纯汉字字符串(utf8)中的汉字的个数，兼容 3 码和 4 码
*/
int calc_han_count(std::string hanText);


/*
    从一个纯汉字字符串中切出第一个汉字出来
*/
std::string han_slice_one(std::string hanText);
