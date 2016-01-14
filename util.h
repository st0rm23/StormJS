#ifndef _UTIL_H_
#define _UTIL_H_

#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include "global.h"

using namespace std;

class Util{
private:
    static char buf[400];

public:
    static string numtoStr(const double num);
    static double strtoNum(const string &str);
    static double formatNum(const string &str);
    static void swap(void** a, void **b);
};

#endif
