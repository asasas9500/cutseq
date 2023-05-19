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

long ConfigurationHandler(void* data, const char* section, const char* entry, const char* value)
{
	SETUP_STRUCT* cfg;

	cfg = (SETUP_STRUCT*)data;

	if (!_stricmp(section, "options"))
	{
		if (cfg->options.idx < 1)
		{
			if (entry == NULL && value == NULL)
				cfg->options.idx++;
			else if (!_stricmp(entry, "number"))
			{
				if (!cfg->options.set.number.on)
				{
					if (!ParseIntegers(value, &cfg->options.set.number.cnt, 1))
					{
						ShowError("Cutscene number must be an integer");
						return 0;
					}

					if (cfg->options.set.number.cnt < 1 || cfg->options.set.number.cnt > 255)
					{
						ShowError("Cutscene number must be between 1 and 255");
						return 0;
					}

					cfg->options.set.number.on = 1;
				}
			}
			else if (!_stricmp(entry, "camera"))
			{
				if (!cfg->options.set.camera.on)
				{
					strcpy_s(cfg->options.set.camera.cnt, 200, value);

					if (cfg->options.set.camera.cnt[0] == '\0')
					{
						ShowError("Camera node name must be non-empty");
						return 0;
					}

					cfg->options.set.camera.on = 1;
				}
			}
			else if (!_stricmp(entry, "origin"))
			{
				if (!cfg->options.set.origin.on)
				{
					if (!ParseIntegers(value, (long*)&cfg->options.set.origin.cnt, 3))
					{
						ShowError("Origin must be composed of three integers separated by commas");
						return 0;
					}

					if (cfg->options.set.origin.cnt.x < 0 || cfg->options.set.origin.cnt.x > 131071)
					{
						ShowError("X coordinate of origin must be between 0 and 131071");
						return 0;
					}

					if (cfg->options.set.origin.cnt.y < -32768 || cfg->options.set.origin.cnt.y > 32767)
					{
						ShowError("Y coordinate of origin must be between -32768 and 32767");
						return 0;
					}

					if (cfg->options.set.origin.cnt.z < 0 || cfg->options.set.origin.cnt.z > 131071)
					{
						ShowError("Z coordinate of origin must be between 0 and 131071");
						return 0;
					}

					cfg->options.set.origin.on = 1;
				}
			}
			else if (!_stricmp(entry, "audio"))
			{
				if (!cfg->options.set.audio.on)
				{
					if (!ParseIntegers(value, &cfg->options.set.audio.cnt, 1))
					{
						ShowError("Audio must be an integer");
						return 0;
					}

					if (cfg->options.set.audio.cnt < 0 || cfg->options.set.audio.cnt > 255)
					{
						ShowError("Audio must be between 0 and 255");
						return 0;
					}

					cfg->options.set.audio.on = 1;
				}
			}
		}
	}
	else if (!_stricmp(section, "lara"))
	{
		if (cfg->lara.idx < 1)
		{
			if (entry == NULL && value == NULL)
				cfg->lara.idx++;
			else if (!_stricmp(entry, "name"))
			{
				if (!cfg->lara.set.name.on)
				{
					strcpy_s(cfg->lara.set.name.cnt, 200, value);

					if (cfg->lara.set.name.cnt[0] == '\0')
					{
						ShowError("Lara node name must be non-empty");
						return 0;
					}

					cfg->lara.set.name.on = 1;
				}
			}
		}
	}
	else if (!_stricmp(section, "actor"))
	{
		if (cfg->actor.idx < 99)
		{
			if (entry == NULL && value == NULL)
				cfg->actor.idx++;
			else if (!_stricmp(entry, "name"))
			{
				if (!cfg->actor.set[cfg->actor.idx].name.on)
				{
					strcpy_s(cfg->actor.set[cfg->actor.idx].name.cnt, 200, value);

					if (cfg->actor.set[cfg->actor.idx].name.cnt[0] == '\0')
					{
						ShowError("Actor %d node name must be non-empty", cfg->actor.idx + 1);
						return 0;
					}

					cfg->actor.set[cfg->actor.idx].name.on = 1;
				}
			}
			else if (!_stricmp(entry, "slot"))
			{
				if (!cfg->actor.set[cfg->actor.idx].slot.on)
				{
					if (!ParseIntegers(value, &cfg->actor.set[cfg->actor.idx].slot.cnt, 1))
					{
						ShowError("Actor %d slot must be an integer", cfg->actor.idx + 1);
						return 0;
					}

					if (cfg->actor.set[cfg->actor.idx].slot.cnt < 1 || cfg->actor.set[cfg->actor.idx].slot.cnt > 519)
					{
						ShowError("Actor %d slot must be between 1 and 519", cfg->actor.idx + 1);
						return 0;
					}

					cfg->actor.set[cfg->actor.idx].slot.on = 1;
				}
			}
		}
	}

	return 1;
}

void InitialiseConfiguration(SETUP_STRUCT* cfg)
{
	cfg->options.set.number.on = 0;
	cfg->options.set.camera.on = 0;
	cfg->options.set.origin.on = 0;
	cfg->options.set.audio.on = 0;
	cfg->options.idx = -1;
	cfg->lara.set.name.on = 0;
	cfg->lara.idx = -1;

	for (int i = 0; i < 99; i++)
	{
		cfg->actor.set[i].name.on = 0;
		cfg->actor.set[i].slot.on = 0;
	}

	cfg->actor.idx = -1;
}

long CheckConfiguration(SETUP_STRUCT* cfg)
{
	if (cfg->options.idx > 0)
		cfg->options.idx = 0;

	if (cfg->lara.idx > 0)
		cfg->lara.idx = 0;

	if (cfg->actor.idx > 98)
		cfg->actor.idx = 98;

	if (cfg->options.idx == -1)
	{
		ShowError("Options section must be present");
		return 0;
	}

	if (!cfg->options.set.number.on)
	{
		ShowError("Cutscene number must be informed");
		return 0;
	}

	if (!cfg->options.set.camera.on)
	{
		ShowError("Camera node name must be informed");
		return 0;
	}

	if (cfg->lara.idx != -1 && !cfg->lara.set.name.on)
	{
		ShowError("Lara node name must be informed");
		return 0;
	}

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		if (!cfg->actor.set[i].name.on)
		{
			ShowError("Actor %d node name must be informed", i + 1);
			return 0;
		}

		if (!cfg->actor.set[i].slot.on)
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
