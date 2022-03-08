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

void Convert(SETUP_STRUCT* cfg)
{
	FbxManager* manager;
	FbxScene* scene;
	FbxNode* root;
	FbxCamera* cam;
	FbxMesh* actor[10];

	manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	scene = ImportScene(manager, cfg->options_input);

	if (scene)
	{
		root = scene->GetRootNode();
		cam = FbxCast<FbxCamera>(FindAttribute(root, cfg->options_camera, FbxNodeAttribute::eCamera));

		if (cam)
		{
			actor[0] = cfg->lara_idx >= 0 ? FbxCast<FbxMesh>(FindAttribute(root, cfg->lara_name, FbxNodeAttribute::eMesh)) : NULL;

			for (int i = 0; i < 9; i++)
				actor[i + 1] = i <= cfg->actor_idx ? FbxCast<FbxMesh>(FindAttribute(root, cfg->actor_name[i], FbxNodeAttribute::eMesh)) : NULL;


		}
	}

	manager->Destroy();
}
