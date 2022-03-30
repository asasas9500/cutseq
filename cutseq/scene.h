#pragma once
#include "types.h"
#include "setup.h"

struct FRAME_DATA
{
	NODELOADHEADER* header;
	long len;
	FbxArray<uchar> seq;
};

FbxScene* ImportScene(FbxManager* manager, const char* filename);
FbxNodeAttribute* FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type);
int EvaluatePropertyByChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, FbxArray<float>* channel);
int FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxMesh** actor);
int CompressChannel(FbxArray<float>* channel, FbxArray<uchar>* seq, short* number);
int AppendValue(ushort value, long shift, FbxArray<uchar>* seq);
int ProcessProperty(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float m, FbxArray<uchar>* seq, short* key, short* number);
int TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player);
int PackActor(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player);
void TransformChannel(float m, FbxArray<float>* channel);
int PackCamera(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player);
void Convert(SETUP_STRUCT* cfg);
