/*
搜狗的全拼似乎限制长度是这个：
yinggaizhichiduoshaogezifunewoyebuzhidaoyanizhidaomakeshiwozhend(64)
双拼的限制似乎是这个：
ykgdviiidoucgezifuneqiuiwoyebuvidcdenividcmawovfdebvidcdekeuiv(62)
应该支持多少个字符呢其实我也不知道的你知道吗我真的不知道的课石
*/
#include "./sqlite_wrapper.h"

#include <algorithm>

/*
    处理分页
*/
void doPageVector(std::vector<std::vector<std::pair<std::string, long>>> &pagedVec,
                  std::vector<std::pair<std::string, long>> &noPagedVec)
{
    std::vector<std::pair<std::string, long>> curVec;
    int cnt = 0;
    for (auto eachEle : noPagedVec)
    {
        curVec.push_back(eachEle);
        cnt += 1;
        if (cnt % 8 == 0)
        {
            pagedVec.push_back(curVec);
            curVec.clear();
            cnt = 0;
        }
    }
    if (cnt < 8 && cnt != 0)
    {
        pagedVec.push_back(curVec);
        curVec.clear();
    }
}

int queryPinyinCallback(void *data, int argc, char **argv, char **azColName)
{
    UserData *userData = static_cast<UserData *>(data);
    std::vector<std::pair<std::string, long>> &myVec = userData->result;
    myVec.push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
    userData->itemCount++;
    return 0;
}

int simpleQueryPinyinCallback(void *data, int argc, char **argv, char **azColName)
{
    UserData *userData = static_cast<UserData *>(data);
    // std::vector<std::pair<std::string, long>>& myVec = userData->result;
    // myVec.push_back(std::make_pair(std::string(argv[1]),
    // std::stol(argv[2])));
    userData->itemCount = atoi(argv[0]);
    return SQLITE_OK;
}

