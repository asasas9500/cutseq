#include "stdafx.h"
#include "file.h"

void RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player)
{
	HANDLE fp;
	NEW_CUTSCENE cut;
	uchar* buf;
	ulong size;

	fp = FileReader(cfg->options.output);

	if (fp == INVALID_HANDLE_VALUE)
	{
		size = FileSize(fp);

		if (size > 0)
		{
			buf = (uchar*)malloc(size);

			if (buf)
			{

				free(buf);
			}
		}
	}
}
