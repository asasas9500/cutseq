#include "stdafx.h"
#include "file.h"
#include "main.h"
#include "zlib/zlib.h"

void ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	ReadFile(fp, buf, size, &bytes, NULL);
}

void WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	WriteFile(fp, buf, size, &bytes, NULL);
}

void ReadULong(HANDLE fp, ulong* value)
{
	ulong bytes;

	ReadFile(fp, value, sizeof(ulong), &bytes, NULL);
}

void WriteULong(HANDLE fp, ulong value)
{
	ulong bytes;

	WriteFile(fp, &value, sizeof(ulong), &bytes, NULL);
}

long LoadCutsceneList(const char* filename, uchar** buf, ulong* size)
{
	HANDLE fp;
	uchar* src;
	ulong compressed;
	long r;

	r = 0;
	fp = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fp != INVALID_HANDLE_VALUE)
	{
		ReadULong(fp, size);
		*buf = (uchar*)malloc(*size);

		if (*buf)
		{
			compressed = GetFileSize(fp, NULL) - 4;
			src = (uchar*)malloc(compressed);

			if (src)
			{
				ReadUCharBuffer(fp, src, compressed);

				if (uncompress(*buf, size, src, compressed) == Z_OK)
					r = 1;
				else
					ShowError("cutseq.pak cannot be decompressed");

				free(src);
			}
		}

		CloseHandle(fp);
	}
	else
		ShowError("cutseq.pak cannot be opened");

	return r;
}

long CheckSignature(uchar* buf)
{
	if (!memcmp(buf, "(C) DEL!", 8))
		return 1;

	return 0;
}

void PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, CUTSCENE_DESCRIPTOR* cd, ulong* space)
{
	NEW_CUTSCENE* cut;
	long curr;

	cut = &cd->cut;
	cut->numactors = cfg->actor.idx + 2;
	cut->numframes = (short)frames;

	if (cfg->options.set.origin.on)
	{
		cut->orgx = cfg->options.set.origin.cnt.x;
		cut->orgy = cfg->options.set.origin.cnt.y;
		cut->orgz = cfg->options.set.origin.cnt.z;
	}
	else
	{
		cut->orgx = 0;
		cut->orgy = 0;
		cut->orgz = 0;
	}

	if (cfg->options.set.audio.on)
		cut->audio_track = cfg->options.set.audio.cnt;
	else
		cut->audio_track = -1;

	*space = sizeof(CUTSCENE_DESCRIPTOR);
	curr = 0;
	cut->camera_offset = *space;
	*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].end * sizeof(uchar);

	if (cfg->lara.idx != -1)
	{
		curr++;
		cut->actor_data[0].offset = *space;
		cut->actor_data[0].objslot = 0;
		cut->actor_data[0].nodes = (short)(player[curr].len - 1);
		*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].end * sizeof(uchar);
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
		cut->actor_data[i + 1].offset = *space;
		cut->actor_data[i + 1].objslot = (short)cfg->actor.set[i].slot.cnt;
		cut->actor_data[i + 1].nodes = (short)(player[curr].len - 1);
		*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].end * sizeof(uchar);
	}

	for (int i = cfg->actor.idx + 1; i < 9; i++)
	{
		cut->actor_data[i + 1].offset = -1;
		cut->actor_data[i + 1].objslot = -1;
		cut->actor_data[i + 1].nodes = -1;
	}

	curr++;
	cd->ext = *space;
	*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].end * sizeof(uchar);
}

void UpdateCutscene(CUTSCENE_DESCRIPTOR* cd, FRAME_DATA* player, long base, uchar* buf)
{
	ulong off, space;

	space = sizeof(CUTSCENE_DESCRIPTOR);
	memcpy(buf, cd, space);
	off = space;

	for (int i = 0; i < base; i++)
	{
		space = player[i].len * sizeof(NODELOADHEADER);
		memcpy(&buf[off], player[i].header, space);
		off += space;
		space = player[i].end * sizeof(uchar);
		memcpy(&buf[off], player[i].seq, space);
		off += space;
	}
}

long DumpCutsceneList(const char* filename, uchar* buf, ulong size)
{
	HANDLE fp;
	uchar* dest;
	ulong compressed;
	long r;

	r = 0;
	fp = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fp != INVALID_HANDLE_VALUE)
	{
		WriteULong(fp, size);
		compressed = compressBound(size);
		dest = (uchar*)malloc(compressed);

		if (dest)
		{
			if (compress2(dest, &compressed, buf, size, Z_BEST_COMPRESSION) == Z_OK)
			{
				WriteUCharBuffer(fp, dest, compressed);
				r = 1;
			}
			else
				ShowError("cutseq.pak cannot be compressed");

			free(dest);
		}

		CloseHandle(fp);
	}
	else
		ShowError("cutseq.pak cannot be opened");

	return r;
}

void AdjustTable(long number, ulong space, ulong* table)
{
	table[2 * number + 1] = space;

	for (int i = number + 1; i < 256; i++)
		table[2 * i] = table[2 * i - 2] + table[2 * i - 1];
}

long RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long base, long frames)
{
	CUTSCENE_DESCRIPTOR cd;
	ulong* table;
	uchar* buf;
	uchar* ptr;
	ulong size, space, old, off;
	long number, r;
	char output[MAX_PATH];

	r = 0;
	GetModuleFileName(NULL, output, MAX_PATH);
	PathRemoveFileSpec(output);
	PathAppend(output, "cutseq.pak");
	buf = NULL;

	if (LoadCutsceneList(output, &buf, &size))
	{
		if (CheckSignature(buf))
		{
			table = (ulong*)buf;
			PrepareCutscene(cfg, player, frames, &cd, &space);
			number = cfg->options.set.number.cnt;
			old = table[2 * number + 1];
			size += space - old;

			if (space > old)
				ptr = (uchar*)realloc(buf, size);
			else
				ptr = buf;

			if (ptr)
			{
				buf = ptr;
				table = (ulong*)buf;
				off = table[2 * number] + space;
				memmove(&buf[off], &buf[off + old - space], size - off);
				UpdateCutscene(&cd, player, base, &buf[table[2 * number]]);
				AdjustTable(number, space, table);

				if (DumpCutsceneList(output, buf, size))
					r = 1;
			}
		}
		else
			ShowError("cutseq.pak signature must match");
	}

	free(buf);
	return r;
}
