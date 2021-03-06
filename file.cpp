#include "stdafx.h"
#include "file.h"
#include "zlib/zlib.h"

long ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	if (ReadFile(fp, buf, size, &bytes, NULL) && bytes == size)
		return 1;

	return 0;
}

long WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size)
{
	ulong bytes;

	if (WriteFile(fp, buf, size, &bytes, NULL) && bytes == size)
		return 1;

	return 0;
}

long ReadULong(HANDLE fp, ulong* value)
{
	ulong bytes;

	if (ReadFile(fp, value, sizeof(ulong), &bytes, NULL) && bytes == sizeof(ulong))
		return 1;

	return 0;
}

long WriteULong(HANDLE fp, ulong value)
{
	ulong bytes;

	if (WriteFile(fp, &value, sizeof(ulong), &bytes, NULL) && bytes == sizeof(ulong))
		return 1;

	return 0;
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
	cut->numactors = (short)(cfg->actor.idx + 2);
	cut->numframes = (short)frames;
	cut->orgx = cfg->options.origin->x;
	cut->orgy = cfg->options.origin->y;
	cut->orgz = cfg->options.origin->z;
	cut->audio_track = *cfg->options.audio;
	*space = sizeof(CUTSCENE_DESCRIPTOR);
	curr = 0;
	cut->camera_offset = *space;
	*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();

	if (cfg->lara.idx != -1)
	{
		curr++;
		cut->actor_data[0].offset = *space;
		cut->actor_data[0].objslot = 0;
		cut->actor_data[0].nodes = (short)(player[curr].len - 1);
		*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();
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
		cut->actor_data[i + 1].objslot = (short)*cfg->actor.slot[i];
		cut->actor_data[i + 1].nodes = (short)(player[curr].len - 1);
		*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();
	}

	for (int i = cfg->actor.idx + 1; i < 9; i++)
	{
		cut->actor_data[i + 1].offset = -1;
		cut->actor_data[i + 1].objslot = -1;
		cut->actor_data[i + 1].nodes = -1;
	}

	curr++;
	cd->ext = *space;
	*space += player[curr].len * sizeof(NODELOADHEADER) + player[curr].seq.Size();
}

void UpdateCutscene(CUTSCENE_DESCRIPTOR* cd, FRAME_DATA* player, uchar* buf, ulong off)
{
	ulong space;

	space = sizeof(CUTSCENE_DESCRIPTOR);
	memcpy(&buf[off], cd, space);
	off += space;

	for (int i = 0; i < 11; i++)
	{
		if (!player[i].len)
			break;

		space = player[i].len * sizeof(NODELOADHEADER);
		memcpy(&buf[off], player[i].header, space);
		off += space;
		space = player[i].seq.Size() * sizeof(uchar);
		memcpy(&buf[off], player[i].seq.GetArray(), space);
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
		if (WriteULong(fp, size))
		{
			compressed = compressBound(size);
			dest = (uchar*)malloc(compressed);

			if (dest)
			{
				if (compress2(dest, &compressed, buf, size, Z_BEST_COMPRESSION) == Z_OK && WriteUCharBuffer(fp, dest, compressed))
					r = 1;

				free(dest);
			}
		}

		CloseHandle(fp);
	}

	return r;
}

void AdjustTable(long id, ulong space, ulong* table)
{
	table[2 * id + 1] = space;

	for (int i = id + 1; i < 256; i++)
		table[2 * i] = table[2 * i - 2] + table[2 * i - 1];
}

long RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames)
{
	CUTSCENE_DESCRIPTOR cd;
	ulong* table;
	uchar* buf;
	uchar* ptr;
	ulong size, space, old, off;
	long id, r;

	r = 0;
	buf = NULL;

	if (LoadCutsceneList(cfg->options.output, &buf, &size) && CheckSignature(buf))
	{
		table = (ulong*)buf;
		PrepareCutscene(cfg, player, frames, &cd, &space);
		id = *cfg->options.id;
		old = table[2 * id + 1];
		size += space - old;

		if (space > old)
			ptr = (uchar*)realloc(buf, size);
		else
			ptr = buf;

		if (ptr)
		{
			buf = ptr;
			table = (ulong*)buf;
			off = table[2 * id] + space;
			memmove(&buf[off], &buf[off + old - space], size - off);
			UpdateCutscene(&cd, player, buf, table[2 * id]);
			AdjustTable(id, space, table);

			if (DumpCutsceneList(cfg->options.output, buf, size))
				r = 1;
		}
	}

	free(buf);
	return r;
}
