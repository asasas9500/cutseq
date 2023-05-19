#pragma once
#include "types.h"

void ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size);
void WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size);
void ReadULong(HANDLE fp, ulong* value);
void WriteULong(HANDLE fp, ulong value);
long LoadCutsceneList(const char* filename, uchar** buf, ulong* size);
long CheckSignature(uchar* buf);
void PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, ulong cap, NEW_CUTSCENE* cut, ulong* space);
void UpdateCutscene(NEW_CUTSCENE* cut, ulong cap, FRAME_DATA* player, long base, uchar* buf);
long DumpCutsceneList(const char* filename, uchar* buf, ulong size);
void AdjustTable(long id, ulong space, ulong* table);
long RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long base, long frames);
