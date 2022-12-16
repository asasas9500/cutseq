#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

int main(int argc, char** argv)
{
	SETUP_STRUCT cfg;
	OPENFILENAME dialog;
	FRAME_DATA player[12];
	char* filename;
	long frames, r;
	char script[MAX_PATH];

	r = 0;

	if (argc == 2)
		filename = argv[1];
	else
	{
		script[0] = '\0';
		dialog.lStructSize = sizeof(OPENFILENAME);
		dialog.hwndOwner = NULL;
		dialog.hInstance = NULL;
		dialog.lpstrFilter = "cutseq Scripts (*.txt)\0*.txt\0";
		dialog.lpstrCustomFilter = NULL;
		dialog.nMaxCustFilter = 0;
		dialog.nFilterIndex = 1;
		dialog.lpstrFile = script;
		dialog.nMaxFile = MAX_PATH;
		dialog.lpstrFileTitle = NULL;
		dialog.nMaxFileTitle = 0;
		dialog.lpstrInitialDir = NULL;
		dialog.lpstrTitle = "Load cutseq Script";
		dialog.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER;
		dialog.nFileOffset = 0;
		dialog.nFileExtension = 0;
		dialog.lpstrDefExt = NULL;
		dialog.lCustData = NULL;
		dialog.lpfnHook = NULL;
		dialog.lpTemplateName = NULL;
		dialog.pvReserved = NULL;
		dialog.dwReserved = 0;
		dialog.FlagsEx = 0;
		filename = script;
	}

	if (argc == 2 || GetOpenFileName(&dialog))
	{
		if (GetConfiguration(filename, &cfg))
		{
			for (int i = 0; i < 12; i++)
			{
				player[i].header = NULL;
				player[i].len = 0;
			}

			if (ConvertScene(filename, &cfg, player, &frames) && RecordCutscene(&cfg, player, frames))
				r = 1;

			for (int i = 0; i < 12; i++)
				free(player[i].header);
		}

		FreeConfiguration(&cfg);
	}

	return !r;
}
