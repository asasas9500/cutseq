#pragma once
#include "types.h"

void ReadUCharBuffer(HANDLE fp, uchar* buf, ulong size);
void WriteUCharBuffer(HANDLE fp, uchar* buf, ulong size);
void ReadULong(HANDLE fp, ulong* value);
void WriteULong(HANDLE fp, ulong value);
long LoadCutsceneList(const char* filename, uchar** buf, ulong* size);
long CheckSignature(uchar* buf);
void PrepareCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames, CUTSCENE_DESCRIPTOR* cd, ulong* space);
void UpdateCutscene(CUTSCENE_DESCRIPTOR* cd, FRAME_DATA* player, uchar* buf, ulong off);
long DumpCutsceneList(const char* filename, uchar* buf, ulong size);
void AdjustTable(long id, ulong space, ulong* table);
long RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player, long frames);
