#pragma once
#include "types.h"

struct OPTIONS_SET
{
	long number;
	char camera[200];
	char input[200];
	char output[200];
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
	short slot[9];
	long idx;
};

struct SETUP_STRUCT
{
	OPTIONS_SET options;
	LARA_SET lara;
	ACTOR_SET actor;
};

char* ReadLine(char* buffer, long size, FILE* fp);
int ConfigurationHandler(void* data, const char* section, const char* name, const char* value);
void InitialiseConfiguration(SETUP_STRUCT* cfg);
int CheckConfiguration(SETUP_STRUCT* cfg);
int GetConfiguration(SETUP_STRUCT* cfg);
