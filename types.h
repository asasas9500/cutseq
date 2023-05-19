#pragma once

#pragma pack(push, 1)

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

struct PHD_VECTOR
{
	long x;
	long y;
	long z;
};

struct ACTORME
{
	long offset;
	short objslot;
	short nodes;
};

struct NEW_CUTSCENE
{
	short numactors;
	short numframes;
	long orgx;
	long orgy;
	long orgz;
	long audio_track;
	long camera_offset;
	ACTORME actor_data[10];
};

struct NODELOADHEADER
{
	short xkey;
	short ykey;
	short zkey;
	short packmethod;
	short xlength;
	short ylength;
	short zlength;
};

struct OPTIONAL_INTEGER
{
	long cnt;
	char on;
};

struct OPTIONAL_STRING
{
	char cnt[200];
	char on;
};

struct OPTIONAL_VECTOR
{
	PHD_VECTOR cnt;
	char on;
};

struct OPTIONS_SET
{
	OPTIONAL_INTEGER number;
	OPTIONAL_STRING camera;
	OPTIONAL_VECTOR origin;
	OPTIONAL_INTEGER audio;
};

struct LARA_SET
{
	OPTIONAL_STRING name;
};

struct ACTOR_SET
{
	OPTIONAL_STRING name;
	OPTIONAL_INTEGER slot;
};

struct OPTIONS_COLLECTION
{
	OPTIONS_SET set;
	char idx;
};

struct LARA_COLLECTION
{
	LARA_SET set;
	char idx;
};

struct ACTOR_COLLECTION
{
	ACTOR_SET set[99];
	char idx;
};

struct SETUP_STRUCT
{
	OPTIONS_COLLECTION options;
	LARA_COLLECTION lara;
	ACTOR_COLLECTION actor;
};

struct FRAME_DATA
{
	NODELOADHEADER* header;
	long len;
	uchar* seq;
	long end;
};

#pragma pack(pop)
