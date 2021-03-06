#include "stdafx.h"
#include "scene.h"

long ImportScene(FbxManager* manager, const char* filename, FbxNode** root, long* frames, FbxAnimLayer** layer)
{
	FbxImporter* importer;
	FbxScene* scene;
	FbxAnimStack* stack;

	importer = FbxImporter::Create(manager, "Importer");

	if (importer->Initialize(filename, -1, manager->GetIOSettings()) && importer->IsFBX())
	{
		scene = FbxScene::Create(manager, "Scene");

		if (importer->Import(scene))
		{
			*root = scene->GetRootNode();
			stack = scene->GetSrcObject<FbxAnimStack>();

			if (stack)
			{
				*frames = (long)stack->GetLocalTimeSpan().GetDuration().GetFrameCount(FbxTime::eFrames30) + 1;

				if (*frames >= 2)
				{
					*layer = stack->GetSrcObject<FbxAnimLayer>();

					if (*layer)
						return 1;
				}
			}
		}
	}

	return 0;
}

long FindAttribute(FbxNode* root, const char* name, FbxNodeAttribute::EType type, FbxNode** node)
{
	FbxNodeAttribute* attr;

	*node = root->FindChild(name);

	if (*node)
	{
		attr = (*node)->GetNodeAttribute();

		if (attr && attr->GetAttributeType() == type)
			return 1;
	}

	return 0;
}

long EvaluatePropertyByChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, FbxArray<float>* channel)
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

long FillActorArray(SETUP_STRUCT* cfg, FbxNode* root, FbxNode** actor)
{
	long curr;

	curr = 0;

	if (cfg->lara.idx != -1)
	{
		if (!FindAttribute(root, cfg->lara.name, FbxNodeAttribute::eMesh, &actor[curr]))
			return 0;

		curr++;
	}

	for (int i = 0; i <= cfg->actor.idx; i++)
	{
		if (!FindAttribute(root, cfg->actor.name[i], FbxNodeAttribute::eMesh, &actor[curr]))
			return 0;

		curr++;
	}

	for (int i = curr; i < 10; i++)
		actor[i] = NULL;

	return 1;
}

long CompressChannel(FbxArray<float>* channel, FbxArray<uchar>* seq, short* number)
{
	long size;
	short diff, count;

	*number = 0;
	size = channel->Size();

	for (int i = 1; i < size; i++)
	{
		if (!((i - 1) % 16))
		{
			count = (short)(size - i);

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

		if (!AppendValue(diff & 0x7FFF, *number % 8, seq))
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

long AppendValue(ushort value, long shift, FbxArray<uchar>* seq)
{
	if (shift)
		seq->SetLast(seq->GetLast() | value << (8 - shift));

	if (seq->Add(value >> shift) == -1)
		return 0;

	if (shift != 7 && seq->Add(value >> (shift + 8)) == -1)
		return 0;

	return 1;
}

long ProcessProperty(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float m, long frames, FbxArray<uchar>* seq, short* key, short* number)
{
	FbxArray<float> channel;

	if (!EvaluatePropertyByChannel(layer, prop, name, &channel) || channel.Size() != frames)
		return 0;

	TransformChannel(m, &channel);

	if (!CompressChannel(&channel, seq, number))
		return 0;

	*key = (short)channel.GetAt(0);

	return 1;
}

long ProcessDummyProperty(float a, long frames, FbxArray<uchar>* seq, short* key, short* number)
{
	FbxArray<float> channel;

	for (int i = 0; i < frames; i++)
	{
		if (channel.Add(a) == -1)
			return 0;
	}

	if (!CompressChannel(&channel, seq, number))
		return 0;

	*key = (short)a;

	return 1;
}

long TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	FbxNode* child;
	long count;

	player->len++;
	header = (NODELOADHEADER*)realloc(player->header, player->len * sizeof(NODELOADHEADER));

	if (!header)
		return 0;

	player->header = header;
	header = &player->header[player->len - 1];
	header->packmethod = 0x3DEF;

	if (!ProcessProperty(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_X, 2.8444444444F, frames, &player->seq, &header->xkey, &header->xlength))
		return 0;

	if (!ProcessProperty(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z, -2.8444444444F, frames, &player->seq, &header->ykey, &header->ylength))
		return 0;

	if (!ProcessProperty(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y, 2.8444444444F, frames, &player->seq, &header->zkey, &header->zlength))
		return 0;

	count = node->GetChildCount();

	for (int i = 0; i < count; i++)
	{
		child = node->GetChild(i);
		if (!TraverseActorHierarchy(layer, child, frames, player))
			return 0;
	}

	return 1;
}

long PackActor(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;

	player->len = 1;
	player->header = (NODELOADHEADER*)malloc(sizeof(NODELOADHEADER));

	if (!player->header)
		return 0;

	header = player->header;
	header->packmethod = 0x3DEF;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, 0.3333333333F, frames, &player->seq, &header->xkey, &header->xlength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, -0.3333333333F, frames, &player->seq, &header->ykey, &header->ylength))
		return 0;

	if (!ProcessProperty(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, -0.3333333333F, frames, &player->seq, &header->zkey, &header->zlength))
		return 0;

	return TraverseActorHierarchy(layer, node, frames, player);
}

