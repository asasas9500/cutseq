#include "stdafx.h"
#include "file.h"

HANDLE FileReader(const char* filename)
{
	return CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE FileWriter(const char* filename)
{
	return CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void FileSeek(HANDLE fp, long offset)
{
	SetFilePointer(fp, offset, NULL, FILE_BEGIN);
}

void FileClose(HANDLE fp)
{
	CloseHandle(fp);
}

char ReadChar(HANDLE fp)
{
	DWORD bytes;
	char value;

	ReadFile(fp, &value, sizeof(char), &bytes, NULL);
	return value;
}

uchar ReadUChar(HANDLE fp)
{
	DWORD bytes;
	uchar value;

	ReadFile(fp, &value, sizeof(uchar), &bytes, NULL);
	return value;
}

short ReadShort(HANDLE fp)
{
	DWORD bytes;
	short value;

	ReadFile(fp, &value, sizeof(short), &bytes, NULL);
	return value;
}

ushort ReadUShort(HANDLE fp)
{
	DWORD bytes;
	ushort value;

	ReadFile(fp, &value, sizeof(ushort), &bytes, NULL);
	return value;
}

long ReadLong(HANDLE fp)
{
	DWORD bytes;
	long value;

	ReadFile(fp, &value, sizeof(long), &bytes, NULL);
	return value;
}

ulong ReadULong(HANDLE fp)
{
	DWORD bytes;
	ulong value;

	ReadFile(fp, &value, sizeof(ulong), &bytes, NULL);
	return value;
}

void WriteChar(HANDLE fp, char value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(char), &bytes, NULL);
}

void WriteUChar(HANDLE fp, uchar value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(uchar), &bytes, NULL);
}

void WriteShort(HANDLE fp, short value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(short), &bytes, NULL);
}

void WriteUShort(HANDLE fp, ushort value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(ushort), &bytes, NULL);
}

void WriteLong(HANDLE fp, long value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(long), &bytes, NULL);
}

void WriteULong(HANDLE fp, ulong value)
{
	DWORD bytes;

	WriteFile(fp, &value, sizeof(ulong), &bytes, NULL);
}
