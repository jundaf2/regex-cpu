#pragma once

#include<stdint.h>
#include<string>
#include<vector>
#include <iostream>
#include<unordered_set>
#include<unordered_map>
#include "cxxtimer.hpp"
using namespace std;
pair<vector<uint32_t>, vector<uint32_t>> match_points(const string text, const char* re);
bool match_all_text(const char* text, const char* re);
void read_file(string* read, const char* a);