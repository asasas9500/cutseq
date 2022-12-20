#include "stdafx.h"
#include "setup.h"
#include "main.h"
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

long ConfigurationHandler(void* data, const char* section, const char* name, const char* value)
{
	SETUP_STRUCT* cfg;

	cfg = (SETUP_STRUCT*)data;

	if (!_stricmp(section, "options"))
	{
		if (cfg->options.idx < 1)
		{
			if (name == NULL && value == NULL)
				cfg->options.idx++;
			else if (!_stricmp(name, "number"))
			{
				if (!cfg->options.number)
				{
					cfg->options.number = (long*)malloc(sizeof(long));

					if (!cfg->options.number)
						return 0;

					if (!ParseIntegers(value, cfg->options.number, 1))
					{
						ShowError("Cutscene number must be an integer");
						return 0;
					}

					if (*cfg->options.number < 1 || *cfg->options.number > 255)
					{
						ShowError("Cutscene number must be between 1 and 255");
						return 0;
					}
				}
			}
			else if (!_stricmp(name, "camera"))
			{
				if (!cfg->options.camera)
				{
					cfg->options.camera = _strdup(value);

					if (!cfg->options.camera)
						return 0;

					if (cfg->options.camera[0] == '\0')
					{
						ShowError("Camera node name must be non-empty");
						return 0;
					}
				}
			}
			else if (!_stricmp(name, "origin"))
			{
				if (!cfg->options.origin)
				{
					cfg->options.origin = (PHD_VECTOR*)malloc(sizeof(PHD_VECTOR));

					if (!cfg->options.origin)
						return 0;

					if (!ParseIntegers(value, (long*)cfg->options.origin, 3))
					{
						ShowError("Origin must be composed of three integers separated by commas");
						return 0;
					}

					if (cfg->options.origin->x < 0 || cfg->options.origin->x > 131071)
					{
						ShowError("X coordinate of origin must be between 0 and 131071");
						return 0;
					}

					if (cfg->options.origin->y < -32768 || cfg->options.origin->y > 32767)
					{
						ShowError("Y coordinate of origin must be between -32768 and 32767");
						return 0;
					}

					if (cfg->options.origin->z < 0 || cfg->options.origin->z > 131071)
					{
						ShowError("Z coordinate of origin must be between 0 and 131071");
						return 0;
					}
				}
			}
			else if (!_stricmp(name, "audio"))
			{
				if (!cfg->options.audio)
				{
					cfg->options.audio = (long*)malloc(sizeof(long));

					if (!cfg->options.audio)
						return 0;

					if (!ParseIntegers(value, cfg->options.audio, 1))
					{
						ShowError("Audio must be an integer");
						return 0;
					}

					if (*cfg->options.audio < 0 || *cfg->options.audio > 255)
					{
						ShowError("Audio must be between 0 and 255");
						return 0;
					}
				}
			}
		}
	}
	else if (!_stricmp(section, "lara"))
	{
		if (cfg->lara.idx < 1)
		{
			if (name == NULL && value == NULL)
				cfg->lara.idx++;
			else if (!_stricmp(name, "name"))
			{
				if (!cfg->lara.name)
				{
					cfg->lara.name = _strdup(value);

					if (!cfg->lara.name)
						return 0;

					if (cfg->lara.name[0] == '\0')
					{
						ShowError("Lara node name must be non-empty");
						return 0;
					}
				}
			}
		}
	}
	else if (!_stricmp(section, "actor"))
	{
		if (cfg->actor.idx < 9)
		{
			if (name == NULL && value == NULL)
				cfg->actor.idx++;
			else if (!_stricmp(name, "name"))
			{
				if (!cfg->actor.name[cfg->actor.idx])
				{
					cfg->actor.name[cfg->actor.idx] = _strdup(value);

					if (!cfg->actor.name[cfg->actor.idx])
						return 0;

					if (cfg->actor.name[cfg->actor.idx][0] == '\0')
					{
						ShowError("Actor %d node name must be non-empty", cfg->actor.idx + 1);
						return 0;
					}
				}
			}
			else if (!_stricmp(name, "slot"))
			{
				if (!cfg->actor.slot[cfg->actor.idx])
				{
					cfg->actor.slot[cfg->actor.idx] = (long*)malloc(sizeof(long));

					if (!cfg->actor.slot[cfg->actor.idx])
						return 0;

					if (!ParseIntegers(value, cfg->actor.slot[cfg->actor.idx], 1))
					{
						ShowError("Actor %d slot must be an integer", cfg->actor.idx + 1);
						return 0;
					}

					if (*cfg->actor.slot[cfg->actor.idx] < 1 || *cfg->actor.slot[cfg->actor.idx] > 519)
					{
						ShowError("Actor %d slot must be between 1 and 519", cfg->actor.idx + 1);
						return 0;
					}
				}
			}
		}
	}

	return 1;
}

