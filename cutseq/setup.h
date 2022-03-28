#pragma once
#include "types.h"

struct SETUP_STRUCT
{
	long options_number;
	char options_camera[200];
	char options_input[200];
	char options_output[200];
	long options_idx;
	char lara_name[200];
	long lara_idx;
	char actor_name[9][200];
	short actor_slot[9];
	long actor_idx;
};

char* ReadLine(char* buffer, long size, FILE* fp);
int ConfigurationHandler(void* data, const char* section, const char* name, const char* value);
void InitialiseConfiguration(SETUP_STRUCT* cfg);
int CheckConfiguration(SETUP_STRUCT* cfg);
int GetConfiguration(SETUP_STRUCT* cfg);
