#pragma once
#include "types.h"
#include "setup.h"
#include "scene.h"

HANDLE FileReader(const char* filename);
HANDLE FileWriter(const char* filename);
void FileSeek(HANDLE fp, long offset);
void FileClose(HANDLE fp);
char ReadChar(HANDLE fp);
uchar ReadUChar(HANDLE fp);
short ReadShort(HANDLE fp);
ushort ReadUShort(HANDLE fp);
long ReadLong(HANDLE fp);
ulong ReadULong(HANDLE fp);
void WriteChar(HANDLE fp, char value);
void WriteUChar(HANDLE fp, uchar value);
void WriteShort(HANDLE fp, short value);
void WriteUShort(HANDLE fp, ushort value);
void WriteLong(HANDLE fp, long value);
void WriteULong(HANDLE fp, ulong value);
ulong FileSize(HANDLE fp);
void RecordCutscene(SETUP_STRUCT* cfg, FRAME_DATA* player);
