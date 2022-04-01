#pragma once
#include "types.h"

struct OPTIONS_SET
{
	long id;
	char camera[200];
	char input[200];
	char output[200];
	PHD_VECTOR origin;
	long audio;
	long idx;
};

struct LARA_SET
{
	char name[200];
	long idx;
};

struct ACTOR_SET
{
	char name[9][200];
	long slot[9];
	long idx;
};

struct SETUP_STRUCT
{
	OPTIONS_SET options;
	LARA_SET lara;
	ACTOR_SET actor;
};

char* ReadLine(char* buf, long size, FILE* fp);
int ConfigurationHandler(void* data, const char* section, const char* name, const char* value);
void InitialiseConfiguration(SETUP_STRUCT* cfg);
int CheckConfiguration(SETUP_STRUCT* cfg);
void ParseIntegers(const char* value, long* arr, long len);
int GetConfiguration(SETUP_STRUCT* cfg);
