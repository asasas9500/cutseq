#include "stdafx.h"
#include "setup.h"
#include "inih/ini.h"

char* ReadLine(char* buffer, long size, FILE* fp)
{
	long c;

	if (fgets(buffer, size, fp))
	{
		if (!strchr(buffer, '\n'))
			do c = fgetc(fp); while (c != EOF && c != '\n');

		return buffer;
	}

	return NULL;
}

int ConfigurationHandler(void* data, const char* section, const char* name, const char* value)
{
	SETUP_STRUCT* cfg;

	cfg = (SETUP_STRUCT*)data;

	if (!_stricmp(section, "options"))
	{
		if (name == NULL && value == NULL)
			cfg->options_idx++;
		else if (cfg->options_idx < 1)
		{
			if (!_stricmp(name, "number"))
				cfg->options_number = atoi(value);
			else if (!_stricmp(name, "camera"))
				strcpy_s(cfg->options_camera, 200, value);
			else if (!_stricmp(name, "input"))
				strcpy_s(cfg->options_input, 200, value);
			else if (!_stricmp(name, "output"))
				strcpy_s(cfg->options_output, 200, value);
		}
	}
	else if (!_stricmp(section, "lara"))
	{
		if (name == NULL && value == NULL)
			cfg->lara_idx++;
		else if (cfg->lara_idx < 1)
		{
			if (!_stricmp(name, "name"))
				strcpy_s(cfg->lara_name, 200, value);
		}
	}
	else if (!_stricmp(section, "actor"))
	{
		if (name == NULL && value == NULL)
			cfg->actor_idx++;
		else if (cfg->actor_idx < 9)
		{
			if (!_stricmp(name, "name"))
				strcpy_s(cfg->actor_name[cfg->actor_idx], 200, value);
			else if (!_stricmp(name, "slot"))
				cfg->actor_slot[cfg->actor_idx] = atoi(value);
		}
	}

	return 1;
}

void InitialiseConfiguration(SETUP_STRUCT* cfg)
{
	cfg->options_number = -1;
	cfg->options_camera[0] = '\0';
	cfg->options_input[0] = '\0';
	cfg->options_output[0] = '\0';
	cfg->options_idx = -1;
	cfg->lara_name[0] = '\0';
	cfg->lara_idx = -1;

	for (int i = 0; i < 9; i++)
	{
		cfg->actor_name[i][0] = '\0';
		cfg->actor_slot[i] = -1;
	}

	cfg->actor_idx = -1;
}

int CheckConfiguration(SETUP_STRUCT* cfg)
{
	if (cfg->actor_idx > 8)
		cfg->actor_idx = 8;

	if (cfg->options_idx < 0 || cfg->options_number < 0 || cfg->options_number > 99 || cfg->options_camera[0] == '\0' || cfg->options_input[0] == '\0' ||
		cfg->options_output[0] == '\0' || (cfg->lara_idx >= 0 && cfg->lara_name[0] == '\0'))
		return 0;

	for (int i = 0; i <= cfg->actor_idx; i++)
	{
		if (cfg->actor_name[i][0] == '\0' || cfg->actor_slot[i] < 1 || cfg->actor_slot[i] > 519)
			return 0;
	}

	return 1;
}

int GetConfiguration(SETUP_STRUCT* cfg)
{
	FILE* fp;
	int r;

	r = 0;

	if (!fopen_s(&fp, "cutseq.txt", "r"))
	{
		InitialiseConfiguration(cfg);

		if (!ini_parse_stream((ini_reader)ReadLine, fp, ConfigurationHandler, cfg) && CheckConfiguration(cfg))
			r = 1;

		fclose(fp);
	}

	return r;
}
