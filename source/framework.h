#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <format>
#include <iostream>
#define sleep_ms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))


typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

typedef const char* cstr;
typedef std::string str;
