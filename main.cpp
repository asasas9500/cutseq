#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

int main(int argc, char** argv)
{
	SETUP_STRUCT cfg;
	FRAME_DATA player[12];
	long frames, r;

	r = 1;

	if (argc == 2)
	{
		r = GetConfiguration(argv[1], &cfg);

		if (!r)
		{
			for (int i = 0; i < 12; i++)
			{
				player[i].header = NULL;
				player[i].len = 0;
			}

			r = ConvertScene(&cfg, player, &frames);

			if (!r)
				r = RecordCutscene(&cfg, player, frames);

			for (int i = 0; i < 12; i++)
				free(player[i].header);
		}

		FreeConfiguration(&cfg);
	}

	return r;
}
