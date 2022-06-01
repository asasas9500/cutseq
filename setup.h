#pragma once
#include "types.h"

char* ReadLine(char* buf, long size, FILE* fp);
int ConfigurationHandler(void* data, const char* section, const char* name, const char* value);
void InitialiseConfiguration(SETUP_STRUCT* cfg);
int CheckConfiguration(SETUP_STRUCT* cfg);
int ParseIntegers(const char* value, long* arr, long len);
int GetConfiguration(const char* filename, SETUP_STRUCT* cfg);
