#pragma once

#include<iostream>
#include<random>
#include<ctime>
#include<string>
#include<stack>
#include<vector>
#include<iomanip>
#include <cstring>
#include "config.h"
#include "SuperBlock.h"

using namespace std;

static string getCurrTime() {
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time, &time_seconds);
	string time = to_string(now_time.tm_year + 1900)
		+ "/"
		+ to_string(now_time.tm_mon + 1)
		+ "/"
		+ to_string(now_time.tm_mday)
		+ " "
		+ to_string(now_time.tm_hour) + ":"
		+ to_string(now_time.tm_min) + ":"
		+ to_string(now_time.tm_sec);
	return time;
}
static int bits2Int(char chs[4])
{
	int res = 0;
	for (int i = 0; i < 4; i++) {
		int x = chs[i];
		res <<= 8 * sizeof(char);
		res += x;
	}
	return res;
}

static void int2Bits(char chs[4], int x)
{
	constexpr int mask = 0x000000FF;
	for (int i = 3; i >= 0; i--) {
		chs[i] = mask & x;
		x >>= 8 * sizeof(char);
	}
}
