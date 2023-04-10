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
        std::cout << "open db error!" << '\n';
        return nullptr;
    }
    std::cout << "db opened successfully!" << '\n';
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
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 2 order by weight desc limit 200";
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询两个字的条目
*/
std::vector<std::pair<std::string, long>> queryTwoPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 4 order by weight desc limit 200";
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    超过两个字的处理逻辑：
        前两个字使用词库里面的双字词汇，
        按照四个字符一组进行切分，
        后面的拼音字符如果是三个，那么，仍然按照双字来查询，
        最好凑不出三个字符的就使用单字查询

        造出 16 个左右的词汇大概就可以了

        TODO:
        还可以再优化
*/
std::vector<std::pair<std::string, long>> queryMultiPinyin(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    int splitBegin = 0;
    std::string curPinyin;
    std::vector<std::vector<std::pair<std::string, long>>> splitVecs;
    std::vector<std::pair<std::string, long>> curVec;
    int minSize = 16;  // 16 并没有什么特殊的含意，个人规定
    while (splitBegin + 4 < pinyin.size()) {
        curPinyin = pinyin.substr(splitBegin, 4);
        curVec = queryTwoPinyin(db, curPinyin);
        splitVecs.push_back(curVec);
        splitBegin += 4;
        if (curVec.size() < minSize) {
            minSize = curVec.size();
        }
    }
    if (splitBegin < pinyin.size()) {
        curPinyin = pinyin.substr(splitBegin, 2);
        curVec = queryPinyin(db, curPinyin);
        splitVecs.push_back(curVec);
        splitBegin += 2;
        if (curVec.size() < minSize) {
            minSize = curVec.size();
        }
    }
    if (splitBegin < pinyin.size()) {
        curPinyin = pinyin.substr(splitBegin, pinyin.size());  // 这里截取的只有一个字符了
        curVec = queryPinyin(db, curPinyin);
        splitVecs.push_back(curVec);
        splitBegin += 2;
        if (curVec.size() < minSize) {
            minSize = curVec.size();
        }
    }
    for (int i = 0; i < minSize; i++) {
        std::pair<std::string, long> curPair;
        std::string curStr = "";
        long curWeight = 0;
        for (auto eachVec : splitVecs) {
            curStr += eachVec[i].first;
            curWeight += eachVec[i].second;
        }
        resVec.push_back(std::make_pair(curStr, curWeight));
    }
    return resVec;
}

/*
    把分页的逻辑给抽离出来，暂时使用这种方法
    Todo: 后续能否使用 SQL 语句中的语法进行分页的效率改进
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
        }
    }
    if (cnt < 8) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
    return pagedVec;
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
