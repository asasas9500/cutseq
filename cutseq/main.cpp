#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

int main()
{
	SETUP_STRUCT cfg;
	FRAME_DATA player[11];

	if (GetConfiguration(&cfg))
	{
		for (int i = 0; i < 11; i++)
			player[i].header = NULL;

		if (ConvertScene(&cfg, player))
			RecordCutscene(&cfg, player);

		for (int i = 0; i < 11; i++)
		{
			if (player[i].header)
				free(player[i].header);
		}
	}

	return 0;
}
