#pragma once
#include "types.h"
#include "setup.h"

FbxScene* ImportScene(FbxManager* manager, const char* filename);
FbxNodeAttribute* FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type);
void Convert(SETUP_STRUCT* cfg);
