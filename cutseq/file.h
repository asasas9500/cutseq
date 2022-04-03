#pragma once
#include "types.h"
#include "setup.h"
#include "scene.h"

int ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size);
int WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size);
int ReadULong(HANDLE fp, ulong* value);
int LoadCutsceneList(const char* filename, uchar** buf, ulong* size);
int CheckSignature(uchar* buf);
ulong PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, NEW_CUTSCENE* cut);
void RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames);
