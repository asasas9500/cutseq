#pragma once
#include "types.h"
#include "setup.h"

FbxScene* ImportScene(FbxManager* manager, const char* filename);
FbxNodeAttribute* FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type);
FbxArray<float> EvaluatePropertyByChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* channel);
int FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxMesh** actor);
void Convert(SETUP_STRUCT* cfg);
