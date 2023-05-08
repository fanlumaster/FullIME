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

struct UserData {
    int& itemCount;
    std::vector<std::pair<std::string, long>>& result;
};

// 一些工具性的函数
// 处理分页
void doPageVector(std::vector<std::vector<std::pair<std::string, long>>>& pagedVec, std::vector<std::pair<std::string, long>>& noPagedVec);

// 回调函数
int queryPinyinCallback(void* data, int argc, char** argv, char** azColName);

sqlite3* openSqlite(std::string dbPath);

std::vector<std::pair<std::string, long>> queryPinyin(sqlite3* db, std::string pinyin);

std::vector<std::pair<std::string, long>> queryPinyinWithHelperCode(sqlite3* db, std::string pinyin);

std::vector<std::pair<std::string, long>> queryTwoPinyin(sqlite3* db, std::string pinyin);

std::vector<std::pair<std::string, long>> queryMultiPinyin(sqlite3* db, std::string pinyin);

std::vector<std::vector<std::pair<std::string, long>>> queryPinyinInPage(sqlite3* db, std::string pinyin);

std::vector<std::vector<std::pair<std::string, long>>> queryPinyinWithHelperCodeInPage(sqlite3* db, std::string pinyin);

std::vector<std::vector<std::pair<std::string, long>>> queryTwoPinyinInPage(sqlite3* db, std::string pinyin);

std::vector<std::vector<std::pair<std::string, long>>> queryMultiPinyinInPage(sqlite3* db, std::string pinyin);

int updateItemWeightInDb(sqlite3* db, std::string pinyin, std::string hans, long weight);

void closeSqliteDB(sqlite3* db);
