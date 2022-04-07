#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

int main(int argc, char** argv)
{
	SETUP_STRUCT cfg;
	FRAME_DATA player[11];
	long frames;

	if (argc == 2 && GetConfiguration(argv[1], &cfg))
	{
		for (int i = 0; i < 11; i++)
		{
			player[i].header = NULL;
			player[i].len = 0;
		}

		if (ConvertScene(&cfg, player, &frames))
			RecordCutscene(&cfg, player, frames);

		for (int i = 0; i < 11; i++)
		{
			if (player[i].header)
				free(player[i].header);
		}
	}

	return 0;
}
