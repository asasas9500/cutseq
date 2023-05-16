#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"
#include "file.h"

void ShowError(const char* format, ...)
{
	va_list list;
	char message[128];

	va_start(list, format);
	vsprintf_s(message, 128, format, list);
	MessageBox(NULL, message, "cutseq", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	va_end(list);
}

int main(int argc, char** argv)
{
	SETUP_STRUCT cfg;
	OPENFILENAME dialog;
	FRAME_DATA* player;
	char* filename;
	long frames, r, base;
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

	if ((argc == 2 || GetOpenFileName(&dialog)) && GetConfiguration(filename, &cfg))
	{
		base = cfg.lara.idx + cfg.actor.idx + 4;
		player = (FRAME_DATA*)malloc(base * sizeof(FRAME_DATA));

		if (player)
		{
			for (int i = 0; i < base; i++)
			{
				player[i].header = NULL;
				player[i].len = 0;
				player[i].seq = NULL;
				player[i].end = 0;
			}

			if (ConvertScene(filename, &cfg, base, player, &frames) && RecordCutscene(&cfg, player, base, frames))
				r = 1;

			for (int i = 0; i < base; i++)
			{
				free(player[i].header);
				free(player[i].seq);
			}

			free(player);
		}
	}

	if (r)
		MessageBox(NULL, "cutseq.pak was saved", "cutseq", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);

	return !r;
}
