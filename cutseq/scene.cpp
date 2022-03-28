#include "stdafx.h"
#include "scene.h"

FbxScene* ImportScene(FbxManager* manager, const char* filename)
{
	FbxImporter* importer;
	FbxScene* scene;

	importer = FbxImporter::Create(manager, "Importer");

	if (importer->Initialize(filename, -1, manager->GetIOSettings()) && importer->IsFBX())
	{
		scene = FbxScene::Create(manager, "Scene");

		if (importer->Import(scene))
			return scene;
	}

	return NULL;
}

FbxNodeAttribute* FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type)
{
	FbxNode* node;
	FbxNodeAttribute* attr;

	node = root->FindChild(name);

	if (node)
	{
		attr = node->GetNodeAttribute();

		if (attr && attr->GetAttributeType() == type)
			return attr;
	}

	return NULL;
}

void EvaluatePropertyByChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, FbxArray<float>* channel)
{
	FbxAnimCurve* curve;
	long count;

	curve = prop->GetCurve(layer, name);

	if (curve)
	{
		count = curve->KeyGetCount();

		for (int i = 0; i < count; i++)
			channel->Add(curve->KeyGetValue(i));
	}
}

int FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxMesh** actor)
{
	for (int i = 0; i < 10; i++)
		actor[i] = NULL;

	if (cfg->lara_idx >= 0)
	{
		actor[0] = FbxCast<FbxMesh>(FindAttribute(root, cfg->lara_name, FbxNodeAttribute::eMesh));

		if (!actor[0])
			return 0;
	}

	for (int i = 0; i <= cfg->actor_idx; i++)
	{
		actor[i + 1] = FbxCast<FbxMesh>(FindAttribute(root, cfg->actor_name[i], FbxNodeAttribute::eMesh));

		if (!actor[i + 1])
			return 0;
	}

	return 1;
}

void Convert(SETUP_STRUCT* cfg)
{
	FbxManager* manager;
	FbxScene* scene;
	FbxAnimStack* stack;
	FbxAnimLayer* layer;
	FbxNode* root;
	FbxCamera* cam;
	FbxMesh* actor[10];

	manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	scene = ImportScene(manager, cfg->options_input);

	if (scene)
	{
		stack = scene->GetSrcObject<FbxAnimStack>();

		if (stack)
		{
			layer = stack->GetSrcObject<FbxAnimLayer>();

			if (layer)
			{
				root = scene->GetRootNode();
				cam = FbxCast<FbxCamera>(FindAttribute(root, cfg->options_camera, FbxNodeAttribute::eCamera));

				if (cam && FillActorArray(cfg, root, actor))
				{


				}
			}
		}
	}

	manager->Destroy();
}
