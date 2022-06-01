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

		free(cfg.options.id);
		free(cfg.options.camera);
		free(cfg.options.input);
		free(cfg.options.output);
		free(cfg.options.origin);
		free(cfg.options.audio);
		free(cfg.lara.name);

		for (int i = 0; i < 9; i++)
		{
			free(cfg.actor.name[i]);
			free(cfg.actor.slot[i]);
		}

		for (int i = 0; i < 12; i++)
			free(player[i].header);
	}

	return !r;
}
