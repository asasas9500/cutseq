#include "stdafx.h"
#include "setup.h"
#include "inih/ini.h"

char* ReadLine(char* buffer, long size, FILE* fp)
{
	int c;

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
	cfg->options_number = 0;
	strcpy_s(cfg->options_camera, 200, "Camera001");
	strcpy_s(cfg->options_input, 200, "cutseq.fbx");
	strcpy_s(cfg->options_output, 200, "cutseq.bin");
	cfg->options_idx = -1;
	strcpy_s(cfg->lara_name, 200, "Lara00");
	cfg->lara_idx = -1;

	for (int i = 0; i < 9; i++)
	{
		strcpy_s(cfg->actor_name[i], 200, "Actor00");
		cfg->actor_slot[i] = 2 * i + 427;
	}

	cfg->actor_idx = -1;
}

int GetConfiguration(SETUP_STRUCT* cfg)
{
	FILE* fp;
	int r;

	r = 0;

	if (!fopen_s(&fp, "cutseq.txt", "r"))
	{
		InitialiseConfiguration(cfg);

		if (!ini_parse_stream((ini_reader)ReadLine, fp, ConfigurationHandler, cfg) && cfg->options_idx >= 0)
		{
			if (cfg->actor_idx > 8)
				cfg->actor_idx = 8;

			if (cfg->options_number < 0)
				cfg->options_number = 0;
			else if (cfg->options_number > 99)
				cfg->options_number = 99;

			for (int i = 0; i <= cfg->actor_idx; i++)
			{
				if (cfg->actor_slot[i] < 1)
					cfg->actor_slot[i] = 1;
				else if (cfg->actor_slot[i] > 519)
					cfg->actor_slot[i] = 519;
			}

			r = 1;
		}

		fclose(fp);
	}

	return r;
}
