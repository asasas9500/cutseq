#pragma once

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
	long properties;
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

struct OPTIONS_SET
{
	long id;
	char camera[200];
	char input[200];
	char output[200];
	PHD_VECTOR origin;
	long audio;
	long idx;
};

struct LARA_SET
{
	char name[200];
	long idx;
};

struct ACTOR_SET
{
	char name[9][200];
	long slot[9];
	long idx;
};

struct SETUP_STRUCT
{
	OPTIONS_SET options;
	LARA_SET lara;
	ACTOR_SET actor;
};

struct FRAME_DATA
{
	NODELOADHEADER* header;
	long len;
	FbxArray<uchar> seq;
};
