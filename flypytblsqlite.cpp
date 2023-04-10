#include "./flypytblsqlite.h"

int count = 0;
std::unordered_map<std::string, std::vector<std::pair<std::string, long>>> pageMap;

std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, long>>>> transTableToMap(std::string dbPath, int pageSize) {
    // 分页后的容器
    std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, long>>>> pagedMap;
    // key: string, value: vector<pair<string, long>>
    // 重定向标准输出到文件中，此文件是和当前 cpp 文件处于同一个目录下
    // 相对路径要仔细核对
    // std::ofstream outfile("../../src/draft/testOutput.txt");
    // std::cout.rdbuf(outfile.rdbuf());

    // std::cout << "start..." << '\n';
    sqlite3 *db;
    // 为什么字符串可以这样使用，即，不给它声明具体的一段空间也可以使用。
    char *errMsg = nullptr;

    // 这里的路径要注意
    const char *c_dbPath = dbPath.c_str();
    int result = sqlite3_open(c_dbPath, &db);
    if (result != SQLITE_OK) {
        // std::cout << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return pagedMap;
    }

    // 把表头给取出来
    const char *selectSQL01 = "select * from flypinyin limit 1";
    result = sqlite3_exec(
        db, selectSQL01,
        [](void *data, int argc, char **argv, char **azColName) -> int {
            // 正式的运行过程中，不需要这个东西，这个可以留待以后设计日志输出的时候再补上
            // std::cout << azColName[0] << "\t" << azColName[1] << "\t"
            //           << azColName[2];
            // std::cout << '\n';
            return 0;
        },
        nullptr, &errMsg);
    if (result != SQLITE_OK) {
        // std::cout << "Failed to select info from table: " << sqlite3_errmsg(db) << std::endl;
        return pagedMap;
    }

    // 取出前 100 条数据，这个和下面的语句只能同时存在一个
    // const char *selectSQL = "select * from flypinyin limit 100";
    // 取出所有的数据
    const char *selectSQL = "select * from flypinyin";
    result = sqlite3_exec(
        db, selectSQL,
        // azColName 是列名数组
        // argv 是实际的值的数组
        // 注意，这里的 [] 是无法捕获局部变量的
        [](void *data, int argc, char **argv, char **azColName) -> int {
            // 注意，这里从数据库中取出来的数据列都是 char *
            // 类型的，所以要转换一下
            // 这个本来是要把权重为 0 的汉字给去掉的
            // if (std::stol(argv[2]) == 0) {
            //     return 0;
            // }
            std::string pinyin = std::string(argv[0]);
            // std::cout << argv[0] << "\t" << argv[1] << "\t" << argv[2] <<
            // '\n';
            if (pageMap.count(pinyin)) {
                pageMap[pinyin].push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
            } else {
                std::vector<std::pair<std::string, long>> curValVec;
                curValVec.push_back(std::make_pair(std::string(argv[1]), std::stol(argv[2])));
                pageMap.insert(std::make_pair(pinyin, curValVec));
            }
            count += 1;
            return 0;
        },
        nullptr, &errMsg);
    if (result != SQLITE_OK) {
        // std::cout << "Failed to select info from table: " << sqlite3_errmsg(db) << std::endl;
        return pagedMap;
    }

    // 给 pageMap 中的每一个值进行排序，这个排序在这个打印的函数里面一并处理了
    // 打印 pageMap
    for (auto it = pageMap.begin(); it != pageMap.end(); ++it) {
        // std::cout << it->first << ":\n";
        std::sort(it->second.begin(), it->second.end(), [](const auto &lhs, const auto &rhs) { return lhs.second > rhs.second; });
        // 这里不用打印了，直接排好序即可
        // for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        //     std::cout << it2->first << '\t' << it2->second << '\n';
        // }
        // std::cout << "----------------" << '\n';
        std::vector<std::vector<std::pair<std::string, long>>> curOutVec;
        std::vector<std::pair<std::string, long>> curInVec;
        int curCount = 0;
        for (auto pageIt = it->second.begin(); pageIt != it->second.end(); ++pageIt) {
            curInVec.push_back(std::make_pair(pageIt->first, pageIt->second));
            curCount += 1;
            if (curCount % pageSize == 0) {
                auto tmpInVec = curInVec;
                curOutVec.push_back(tmpInVec);
                curInVec.clear();
            }
        }
        if (curCount % pageSize != 0) {
            auto tmpInVec = curInVec;
            curOutVec.push_back(tmpInVec);
        }
        pagedMap.insert(std::make_pair(it->first, curOutVec));
    }

    sqlite3_close(db);
    // std::cout << "总条数是： " << count << '\n';
    // std::cout << "end." << '\n';
    return pagedMap;
}

// int main(int argc, char const *argv[]) {
//     std::string dbPath = "../../src/flyciku.db";
//     // std::cout << dbPath << '\n';

//     std::ofstream outfile("../../src/draft/testOutput.txt");
//     std::cout.rdbuf(outfile.rdbuf());

//     std::unordered_map<std::string, std::vector<std::pair<std::string,
//     long>>>
//         resMap = transTableToMap(dbPath);

//     for (auto it = resMap.begin(); it != resMap.end(); ++it) {
//         std::cout << it->first << ":\n";
//         // 在这里打印
//         for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
//             std::cout << it2->first << '\t' << it2->second << '\n';
//         }
//         std::cout << "----------------" << '\n';
//     }

//     return 0;
// }
