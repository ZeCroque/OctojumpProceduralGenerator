#ifndef IO__H
#define IO__H

#include <shobjidl_core.h>
#include <string>

void stringToInt(std::string s, int** i);
int readIntFromKB();
PWSTR readFilePath();
void saveFile(PWSTR path, int** iMap, int iSize);

#endif


