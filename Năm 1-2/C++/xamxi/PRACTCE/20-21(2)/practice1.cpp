#include <iostream>
#include <cstring>
#include <string.h>

using namespace std;

int countSubString(char *src, char *sub)
{
    int count = 0;
    char *pos = src;
    while ((pos = strstr(pos, sub)) != NULL)
    {
        count++;
        pos += strlen(sub);
    }

    return count;
}

char *replaceStr(char *src, char *sub, char *rep)
{
    if (src == NULL)
        return NULL;

    int len_src = strlen(src);
    int len_sub = strlen(sub);
    int len_rep = strlen(rep);
    int numSub = countSubString(src, sub);
    int res_len = len_src + (len_rep - len_sub) * numSub;
    char *res = new char[res_len];
    char *des = res;
    char *pos = src;

    while ((pos = strstr(src, sub)) != NULL)
    {
        int len = pos - src;
        memcpy(des, src, len);
        des += len;
        memcpy(des, rep, len_rep);
        des += len_rep;
        src = pos + len_sub;
    }
    strcpy(des, src);
    return res;
}

int main()
{
    char *c, *sub, *rep, *res;
    c = new char[35];
    sub = new char[10];
    rep = new char[10];
    res = new char[35];

    strcpy(c, "brown fox and brown dog");
    strcpy(sub, "brown");
    strcpy(rep, "red");
    res = replaceStr(c, sub, rep);
    cout << res;
}