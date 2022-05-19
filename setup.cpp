#include "stdafx.h"
#include "setup.h"
#include "inih/ini.h"

char* ReadLine(char* buf, long size, FILE* fp)
{
	long c;

	if (fgets(buf, size, fp))
	{
		if (!strchr(buf, '\n'))
			do c = fgetc(fp); while (c != EOF && c != '\n');

		return buf;
	}

	return NULL;
}

int ConfigurationHandler(void* data, const char* section, const char* name, const char* value)
{
	SETUP_STRUCT* cfg;

	cfg = (SETUP_STRUCT*)data;

	if (!_stricmp(section, "options"))
	{
		if (cfg->options.idx < 1)
		{
			if (name == NULL && value == NULL)
				cfg->options.idx++;
			else if (!_stricmp(name, "id"))
				ParseIntegers(value, &cfg->options.id, 1);
			else if (!_stricmp(name, "camera"))
				strcpy_s(cfg->options.camera, 200, value);
			else if (!_stricmp(name, "input"))
				strcpy_s(cfg->options.input, 200, value);
			else if (!_stricmp(name, "output"))
				strcpy_s(cfg->options.output, 200, value);
			else if (!_stricmp(name, "origin"))
				ParseIntegers(value, &cfg->options.origin.x, 3);
			else if (!_stricmp(name, "audio"))
				ParseIntegers(value, &cfg->options.audio, 1);
		}
	}
	else if (!_stricmp(section, "lara"))
	{
		if (cfg->lara.idx < 1)
		{
			if (name == NULL && value == NULL)
				cfg->lara.idx++;
			else if (!_stricmp(name, "name"))
				strcpy_s(cfg->lara.name, 200, value);
		}
	}
	else if (!_stricmp(section, "actor"))
	{
		if (cfg->actor.idx < 9)
		{
			if (name == NULL && value == NULL)
				cfg->actor.idx++;
			else if (!_stricmp(name, "name"))
				strcpy_s(cfg->actor.name[cfg->actor.idx], 200, value);
			else if (!_stricmp(name, "slot"))
				ParseIntegers(value, &cfg->actor.slot[cfg->actor.idx], 1);
		}
	}

	return 1;
}

void InitialiseConfiguration(SETUP_STRUCT* cfg)
{
	cfg->options.id = -1;
	cfg->options.camera[0] = '\0';
	cfg->options.input[0] = '\0';
	cfg->options.output[0] = '\0';
	cfg->options.origin.x = 0x7FFFFFFF;
	cfg->options.origin.y = 0x7FFFFFFF;
	cfg->options.origin.z = 0x7FFFFFFF;
	cfg->options.audio = -1;
	cfg->options.idx = -1;
	cfg->lara.name[0] = '\0';
	cfg->lara.idx = -1;

	for (int i = 0; i < 9; i++)
	{
		cfg->actor.name[i][0] = '\0';
		cfg->actor.slot[i] = -1;
	}

	cfg->actor.idx = -1;
}

int CheckConfiguration(SETUP_STRUCT* cfg)
{
	if (cfg->options.idx > 0)
		cfg->options.idx = 0;

	if (cfg->lara.idx > 0)
		cfg->lara.idx = 0;

	if (cfg->actor.idx > 8)
		cfg->actor.idx = 8;

	if (cfg->options.idx == -1 || cfg->options.id < 1 || cfg->options.id > 255 || cfg->options.camera[0] == '\0' || cfg->options.input[0] == '\0' ||
		cfg->options.output[0] == '\0' || cfg->options.origin.x < 0 || cfg->options.origin.x > 131072 || cfg->options.origin.y < -32768 ||
		cfg->options.origin.y > 32768 || cfg->options.origin.z < 0 || cfg->options.origin.z > 131072 || cfg->options.audio < -1 ||
		cfg->options.audio > 255 || (cfg->lara.idx != -1 && cfg->lara.name[0] == '\0'))
		return 0;

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		if (cfg->actor.name[i][0] == '\0' || cfg->actor.slot[i] < 1 || cfg->actor.slot[i] > 519)
			return 0;
	}

	return 1;
}

void ParseIntegers(const char* value, long* arr, long len)
{
	char* str, * tok, * ctx, * end;
	long n;

	str = _strdup(value);

	if (str)
	{
		tok = strtok_s(str, ",", &ctx);

		for (int i = 0; i < len; i++)
		{
			if (!tok)
				break;

			n = strtol(tok, &end, 10);

			if (*end == '\0')
				arr[i] = n;

			tok = strtok_s(NULL, ",", &ctx);
		}

		free(str);
	}
}

int GetConfiguration(const char* filename, SETUP_STRUCT* cfg)
{
	FILE* fp;
	int r;

	r = 0;

	if (!fopen_s(&fp, filename, "r"))
	{
		InitialiseConfiguration(cfg);

		if (!ini_parse_stream((ini_reader)ReadLine, fp, ConfigurationHandler, cfg) && CheckConfiguration(cfg))
			r = 1;

		fclose(fp);
	}

	return r;
}