void InitialiseConfiguration(SETUP_STRUCT* cfg)
{
	cfg->options.number = NULL;
	cfg->options.camera = NULL;
	cfg->options.origin = NULL;
	cfg->options.audio = NULL;
	cfg->options.idx = -1;
	cfg->lara.name = NULL;
	cfg->lara.idx = -1;

	for (int i = 0; i < 9; i++)
	{
		cfg->actor.name[i] = NULL;
		cfg->actor.slot[i] = NULL;
	}

	cfg->actor.idx = -1;
}

long CheckConfiguration(SETUP_STRUCT* cfg)
{
	if (cfg->options.idx > 0)
		cfg->options.idx = 0;

	if (cfg->lara.idx > 0)
		cfg->lara.idx = 0;

	if (cfg->actor.idx > 8)
		cfg->actor.idx = 8;

	if (cfg->options.idx == -1)
	{
		ShowError("Options section must be present");
		return 0;
	}

	if (!cfg->options.number)
	{
		ShowError("Cutscene number must be informed");
		return 0;
	}

	if (!cfg->options.camera)
	{
		ShowError("Camera node name must be informed");
		return 0;
	}

	if (cfg->lara.idx != -1 && !cfg->lara.name)
	{
		ShowError("Lara node name must be informed");
		return 0;
	}

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		if (!cfg->actor.name[i])
		{
			ShowError("Actor %d node name must be informed", i + 1);
			return 0;
		}

		if (!cfg->actor.slot[i])
		{
			ShowError("Actor %d slot must be informed", i + 1);
			return 0;
		}
	}

	return 1;
}

long ParseIntegers(const char* value, long* arr, long len)
{
	char* str;
	char* tok;
	char* ctx;
	char* end;
	long n, r;

	r = 1;
	str = _strdup(value);

	if (str)
	{
		tok = strtok_s(str, ",", &ctx);

		for (int i = 0; i < len; i++)
		{
			if (!tok)
			{
				r = 0;
				break;
			}

			n = strtol(tok, &end, 10);

			if (*end != '\0')
			{
				r = 0;
				break;
			}

			arr[i] = n;
			tok = strtok_s(NULL, ",", &ctx);
		}

		if (tok)
			r = 0;

		free(str);
	}
	else
		r = 0;

	return r;
}

void FreeConfiguration(SETUP_STRUCT* cfg)
{
	free(cfg->options.number);
	free(cfg->options.camera);
	free(cfg->options.origin);
	free(cfg->options.audio);
	free(cfg->lara.name);

	for (int i = 0; i < 9; i++)
	{
		free(cfg->actor.name[i]);
		free(cfg->actor.slot[i]);
	}
}

long GetConfiguration(const char* filename, SETUP_STRUCT* cfg)
{
	FILE* fp;
	long r, line;

	r = 0;

	InitialiseConfiguration(cfg);

	if (!fopen_s(&fp, filename, "r"))
	{
		line = ini_parse_stream((ini_reader)ReadLine, fp, (ini_handler)ConfigurationHandler, cfg);

		if (!line)
		{
			if (CheckConfiguration(cfg))
				r = 1;
		}
		else
			ShowError("Line %d cannot be parsed", line);

		fclose(fp);
	}
	else
		ShowError("cutseq script cannot be opened");

	return r;
}
