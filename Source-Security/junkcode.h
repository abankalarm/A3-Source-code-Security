#pragma once
#include <string>
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>

const char* get_size1(int& rem_size);
const char* get_size2(int& rem_size);
const char* get_size3(int& rem_size);
const char* get_size4(int& rem_size);
std::string get_junk(int size, int& junk_counter);