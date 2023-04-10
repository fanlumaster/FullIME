#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
// #include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "./libs/sqlite/sqlite3.h"

std::unordered_map<std::string, std::vector<std::vector<std::pair<std::string, long>>>> transTableToMap(std::string dbPath, int pageSize);