sqlite3 *openSqlite(std::string dbPath)
{
    sqlite3 *db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc)
    {
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
std::vector<std::pair<std::string, long>> queryPinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 2 order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

std::vector<std::pair<std::string, long>> queryPinyinWithHelperCode(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // std::string querySQL = "select * from " + tblName + " where key like " +
    // "'" + pinyin + "%'" + " and length(key) == 5 order by weight desc limit
    // 80";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 5 order by weight desc";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询两个字的条目
*/
std::vector<std::pair<std::string, long>> queryTwoPinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // std::string querySQL = "select * from " + tblName + " where key like " +
    // "'" + pinyin + "%'" + " and length(key) == 4 order by weight desc limit
    // 80";
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 4 order by weight desc";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询三个字的条目
*/
std::vector<std::pair<std::string, long>> queryThreePinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是六个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 6 order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result)
    {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询四个字的条目
*/
std::vector<std::pair<std::string, long>> queryFourPinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是八个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 8 order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result)
    {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询五个字的条目
*/
std::vector<std::pair<std::string, long>> queryFivePinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是10个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) == 10 order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result)
    {
        // Todo: 日志
        // std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询超过五个字的条目
*/
std::vector<std::pair<std::string, long>> queryManyPinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 双拼码是10个字符
    std::string querySQL = "select * from " + tblName + " where key like " + "'" + pinyin + "%'" +
                           " and length(key) > 10 order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryMultiPinyin(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    if (pinyin.size() <= 6)
    {
        resVec = queryThreePinyin(db, pinyin);
    }
    else if (pinyin.size() <= 8)
    {
        resVec = queryFourPinyin(db, pinyin);
    }
    else if (pinyin.size() <= 10)
    {
        resVec = queryFivePinyin(db, pinyin);
    }
    else
    {
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
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinInPage(sqlite3 *db, std::string pinyin)
{
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryPinyin(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

/*
    带有辅助码的单字查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryPinyinWithHelperCodeInPage(sqlite3 *db, std::string pinyin)
{
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryPinyinWithHelperCode(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

/*
    两个字的条目的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryTwoPinyinInPage(sqlite3 *db, std::string pinyin)
{
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryTwoPinyin(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

std::vector<std::vector<std::pair<std::string, long>>> queryMultiPinyinInPage(sqlite3 *db, std::string pinyin)
{
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    std::vector<std::pair<std::string, long>> noPagedVec = queryMultiPinyin(db, pinyin);

    // 分页
    doPageVector(pagedVec, noPagedVec);

    return pagedVec;
}

/*
    更新词库中条目的权重

    update_sql_string = (
        Rf"update {table_name} set {column_name} = ? where key = ? and value =
   ?"
    )

    return:
            0 -> 更新错误
            1 -> 更新成功
*/
int updateItemWeightInDb(sqlite3 *db, std::string pinyin, std::string hans, long weight)
{
    std::string tblName = "fullpinyinsimple";
    // int curWeight = weight + 10000;
    // if ()
    long curWeight = weight;
    std::string curWeightStr = std::to_string(curWeight);
    std::string updateSQL = "update " + tblName + " set weight = " + curWeightStr + " where key = '" + pinyin +
                            "' and value = '" + hans + "'";
    // std::cout << updateSQL << '\n';

    int result;
    char *errMsg = nullptr;
    // 执行查询
    result = sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK)
    {
        std::cout << "update error!" << '\n';
        return 0;
    }
    // std::cout << "udpate success" << '\n';
    return 1;
}

bool comparePairs(const std::pair<std::string, long> &pair1, const std::pair<std::string, long> &pair2)
{
    return pair1.second > pair2.second; // 按第二个元素降序排列
}

/*
    查询 1 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>> -> value + weight
    说明：这里必须对查询出来的单字作数量上的限制。不然，如果查询出来的结果过多，vector
   的速度会拖慢这个进程的。
*/
std::vector<std::pair<std::string, long>> queryOneChar(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // 因为一个以一个字符开头的条目会很多，所以，这里只取单字，并且，只取权重最大的前
    // 80 个 select * from fullpinyinsimple where key like 'l%' and
    // length(value) == 1 order by weight desc limit 80 std::string querySQL =
    // "select * from " + tblName + " where key like " + "'" + pinyin + "%'" + "
    // and length(value) == 1 order by weight desc limit 80"; select * from
    // fullpinyinsimple where key like 'l%' and key >= 'la' and key <= 'lz' and
    // length(key) == 4 order by weight desc limit 80; auto start =
    // std::chrono::high_resolution_clock::now();
    /*
        select * from fullpinyinsimple where key like 'y%' and key >= 'ya' and
       key <= 'yz' and length(key) == 2 order by weight desc limit 80;
    */
    /*
        SELECT *
        FROM fullpinyinsimple
        WHERE key >= 'ya' AND key <= 'yz' AND key LIKE 'y%'
            AND LENGTH(key) = 2
        ORDER BY weight DESC
        LIMIT 80;
    */
    std::string querySQL = "select * from " + tblName + " where key >= '" + pinyin + "a' and key <= '" + pinyin +
                           "z' and length(key) = 2 limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    // result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback,
    // &userData, &errMsg);
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end
    // - start).count(); 输出结果 std::cout << "single 执行时间: " << duration
    // << " 毫秒" << std::endl; std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    std::sort(resVec.begin(), resVec.end(), comparePairs);
    return resVec;
}

/*
    查询 2 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryTwoChars(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    // select * from fullpinyinsimple where key like 'buu%' and length(value) ==
    // 2 order by weight desc limit 80 std::string querySQL = "select * from " +
    // tblName + " where key like " + "'" + pinyin + "%'" + " and length(key) ==
    // 2 order by weight desc limit 80";
    std::string querySQL = "select * from " + tblName + " where key  = '" + pinyin + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryThreeChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin01 = pinyin.substr(2, 1); // 切最后一个字符
    std::string pinyin02 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin02 + "[" + pinyin01 +
                           "%' and key >= '" + pinyin02 + "[" + pinyin01 + "a' and key <= '" + pinyin + "[" + pinyin01 +
                           "z' and length(key) == 5 order by weight desc limit 80";
    std::string querySQL02 = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                             "a' and key <= '" + pinyin + "z' and length(key) == 4 order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key ='" + pinyin02 + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryFourChars(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 400";
    std::string querySQL02 =
        "select * from " + tblName + " where key ='" + pinyin02 + "' order by weight desc limit 400";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryFourCharsWithHelper(sqlite3 *db, std::string pinyin)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 5 order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
    说明：
        5 个字符的时候，其实也可以把 四字成语 给提到这里面来
        6 个字符的时候，如果查不到
   三字词，那么，把四字词展示出来，那肯定是相当合理的
*/
std::vector<std::pair<std::string, long>> queryFiveChars(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 4); // 切前两个字符
    std::string pinyin03 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 6 order by weight desc limit 80";
    /*
        百步穿杨：bdbuiryh
        select * from fullpinyinsimple where key like 'bdbui%' and key >=
       'bdbuia' and key <= 'bdbuizzz'
    */
    std::string querySQL02 = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                             "a' and key <= '" + pinyin + "zzz' and length(key) == 8 order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key  = '" + pinyin03 + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 5 个字符，辅助码的情况，第三个字符是 [
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFiveCharsWithHelper(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
    说明：这里也要把四字的成语给查询出来
*/
std::vector<std::pair<std::string, long>> querySixChars(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 4); // 切前四个字符
    std::string pinyin03 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    /*
        百步穿杨：bdbuiryh
        select * from fullpinyinsimple where key like 'bdbuir%' and key >=
       'bdbuia' and key <= 'bdbuirzz'
    */
    std::string querySQL02 = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                             "a' and key <= '" + pinyin + "zz' and length(key) == 8 order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key ='" + pinyin03 + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> querySevenChars(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 6); // 切前六个字符
    std::string pinyin03 = pinyin.substr(0, 4); // 切前四个字符
    std::string pinyin04 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 8 order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04 + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryEightChars(sqlite3 *db, std::string pinyin)
{
    // std::string pinyin02 = pinyin.substr(0, 1);  // 切第一个字符
    std::string pinyin02 = pinyin.substr(0, 6); // 切前六个字符
    std::string pinyin03 = pinyin.substr(0, 4); // 切前四个字符
    std::string pinyin04 = pinyin.substr(0, 2); // 切前两个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04 + "' order by weight desc limit 80";
    // std::cout << querySQL << '\n';
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryNineChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_8 = pinyin.substr(0, 8); // 切前 8 个字符
    std::string pinyin03_6 = pinyin.substr(0, 6); // 切前 6 个字符
    std::string pinyin04_4 = pinyin.substr(0, 4); // 切前 4 个字符
    std::string pinyin05_2 = pinyin.substr(0, 2); // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 10 order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_8 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_6 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_4 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryTenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_8 = pinyin.substr(0, 8); // 切前 8 个字符
    std::string pinyin03_6 = pinyin.substr(0, 6); // 切前 6 个字符
    std::string pinyin04_4 = pinyin.substr(0, 4); // 切前 4 个字符
    std::string pinyin05_2 = pinyin.substr(0, 2); // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_8 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_6 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_4 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryElevenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin03_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin04_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin05_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin06_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 10 order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_10 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_8 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_6 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_4 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryTwelveChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin03_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin04_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin05_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin06_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_10 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_8 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_6 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_4 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryThirteenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_12 = pinyin.substr(0, 10); // 切前 12 个字符
    std::string pinyin03_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin04_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin05_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin06_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin07_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 10 order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_12 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_10 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_8 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_6 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_4 + "' order by weight desc limit 80";
    std::string querySQL07 =
        "select * from " + tblName + " where key = '" + pinyin07_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 7 次查询
    result = sqlite3_exec(db, querySQL07.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
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
std::vector<std::pair<std::string, long>> queryFourteenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_12 = pinyin.substr(0, 12); // 切前 12 个字符
    std::string pinyin03_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin04_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin05_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin06_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin07_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_12 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_10 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_8 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_6 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_4 + "' order by weight desc limit 80";
    std::string querySQL07 =
        "select * from " + tblName + " where key = '" + pinyin07_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 7 次查询
    result = sqlite3_exec(db, querySQL07.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 15 个字符
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> queryFifteenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_14 = pinyin.substr(0, 14); // 切前 14 个字符
    std::string pinyin03_12 = pinyin.substr(0, 12); // 切前 12 个字符
    std::string pinyin04_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin05_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin06_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin07_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin08_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key like '" + pinyin + "%' and key >= '" + pinyin +
                           "a' and key <= '" + pinyin + "z' and length(key) == 10 order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_14 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_12 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_10 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_8 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_6 + "' order by weight desc limit 80";
    std::string querySQL07 =
        "select * from " + tblName + " where key = '" + pinyin07_4 + "' order by weight desc limit 80";
    std::string querySQL08 =
        "select * from " + tblName + " where key = '" + pinyin08_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 7 次查询
    result = sqlite3_exec(db, querySQL07.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 8 次查询
    result = sqlite3_exec(db, querySQL08.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}

/*
    查询 16 个字符
    暂定最多就是这个数目：len("对潇潇暮雨洒江天") * 2
    参数：
        pinyin: string
    返回值：vector<vector<pair<string, long>>>
*/
std::vector<std::pair<std::string, long>> querySixteenChars(sqlite3 *db, std::string pinyin)
{
    std::string pinyin02_14 = pinyin.substr(0, 14); // 切前 14 个字符
    std::string pinyin03_12 = pinyin.substr(0, 12); // 切前 12 个字符
    std::string pinyin04_10 = pinyin.substr(0, 10); // 切前 10 个字符
    std::string pinyin05_8 = pinyin.substr(0, 8);   // 切前 8 个字符
    std::string pinyin06_6 = pinyin.substr(0, 6);   // 切前 6 个字符
    std::string pinyin07_4 = pinyin.substr(0, 4);   // 切前 4 个字符
    std::string pinyin08_2 = pinyin.substr(0, 2);   // 切前 2 个字符
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL = "select * from " + tblName + " where key = '" + pinyin + "' order by weight desc limit 80";
    std::string querySQL02 =
        "select * from " + tblName + " where key = '" + pinyin02_14 + "' order by weight desc limit 80";
    std::string querySQL03 =
        "select * from " + tblName + " where key = '" + pinyin03_12 + "' order by weight desc limit 80";
    std::string querySQL04 =
        "select * from " + tblName + " where key = '" + pinyin04_10 + "' order by weight desc limit 80";
    std::string querySQL05 =
        "select * from " + tblName + " where key = '" + pinyin05_8 + "' order by weight desc limit 80";
    std::string querySQL06 =
        "select * from " + tblName + " where key = '" + pinyin06_6 + "' order by weight desc limit 80";
    std::string querySQL07 =
        "select * from " + tblName + " where key = '" + pinyin07_4 + "' order by weight desc limit 80";
    std::string querySQL08 =
        "select * from " + tblName + " where key = '" + pinyin08_2 + "' order by weight desc limit 80";
    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第二次查询
    result = sqlite3_exec(db, querySQL02.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第三次查询
    result = sqlite3_exec(db, querySQL03.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第四次查询
    result = sqlite3_exec(db, querySQL04.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 5 次查询
    result = sqlite3_exec(db, querySQL05.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 6 次查询
    result = sqlite3_exec(db, querySQL06.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 7 次查询
    result = sqlite3_exec(db, querySQL07.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    // 第 8 次查询
    result = sqlite3_exec(db, querySQL07.c_str(), queryPinyinCallback, &userData, &errMsg);
    // std::cout << "itemCnt = " << itemCount << '\n';
    if (result)
    {
        // Todo: 日志
        std::cout << "query error!" << '\n';
    }
    return resVec;
}
/*
    所有查询的分页
*/
std::vector<std::vector<std::pair<std::string, long>>> queryCharsInPage(sqlite3 *db, std::string hankey)
{
    std::string pinyin = hankey;
    for (char &c : pinyin)
    {
        c = std::tolower(c);
    }
    // auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::pair<std::string, long>>> pagedVec;
    // std::vector<std::pair<std::string, long>> noPagedVec = queryPinyin(db,
    // pinyin);
    std::vector<std::pair<std::string, long>> noPagedVec;
    int pinyinSize = pinyin.size();
    if (pinyinSize == 1)
    {
        noPagedVec = queryOneChar(db, pinyin);
    }
    else if (pinyinSize == 2)
    {
        noPagedVec = queryTwoChars(db, pinyin);
    }
    else if (pinyinSize == 3)
    {
        noPagedVec = queryThreeChars(db, pinyin);
    }
    else if (pinyinSize == 4)
    {
        if (pinyin[2] == '[')
        {
            noPagedVec = queryFourCharsWithHelper(db, pinyin);
        }
        else
        {
            noPagedVec = queryFourChars(db, pinyin);
        }
    }
    else if (pinyinSize == 5)
    {
        if (pinyin[2] == '[')
        {
            noPagedVec = queryFiveCharsWithHelper(db, pinyin);
        }
        else
        {
            noPagedVec = queryFiveChars(db, pinyin);
        }
    }
    else if (pinyinSize == 6)
    {
        noPagedVec = querySixChars(db, pinyin);
    }
    else if (pinyinSize == 7)
    {
        noPagedVec = querySevenChars(db, pinyin);
    }
    else if (pinyinSize == 8)
    {
        noPagedVec = queryEightChars(db, pinyin);
    }
    else if (pinyinSize == 9)
    {
        noPagedVec = queryNineChars(db, pinyin);
    }
    else if (pinyinSize == 10)
    {
        noPagedVec = queryTenChars(db, pinyin);
    }
    else if (pinyinSize == 11)
    {
        noPagedVec = queryElevenChars(db, pinyin);
    }
    else if (pinyinSize == 12)
    {
        noPagedVec = queryTwelveChars(db, pinyin);
    }
    else if (pinyinSize == 13)
    {
        noPagedVec = queryThirteenChars(db, pinyin);
    }
    else if (pinyinSize == 14)
    {
        noPagedVec = queryFourteenChars(db, pinyin);
    }
    else if (pinyinSize == 15)
    {
        noPagedVec = queryFifteenChars(db, pinyin);
    }
    else if (pinyinSize == 16)
    {
        noPagedVec = querySixteenChars(db, pinyin);
    }
    // 分页
    doPageVector(pagedVec, noPagedVec);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end
    // - start).count(); 输出结果 std::cout << "page 执行时间: " << duration <<
    // " 毫秒" << std::endl;
    noPagedVec.clear();
    return pagedVec;
}

/*
    插入一个 item
*/
int insertItem(sqlite3 *db, std::string pinyin, std::string hanValue)
{
    std::vector<std::pair<std::string, long>> resVec;
    std::string tblName = "fullpinyinsimple";
    std::string querySQL =
        "select count(*) from " + tblName + " where key = '" + pinyin + "' and value = '" + hanValue + "'";
    std::string insertSQL =
        "insert into " + tblName + " (key, value, weight) values ('" + pinyin + "', '" + hanValue + "', 0)";

    std::cout << insertSQL << '\n';

    int result;
    char *errMsg = nullptr;
    int itemCount = 0;
    UserData userData{itemCount, resVec};
    // 查询
    result = sqlite3_exec(db, querySQL.c_str(), simpleQueryPinyinCallback, &userData, &errMsg);
    if (result != SQLITE_OK)
    {
        std::cout << "query error!" << '\n';
    }

    // 检查条件并执行插入操作
    if (userData.itemCount == 0)
    {
        result = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);
        if (result)
        {
            // Todo: 日志
            std::cout << "update error!" << '\n';
        }
        std::cout << "insert success!" << '\n';
    }
    return 0;
}

void closeSqliteDB(sqlite3 *db)
{
    sqlite3_close(db);
}
