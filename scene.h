#pragma once
#include "types.h"

long ImportScene(FbxManager* manager, const char* filename, FbxNode** root, long* frames, FbxAnimLayer** layer);
long FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type, FbxNode** node);
long EvaluateChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float a, long frames, float** channel);
long FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxNode** actor);
long CompressChannel(float* channel, long frames, uchar** seq, long* end, short* number);
long AppendValue(ushort value, long shift, uchar** seq, long* end);
long PushElement(uchar elem, uchar** seq, long* end);
long ProcessChannel(float m, long mask, long frames, float* channel, uchar** seq, long* end, short* key, short* number);
long ProvideFixedChannel(float a, long frames, float** channel);
long PackActorRotation(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long PackActor(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
void TransformChannel(float m, long frames, float* channel);
long PackCamera(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player);
long PackExtensions(FbxAnimLayer* layer, FbxCamera* cam, long frames, FRAME_DATA* player);
long PackScene(FbxAnimLayer* layer, FbxNode* cam, FbxNode** actor, long frames, long base, FRAME_DATA* player);
long ConvertScene(const char* filename, SETUP_STRUCT* cfg, long base, FRAME_DATA* player, long* frames);
