#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

int main(int argc, char** argv)
{
	SETUP_STRUCT cfg;
	FRAME_DATA player[12];
	long frames;
	int r;

	r = 0;

	if (argc == 2 && GetConfiguration(argv[1], &cfg))
	{
		for (int i = 0; i < 12; i++)
		{
			player[i].header = NULL;
			player[i].len = 0;
		}

		if (ConvertScene(&cfg, player, &frames) && RecordCutscene(&cfg, player, frames))
			r = 1;

		for (int i = 0; i < 12; i++)
			free(player[i].header);
	}

	return !r;
}
