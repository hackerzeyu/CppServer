#pragma once
#include <cstring>
#include <ctype.h>

const char* getFileType(const char* name);
int hexToDec(char c);
void decodeMsg(char* to, char* from);