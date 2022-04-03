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

int LoadCutsceneList(const char* filename, uchar** buf, ulong* size)
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

ulong PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, NEW_CUTSCENE* cut)
{
	long curr;
	ulong space;

	cut->numactors = (short)(cfg->actor.idx + 2);
	cut->numframes = (short)frames;
	cut->orgx = cfg->options.origin.x;
	cut->orgy = cfg->options.origin.y;
	cut->orgz = cfg->options.origin.z;
	cut->audio_track = cfg->options.audio;
	cut->camera_offset = sizeof(NEW_CUTSCENE);
	curr = 0;
	space = cut->camera_offset + player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();

	if (cfg->lara.idx != -1)
	{
		curr++;
		cut->actor_data[0].offset = space;
		cut->actor_data[0].objslot = 0;
		cut->actor_data[0].nodes = (short)(player[curr].len - 1);
		space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();
	}
	else
	{
		cut->actor_data[0].offset = -1;
		cut->actor_data[0].objslot = -1;
		cut->actor_data[0].nodes = -1;
	}

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		curr++;
		cut->actor_data[i + 1].offset = space;
		cut->actor_data[i + 1].objslot = (short)cfg->actor.slot[i];
		cut->actor_data[i + 1].nodes = (short)(player[curr].len - 1);
		space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();
	}

	for (int i = cfg->actor.idx + 1; i < 9; i++)
	{
		cut->actor_data[i + 1].offset = -1;
		cut->actor_data[i + 1].objslot = -1;
		cut->actor_data[i + 1].nodes = -1;
	}

	return space;
}

void RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames)
{
	NEW_CUTSCENE cut;
	ulong* table;
	uchar* buf;
	ulong size, space;

	space = PrepareCutscene(cfg, player, frames, &cut);
	buf = NULL;

	if (LoadCutsceneList(cfg->options.output, &buf, &size) && CheckSignature(buf))
	{
		table = (ulong*)&buf[8];
	}

	if (buf)
		free(buf);
}
