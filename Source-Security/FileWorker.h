#pragma once
#include <regex>
#include <string>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <cstdio>
#include "bits.h"

using namespace std;

bool checkFileExtension(string fileName);
vector<string> getFilesInFolder(string folder);
void addStringToTop(string fileName, string Line0, int& count);
vector<string> addTextToVector(string fileName, int& count);
void _add_antidebug(string o_code, bool& anti_debug_included, string file_name);
int copy_file(string src, string dst, bool delete_after_move);