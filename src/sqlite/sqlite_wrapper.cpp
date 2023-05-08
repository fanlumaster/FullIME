#include "./sqlite_wrapper.h"

int queryPinyinCallback(void* data, int argc, char** argv, char** azColName) {
    UserData* userData = static_cast<UserData*>(data);
    std::vector<std::pair<std::string, long>>& myVec = userData->result;
    myVec.push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
    userData->itemCount++;
    return 0;
}

sqlite3* openSqlite(std::string dbPath) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        // Todo: 异常处理
        // std::cout << "open db error!" << '\n';
        return nullptr;
    }
    // std::cout << "db opened successfully!" << '\n';
    return db;
}

/*
    单字查询
    从某一个表中模糊查询一个拼音
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 2 order by weight desc limit 50";
    // std::cout << querySQL << '\n';
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

std::vector<std::pair<std::string, long>> queryPinyinWithHelperCode(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 5 order by weight desc limit 50";
    // std::cout << querySQL << '\n';
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询两个字的条目
*/
std::vector<std::pair<std::string, long>> queryTwoPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 4 order by weight desc limit 50";
    // std::cout << querySQL << '\n';
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询三个字的条目
*/
std::vector<std::pair<std::string, long>> queryThreePinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是六个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 6 order by weight desc limit 50";
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询四个字的条目
*/
std::vector<std::pair<std::string, long>> queryFourPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是八个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 8 order by weight desc limit 50";
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询五个字的条目
*/
std::vector<std::pair<std::string, long>> queryFivePinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是10个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 10 order by weight desc limit 50";
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询超过五个字的条目
*/
std::vector<std::pair<std::string, long>> queryManyPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是10个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) > 10 order by weight desc limit 50";
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result) {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    超过两个字的处理逻辑：
        主要有三字、四字、五字、七字


        TODO:
        还可以再优化
*/
std::vector<std::pair<std::string, long>> queryMultiPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    if (pinyin.size() <= 6) {
        resVec = queryThreePinyin(db, pinyin);
    } else if (pinyin.size() <= 8) {
        resVec = queryFourPinyin(db, pinyin);
    } else if (pinyin.size() <= 10) {
        resVec = queryFivePinyin(db, pinyin);
    } else {
        resVec = queryManyPinyin(db, pinyin);
    }
    return resVec;
}

/*
    把分页的逻辑给抽离出来，暂时使用这种方法
    Todo: 后续能否使用 SQL 语句中的语法进行分页的效率改进
*/
/*
    单字查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryPinyin(db, pinyin);
    std::vector<std::pair<std::string, long>> curVec;
    int cnt = 0;
    for (auto eachEle : noPagedVec) {
        curVec.push_back(eachEle);
        cnt += 1;
        if (cnt % 8 == 0) {
            pagedVec.push_back(curVec);
            curVec.clear();
            cnt = 0;
        }
    }
    if (cnt < 8) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
    return pagedVec;
}

/*
    带有辅助码的单字查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinWithHelperCodeInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryPinyinWithHelperCode(db, pinyin);
    std::vector<std::pair<std::string, long>> curVec;
    int cnt = 0;
    for (auto eachEle : noPagedVec) {
        curVec.push_back(eachEle);
        cnt += 1;
        if (cnt % 8 == 0) {
            pagedVec.push_back(curVec);
            curVec.clear();
            cnt = 0;
        }
    }
    if (cnt < 8) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
    return pagedVec;
}

/*
    两个字的条目的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryTwoPinyinInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryTwoPinyin(db, pinyin);
    // for (auto each : noPagedVec) {
    //     std::cout << "fany => " << each.first << '\t' << each.second << '\n';
    // }
    std::vector<std::pair<std::string, long>> curVec;
    int cnt = 0;
    for (auto eachEle : noPagedVec) {
        curVec.push_back(eachEle);
        cnt += 1;
        if (cnt % 8 == 0) {
            pagedVec.push_back(curVec);
            curVec.clear();
            cnt = 0;
        }
    }
    if (cnt < 8) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
    return pagedVec;
}

std::vector<std::vector<std::pair<std::string, long>>> queryMultiPinyinInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryMultiPinyin(db, pinyin);
    std::vector<std::pair<std::string, long>> curVec;
    int cnt = 0;
    for (auto eachEle : noPagedVec) {
        curVec.push_back(eachEle);
        cnt += 1;
        if (cnt % 8 == 0) {
            pagedVec.push_back(curVec);
            curVec.clear();
            cnt = 0;
        }
    }
    if (cnt < 8) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
    return pagedVec;
}

/*
    更新词库中条目的权重

    update_sql_string = (
        Rf"update {table_name} set {column_name} = ? where key = ? and value = ?"
    )

    return:
            0 -> 更新错误
            1 -> 更新成功
*/
int updateItemWeightInDb(sqlite3* db, std::string pinyin, std::string hans, long weight) {
    std::string tblName = "fullpinyinsimple";
    int curWeight = weight + 10000;
    std::string curWeightStr = std::to_string(curWeight);
    std::string updateSQL = "update " + tblName + " set weight = " + curWeightStr + " where key = '" + pinyin + "' and value = '" + hans + "'";
    std::cout << updateSQL << '\n';

    int result;
    char* errMsg = nullptr;
    // 执行查询
    result = sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::cout << "update error!" << '\n';
        return 0;
    }
    std::cout << "udpate success" << '\n';
    return 1;
}

void closeSqliteDB(sqlite3* db) { sqlite3_close(db); }

// int main(int argc, char const* argv[]) {
//     std::string dbPath = "../../src/flyciku.db";
//     sqlite3* db = openSqlite(dbPath);
//     std::vector<std::pair<std::string, long>> resVec = queryPinyin(db, "ni");
//     std::vector<std::vector<std::pair<std::string, long>>> pagedVec = queryPinyinInPage(db, "ni");
//     for (auto fstVar : pagedVec) {
//         std::cout << fstVar.size() << '\n';
//         for (auto sndVar : fstVar) {
//             std::cout << sndVar.first << "\t" << sndVar.second << '\n';
//         }
//         std::cout << "split ===========" << '\n';
//     }
//     // for (auto eachPair : resVec) {
//     //     std::cout << eachPair.first << "\t" << eachPair.second << '\n';
//     // }
//     closeSqliteDB(db);
//     return 0;
// }