void TransformChannel(float m, FbxArray<float>* channel)
{
	float x, y;
	long size;

	size = channel->Size();

	for (int i = 0; i < size; i++)
	{
		x = m * channel->GetAt(i);

		if (x >= 0)
		{
			y = floorf(x);

			if (x - y >= 0.5F)
				y += 1;
		}
		else
		{
			y = ceilf(x);

			if (x - y <= -0.5F)
				y -= 1;
		}

		channel->SetAt(i, y);
	}
}

long PackCamera(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
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

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, 0.5F, frames, &player->seq, &header->xkey, &header->xlength))
			return 0;

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, -0.5F, frames, &player->seq, &header->ykey, &header->ylength))
			return 0;

		if (!ProcessProperty(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, -0.5F, frames, &player->seq, &header->zkey, &header->zlength))
			return 0;

		header++;
	}

	return 1;
}

long PackExtensions(FbxAnimLayer* layer, FbxCamera* cam, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;

	player->len = 1;
	player->header = (NODELOADHEADER*)malloc(sizeof(NODELOADHEADER));

	if (!player->header)
		return 0;

	header = player->header;
	header->packmethod = 0x3DEF;

	if (cam->Roll.IsAnimated(layer))
	{
		if (!ProcessProperty(layer, &cam->Roll, NULL, -2.8444444444F, frames, &player->seq, &header->xkey, &header->xlength))
			return 0;
	}
	else
	{
		if (!ProcessDummyProperty(0, frames, &player->seq, &header->xkey, &header->xlength))
			return 0;
	}

	if (cam->FieldOfView.IsAnimated(layer))
	{
		if (!ProcessProperty(layer, &cam->FieldOfView, NULL, 182, frames, &player->seq, &header->ykey, &header->ylength))
			return 0;
	}
	else
	{
		if (!ProcessDummyProperty(11488, frames, &player->seq, &header->ykey, &header->ylength))
			return 0;
	}

	if (!ProcessDummyProperty(0, frames, &player->seq, &header->zkey, &header->zlength))
		return 0;

	return 1;
}

long PackScene(FbxAnimLayer* layer, FbxNode* cam, FbxNode** actor, long frames, FRAME_DATA* player)
{
	long curr;

	if (!PackCamera(layer, cam, frames, player))
		return 0;

	curr = 1;

	for (int i = 0; i < 10; i++)
	{
		if (!actor[i])
			break;

		if (!PackActor(layer, actor[i], frames, &player[i + 1]))
			return 0;

		curr++;
	}

	if (!PackExtensions(layer, FbxCast<FbxCamera>(cam->GetNodeAttribute()), frames, &player[curr]))
		return 0;

	return 1;
}

long ConvertScene(SETUP_STRUCT* cfg, FRAME_DATA* player, long* frames)
{
	FbxManager* manager;
	FbxAnimLayer* layer;
	FbxNode* root;
	FbxNode* cam;
	FbxNode* actor[10];
	long r;

	r = 0;
	manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	if (ImportScene(manager, cfg->options.input, &root, frames, &layer) &&
		FindAttribute(root, cfg->options.camera, FbxNodeAttribute::eCamera, &cam) && cam->GetTarget() &&
		FillActorArray(cfg, root, actor) && PackScene(layer, cam, actor, *frames, player))
		r = 1;

	manager->Destroy();
	return r;
}
