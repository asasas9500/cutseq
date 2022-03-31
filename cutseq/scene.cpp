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

int EvaluatePropertyByChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, FbxArray<float>* channel)
{
	FbxAnimCurve* curve;
	long count;

	curve = prop->GetCurve(layer, name);

	if (!curve)
		return 0;

	count = curve->KeyGetCount();

	if (count < 2)
		return 0;

	for (int i = 0; i < count; i++)
	{
		if (channel->Add(curve->KeyGetValue(i)) == -1)
			return 0;
	}

	return 1;
}

int FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxMesh** actor)
{
	long curr;

	curr = 0;

	if (!cfg->lara.idx)
	{
		actor[curr] = FbxCast<FbxMesh>(FindAttribute(root, cfg->lara.name, FbxNodeAttribute::eMesh));

		if (!actor[curr])
			return 0;

		curr++;
	}

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		actor[curr] = FbxCast<FbxMesh>(FindAttribute(root, cfg->actor.name[i], FbxNodeAttribute::eMesh));

		if (!actor[curr])
			return 0;

		curr++;
	}

	for (int i = curr; i < 10; i++)
		actor[i] = NULL;

	return 1;
}

int CompressChannel(FbxArray<float>* channel, FbxArray<uchar>* seq, short* number)
{
	short diff, count;

	*number = 0;

	for (int i = 1; i < channel->Size(); i++)
	{
		if (!((i - 1) % 16))
		{
			count = channel->Size() - i;

			if (count >= 16)
				count = 0;

			if (!AppendValue(32 | count, *number % 8, seq))
				return 0;

			(*number)++;
		}

		diff = (short)(channel->GetAt(i) - channel->GetAt(i - 1));

		if (diff > 16383)
			diff = 16383;
		else if (diff < -16384)
			diff = -16384;

		if (!AppendValue(diff & 0x3FFF, *number % 8, seq))
			return 0;

		(*number)++;
	}

	for (int i = 0; i < 3; i++)
	{
		if (seq->Add(0) == -1)
			return 0;
	}

	if (!(*number % 8) && seq->Add(0) == -1)
		return 0;

	return 1;
}

int AppendValue(ushort value, long shift, FbxArray<uchar>* seq)
{
	if (shift)
		seq->SetLast(seq->GetLast() | value >> (15 - shift));

	if (seq->Add(value >> (7 - shift)) == -1)
		return 0;

	if (shift != 7 && seq->Add(value << (shift + 1)) == -1)
		return 0;

	return 1;
}

int ProcessProperty(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float m, FbxArray<uchar>* seq, short* key, short* number)
{
	FbxArray<float> channel;

	if (!EvaluatePropertyByChannel(layer, prop, name, &channel))
		return 0;

	TransformChannel(m, &channel);

	if (!CompressChannel(&channel, seq, number))
		return 0;

	*key = (short)channel.GetAt(0);

	return 1;
}

int TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	FbxNode* child;

	player->len++;
	header = (NODELOADHEADER*)realloc(player->header, player->len * sizeof(NODELOADHEADER));

	if (!header)
		return 0;

	player->header = header;
	header = &player->header[player->len - 1];
	header->packmethod = 0x3DEF;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, 2.8444444444F, &player->seq, &header->xkey, &header->xlength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, -2.8444444444F, &player->seq, &header->ykey, &header->ylength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, -2.8444444444F, &player->seq, &header->zkey, &header->zlength))
		return 0;

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		child = node->GetChild(i);
		if (!TraverseActorHierarchy(layer, child, player))
			return 0;
	}

	return 1;
}

int PackActor(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player)
{
	NODELOADHEADER* header;

	player->len = 1;
	player->header = (NODELOADHEADER*)malloc(sizeof(NODELOADHEADER));

	if (!player->header)
		return 0;

	header = player->header;
	header->packmethod = 0x3DEF;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, 0.3333333333F, &player->seq, &header->xkey, &header->xlength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, -0.3333333333F, &player->seq, &header->ykey, &header->ylength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, -0.3333333333F, &player->seq, &header->zkey, &header->zlength))
		return 0;

	return TraverseActorHierarchy(layer, node, player);
}

void TransformChannel(float m, FbxArray<float>* channel)
{
	for (int i = 0; i < channel->Size(); i++)
		channel->SetAt(i, m * channel->GetAt(i));
}

int PackCamera(FbxAnimLayer* layer, FbxNode* node, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	FbxNode* target;

	player->len = 2;
	player->header = (NODELOADHEADER*)malloc(2 * sizeof(NODELOADHEADER));

	if (!player->header)
		return 0;

	header = player->header;

	for (int i = 0; i < 2; i++)
	{
		header->packmethod = 0x3DEF;

		if (!i)
			target = node->GetTarget();
		else
			target = node;

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, 0.5F, &player->seq, &header->xkey, &header->xlength))
			return 0;

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, -0.5F, &player->seq, &header->ykey, &header->ylength))
			return 0;

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, -0.5F, &player->seq, &header->zkey, &header->zlength))
			return 0;

		header++;
	}

	return 1;
}

int PackScene(FbxAnimLayer* layer, FbxCamera* cam, FbxMesh** actor, FRAME_DATA* player)
{
	if (!PackCamera(layer, cam->GetNode(), player))
		return 0;

	for (int i = 0; i < 10; i++)
	{
		if (!actor[i])
			break;

		if (!PackActor(layer, actor[i]->GetNode(), &player[i + 1]))
			return 0;
	}

	return 1;
}

int ConvertScene(SETUP_STRUCT* cfg, FRAME_DATA* player)
{
	FbxManager* manager;
	FbxScene* scene;
	FbxAnimStack* stack;
	FbxAnimLayer* layer;
	FbxNode* root;
	FbxCamera* cam;
	FbxMesh* actor[10];
	int r;

	r = 0;
	manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	scene = ImportScene(manager, cfg->options.input);

	if (scene)
	{
		stack = scene->GetSrcObject<FbxAnimStack>();

		if (stack)
		{
			layer = stack->GetSrcObject<FbxAnimLayer>();

			if (layer)
			{
				root = scene->GetRootNode();
				cam = FbxCast<FbxCamera>(FindAttribute(root, cfg->options.camera, FbxNodeAttribute::eCamera));

				if (cam && cam->GetNode()->GetTarget() && FillActorArray(cfg, root, actor) && PackScene(layer, cam, actor, player))
					r = 1;
			}
		}
	}

	manager->Destroy();
	return r;
}
