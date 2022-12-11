#pragma once
#include "types.h"

long ImportScene(FbxManager* manager, const char* filename, FbxNode** root, long* frames, FbxAnimLayer** layer);
long FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type, FbxNode** node);
long EvaluateChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, long frames, float** channel);
long FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxNode** actor);
long CompressChannel(float* channel, long frames, FbxArray<uchar>* seq, short* number);
long AppendValue(ushort value, long shift, FbxArray<uchar>* seq);
long ProcessChannel(float m, long mask, long frames, float* channel, FbxArray<uchar>* seq, short* key, short* number);
long EvaluateDummyChannel(float a, long frames, float** channel);
long ProvideChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float a, long frames, float** channel);
long PackActorRotation(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long PackActor(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
void TransformChannel(float m, long frames, float* channel);
long PackCamera(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long PackExtensions(FbxAnimLayer* layer, FbxCamera* cam, long frames, FRAME_DATA* player);
long PackScene(FbxAnimLayer* layer, FbxNode* cam, FbxNode** actor, long frames, FRAME_DATA* player);
long ConvertScene(SETUP_STRUCT* cfg, FRAME_DATA* player, long* frames);
