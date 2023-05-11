#include "./sqlite_wrapper.h"

#include <chrono>

/*
    处理分页
*/
void doPageVector(std::vector<std::vector<std::pair<std::string, long>>>& pagedVec, std::vector<std::pair<std::string, long>>& noPagedVec) {
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
    if (cnt < 8 && cnt != 0) {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
}

int queryPinyinCallback(void* data, int argc, char** argv, char** azColName) {
    UserData* userData = static_cast<UserData*>(data);
    std::vector<std::pair<std::string, long>>& myVec = userData->result;
    myVec.push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
    userData->itemCount++;
    return 0;
}

int simpleQueryPinyinCallback(void* data, int argc, char** argv, char** azColName) {
    UserData* userData = static_cast<UserData*>(data);
    // std::vector<std::pair<std::string, long>>& myVec = userData->result;
    // myVec.push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
    userData->itemCount = atoi(argv[0]);
    return SQLITE_OK;
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

std::vector<std::pair<std::string, long>> queryPinyinWithHelperCode(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 5 order by weight desc limit 50";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 5 order by weight desc";
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
    // std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 4 order by weight desc limit 50";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 4 order by weight desc";
    // std::cout << querySQL << '\n';
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    std::cout << "itemCnt = " << itemCount << '\n';
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

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

/*
    带有辅助码的单字查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinWithHelperCodeInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryPinyinWithHelperCode(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

/*
    两个字的条目的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryTwoPinyinInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryTwoPinyin(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

std::vector<std::vector<std::pair<std::string, long>>> queryMultiPinyinInPage(sqlite3* db, std::string pinyin) {
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryMultiPinyin(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

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
    // int curWeight = weight + 10000;
    int curWeight = weight + 50000;
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

/*
    查询 1 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>> -> value + weight
*/
std::vector<std::pair<std::string, long>> queryOneChar(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 因为一个以一个字符开头的条目会很多，所以，这里只取单字，并且，只取权重最大的前 50 个
    // select * from fullpinyinsimple where key like 'l%' and length(value) == 1 order by weight desc limit 50
    // std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(value) == 1 order by weight desc limit 50";
    // select * from fullpinyinsimple where key like 'l%' and key >= 'la' and key <= 'lz' and length(key) == 4 order by weight desc limit 50;
    // auto start = std::chrono::high_resolution_clock::now();
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%' and key >= '" + pinyin + "a' and key <= '" + pinyin + "z' and length(key) == 2 order by weight desc limit 50";
    // std::cout << querySQL << '\n';
    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    // result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // 输出结果
    // std::cout << "single 执行时间: " << duration << " 毫秒" << std::endl;
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 2 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryTwoChars(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // select * from fullpinyinsimple where key like 'buu%' and length(value) == 2 order by weight desc limit 50
    // std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) == 2 order by weight desc limit 50";
    std::string querySQL = "select * from " + tblName + " where key  = '" + pinyin + "' order by weight desc limit 80";
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
    查询 3 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryThreeChars(sqlite3* db, std::string pinyin) {
    std::string pinyin01 = pinyin.substr(2, 1);  // 切最后一个字符
    std::string pinyin02 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin02 + "[" + pinyin01 + "%' and key >= '" + pinyin02 + "[" + pinyin01 + "a' and key <= '" + pinyin + "[" + pinyin01 + "z' and length(key) == 5 order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin + "a' and key <= '" + pinyin + "z' and length(key) == 4 order by weight desc limit 50";
    std::string querySQL03 = "select * from " + tblName + " where key ='" + pinyin02 + "' order by weight desc limit 50";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 4 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFourChars(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key ='" + pinyin02 + "' order by weight desc limit 50";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 4 个字符，辅助码的情况，第三个字符为 [
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFourCharsWithHelper(sqlite3* db, std::string pinyin) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin + "a' and key <= '" + pinyin + "z' and length(key) == 5 order by weight desc limit 50";
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
    查询 5 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFiveChars(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 4);  // 切前两个字符
    std::string pinyin03 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin + "a' and key <= '" + pinyin + "z' and length(key) == 6 order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 50";
    std::string querySQL03 = "select * from " + tblName + " where key  = '" + pinyin03 + "' order by weight desc limit 80";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 5 个字符，辅助码的情况
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFiveCharsWithHelper(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 50";
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
    查询 6 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> querySixChars(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 4);  // 切前四个字符
    std::string pinyin03 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 50";
    std::string querySQL03 = "select * from " + tblName + " where key ='" + pinyin03 + "' order by weight desc limit 50";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 7 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> querySevenChars(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 6);  // 切前六个字符
    std::string pinyin03 = pinyin.substr(0, 4);  // 切前四个字符
    std::string pinyin04 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin + "a' and key <= '" + pinyin + "z' and length(key) == 8 order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 50";
    std::string querySQL03 = "select * from " + tblName + " where key = '" + pinyin03 + "' order by weight desc limit 50";
    std::string querySQL04 = "select * from " + tblName + " where key = '" + pinyin04 + "' order by weight desc limit 50";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 8 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryEightChars(sqlite3* db, std::string pinyin) {
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 6);  // 切前六个字符
    std::string pinyin03 = pinyin.substr(0, 4);  // 切前四个字符
    std::string pinyin04 = pinyin.substr(0, 2);  // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 50";
    std::string querySQL02 = "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 50";
    std::string querySQL03 = "select * from " + tblName + " where key = '" + pinyin03 + "' order by weight desc limit 50";
    std::string querySQL04 = "select * from " + tblName + " where key = '" + pinyin04 + "' order by weight desc limit 50";
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
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result) {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 9 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryNineChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 10 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryTenChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 11 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryElevenChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 12 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryTwelveChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 13 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryThirteenChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 14 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFourteenChars(sqlite3* db, std::string pinyin) {
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
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    所有查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryCharsInPage(sqlite3* db, std::string pinyin) {
    // auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    // std::vector<std::pair<std::string, long>> noPagedVec = queryPinyin(db, pinyin);
    std::vector<std::pair<std::string, long>> noPagedVec;
    int pinyinSize = pinyin.size();
    if (pinyinSize == 1) {
        noPagedVec = queryOneChar(db, pinyin);
    } else if (pinyinSize == 2) {
        noPagedVec = queryTwoChars(db, pinyin);
    } else if (pinyinSize == 3) {
        noPagedVec = queryThreeChars(db, pinyin);
    } else if (pinyinSize == 4) {
        if (pinyin[2] == '[') {
            noPagedVec = queryFourCharsWithHelper(db, pinyin);
        } else {
            noPagedVec = queryFourChars(db, pinyin);
        }
    } else if (pinyinSize == 5) {
        if (pinyin[2] == '[') {
            noPagedVec = queryFiveCharsWithHelper(db, pinyin);
        } else {
            noPagedVec = queryFiveChars(db, pinyin);
        }
    } else if (pinyinSize == 6) {
        noPagedVec = querySixChars(db, pinyin);
    } else if (pinyinSize == 7) {
        noPagedVec = querySevenChars(db, pinyin);
    } else if (pinyinSize == 8) {
        noPagedVec = queryEightChars(db, pinyin);
    } else if (pinyinSize == 9) {
        noPagedVec = queryNineChars(db, pinyin);
    } else if (pinyinSize == 10) {
        noPagedVec = queryTenChars(db, pinyin);
    } else if (pinyinSize == 11) {
        noPagedVec = queryElevenChars(db, pinyin);
    } else if (pinyinSize == 12) {
        noPagedVec = queryTwelveChars(db, pinyin);
    } else if (pinyinSize == 13) {
        noPagedVec = queryThirteenChars(db, pinyin);
    } else if (pinyinSize == 14) {
        noPagedVec = queryFourChars(db, pinyin);
    }
    // 分页

    doPageVector(pagedVec, noPagedVec);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // 输出结果
    // std::cout << "page 执行时间: " << duration << " 毫秒" << std::endl;
    noPagedVec.clear();
    return pagedVec;
}

/*
    插入一个 item
*/
int insertItem(sqlite3* db, std::string pinyin, std::string hanValue) {
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select count(*) from " + tblName + " where key = '" + pinyin + "' and value = '" + hanValue + "'";
    std::string insertSQL = "insert into " + tblName + " (key, value, weight) values ('" + pinyin + "', '" + hanValue + "', 0)";

    int result;
    char* errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), simpleQueryPinyinCallback, &userData, &errMsg);
    if (result != SQLITE_OK) {
        std::cout << "query error!" << '\n';
    }

    // 检查条件并执行插入操作
    if (userData.itemCount == 0) {
        result = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);
        if (result) {
            // Todo: 日志
            std::cout << "update error!" << '\n';
        }
        std::cout << "insert success!" << '\n';
    }
    return 0;
}

void closeSqliteDB(sqlite3* db) { sqlite3_close(db); }
