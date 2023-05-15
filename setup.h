#pragma once
#include "types.h"

char* ReadLine(char* buf, long size, FILE* fp);
long ConfigurationHandler(void* data, const char* section, const char* entry, const char* value);
void InitialiseConfiguration(SETUP_STRUCT* cfg);
long CheckConfiguration(SETUP_STRUCT* cfg);
long ParseIntegers(const char* value, long* arr, long len);
long GetConfiguration(const char* filename, SETUP_STRUCT* cfg);
