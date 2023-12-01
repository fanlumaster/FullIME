#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../libs/sqlite/sqlite3.h"

struct UserData
{
    int &itemCount;
    std::vector<std::pair<std::string, long>> &result;
};

// 一些工具性的函数
// 处理分页
void doPageVector(std::vector<std::vector<std::pair<std::string, long>>> &pagedVec,
                  std::vector<std::pair<std::string, long>> &noPagedVec);

// 回调函数
int queryPinyinCallback(void *data, int argc, char **argv, char **azColName);
// 这个函数是给 select count(*) 使用的
int simpleQueryPinyinCallback(void *data, int argc, char **argv, char **azColName);

sqlite3 *openSqlite(std::string dbPath);

// 以汉字的数量来查询
std::vector<std::pair<std::string, long>> queryPinyin(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryPinyinWithHelperCode(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryTwoPinyin(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryMultiPinyin(sqlite3 *db, std::string pinyin);
// 分页
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinInPage(sqlite3 *db, std::string pinyin);
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinWithHelperCodeInPage(sqlite3 *db, std::string pinyin);
std::vector<std::vector<std::pair<std::string, long>>> queryTwoPinyinInPage(sqlite3 *db, std::string pinyin);
std::vector<std::vector<std::pair<std::string, long>>> queryMultiPinyinInPage(sqlite3 *db, std::string pinyin);

// 更新权重
int updateItemWeightInDb(sqlite3 *db, std::string pinyin, std::string hans, long weight);

// 以拼音字符的数量来查询
std::vector<std::pair<std::string, long>> queryOneChar(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryTwoChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryThreeChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryFourChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryFiveChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> querySixChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> querySevenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryEightChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryNineChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryTenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryElevenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryTwelveChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryThirteenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryFourteenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> queryFifteenChars(sqlite3 *db, std::string pinyin);
std::vector<std::pair<std::string, long>> querySixteenChars(sqlite3 *db, std::string pinyin);
// 分页，就在这个一个函数里面处理了
std::vector<std::vector<std::pair<std::string, long>>> queryCharsInPage(sqlite3 *db, std::string pinyin);

// 插入新的条目
int insertItem(sqlite3 *db, std::string pinyin, std::string hanValue);
void closeSqliteDB(sqlite3 *db);
