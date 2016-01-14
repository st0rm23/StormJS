#include "util.h"

char Util::buf[400];

//floating to string with erasing succeeding zero
string Util::numtoStr(const double num){
    sprintf(buf, "%.10lf", num);
    char *p1 = buf;
    while (*p1 != '\0' && *p1 != '.') p1++;
    if (p1 == '\0') return buf;     //it's an integer

    char *p2 = buf + strlen(buf) - 1;
    while (p2 != p1 && *p2 == '0') p2--;
    if (p2 == p1) *p2 = '\0';       //if there are all zero behind the floating point, then delete the zero
    else *(p2+1) = '\0';            //erease succeeding zero
    return buf;
}

double Util::strtoNum(const string &str){
    stringstream ss(str);
    double num;
    char c;
    if (!(ss >> num)) throw ExpressionException();
    if (ss >> c) throw ExpressionException();
    return num;
}

void swap(void** a, void **b){
    void* tmp = *a;
    *a = *b;
    *b = tmp;
}
