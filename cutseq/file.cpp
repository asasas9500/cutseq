#include "stdafx.h"
#include "file.h"
#include "zlib/zlib.h"

int ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	if (ReadFile(fp, buf, size, &bytes, NULL) && bytes == size)
		return 1;

	return 0;
}

int WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	if (WriteFile(fp, buf, size, &bytes, NULL) && bytes == size)
		return 1;

	return 0;
}

int ReadULong(HANDLE fp, ulong* value)
{
	ulong bytes;

	if (ReadFile(fp, value, sizeof(ulong), &bytes, NULL) && bytes == sizeof(ulong))
		return 1;

	return 0;
}

int LoadBinaryFile(const char* filename, uchar** buf, ulong* size)
{
	HANDLE fp;
	uchar* src;
	ulong compressed;
	int r;

	r = 0;
	fp = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fp != INVALID_HANDLE_VALUE)
	{
		if (ReadULong(fp, size))
		{
			*buf = (uchar*)malloc(*size);

			if (*buf)
			{
				compressed = GetFileSize(fp, NULL) - 4;
				src = (uchar*)malloc(compressed);

				if (src)
				{
					if (ReadUCharBuffer(fp, src, compressed) && uncompress(*buf, size, src, compressed) == Z_OK)
						r = 1;

					free(src);
				}
			}
		}

		CloseHandle(fp);
	}

	return r;
}

int CheckSignature(uchar* buf)
{
	if (!memcmp(buf, "(C) DEL!", 8))
		return 1;

	return 0;
}

void PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, NEW_CUTSCENE* cut)
{
	long l;

	cut->numframes = (short)frames;
	cut->orgx = cfg->options.origin.x;
	cut->orgy = cfg->options.origin.y;
	cut->orgz = cfg->options.origin.z;
	cut->audio_track = cfg->options.audio;
	cut->camera_offset = sizeof(NEW_CUTSCENE);

	if (cfg->lara.idx != -1)
	{
		cut->actor_data[0].offset = cut->camera_offset + player[0].len * sizeof(NODELOADHEADER) + player[0].seq.Size();
		cut->actor_data[0].objslot = 0;
		cut->actor_data[0].nodes = (short)(player[1].len - 1);
		cut->numactors = 1;
	}
	else
	{
		cut->actor_data[0].offset = cut->camera_offset;
		cut->actor_data[0].objslot = -1;
		cut->actor_data[0].nodes = 0;
		cut->numactors = 0;
	}

	l = cut->numactors;

	for (int i = 1; i < 10; i++)
	{
		if (player[i + l].len)
		{
			cut->actor_data[i].offset = cut->actor_data[i - 1].offset + player[i + l - 1].len * sizeof(NODELOADHEADER) + player[i + l - 1].seq.Size();
			cut->actor_data[i].objslot = (short)cfg->actor.slot[i - 1];
			cut->actor_data[i].nodes = (short)player[i + l].len - 1;
			cut->numactors++;
		}
		else
		{
			cut->actor_data[i].offset = cut->actor_data[i - 1].offset;
			cut->actor_data[i].objslot = -1;
			cut->actor_data[i].nodes = 0;
		}
	}
}

void RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames)
{
	NEW_CUTSCENE cut;
	ulong* table;
	uchar* buf;
	ulong size;

	PrepareCutscene(cfg, player, frames, &cut);
	buf = NULL;

	if (LoadBinaryFile(cfg->options.output, &buf, &size) && CheckSignature(buf))
	{
		table = (ulong*)&buf[8];
	}

	if (buf)
		free(buf);
}
