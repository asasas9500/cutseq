#include "stdafx.h"
#include "scene.h"
extern "C"
{
#include "euler_angle/EulerAngles.h"
}

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
			FbxAxisSystem::Max.ConvertScene(scene);
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

long EvaluateChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, long frames, float** channel)
{
	FbxAnimCurve* curve;
	FbxTime time;

	curve = prop->GetCurve(layer, name);

	if (!curve)
		return 0;

	*channel = (float*)malloc(frames * sizeof(float));

	if (!*channel)
		return 0;

	for (int i = 0; i < frames; i++)
	{
		time.SetFrame(i, FbxTime::eFrames30);
		(*channel)[i] = curve->Evaluate(time);
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

long CompressChannel(float* channel, long frames, FbxArray<uchar>* seq, short* number)
{
	short diff, count;

	*number = 0;

	for (int i = 1; i < frames; i++)
	{
		if (!((i - 1) % 16))
		{
			count = (short)(frames - i);

			if (count >= 16)
				count = 0;

			if (!AppendValue(32 | count, *number % 8, seq))
				return 0;

			(*number)++;
		}

		diff = (short)(channel[i] - channel[i - 1]);

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

long ProcessChannel(float m, long mask, long frames, float* channel, FbxArray<uchar>* seq, short* key, short* number)
{
	TransformChannel(m, frames, channel);

	if (!CompressChannel(channel, frames, seq, number))
		return 0;

	*key = (short)channel[0] & mask;

	return 1;
}

long EvaluateDummyChannel(float a, long frames, float** channel)
{
	*channel = (float*)malloc(frames * sizeof(float));

	if (!*channel)
		return 0;

	for (int i = 0; i < frames; i++)
		(*channel)[i] = a;

	return 1;
}

long ProvideChannel(FbxAnimLayer* layer, FbxProperty* prop, const char* name, float a, long frames, float** channel)
{
	if (prop->IsAnimated(layer))
	{
		if (!EvaluateChannel(layer, prop, NULL, frames, channel))
			return 0;
	}
	else if (!EvaluateDummyChannel(a, frames, channel))
		return 0;

	return 1;
}

long PackActorRotation(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	EulerAngles rotation;
	HMatrix matrix;
	float* channel[3];
	long r;

	r = 0;
	player->len++;
	header = (NODELOADHEADER*)realloc(player->header, player->len * sizeof(NODELOADHEADER));

	if (header)
	{
		player->header = header;
		header = &player->header[player->len - 1];
		header->packmethod = 0x3DEF;

		for (int i = 0; i < 3; i++)
			channel[i] = NULL;

		if (EvaluateChannel(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_X, frames, &channel[0]) &&
			EvaluateChannel(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y, frames, &channel[1]) &&
			EvaluateChannel(layer, &node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z, frames, &channel[2]))
		{
			for (int i = 0; i < frames; i++)
			{
				rotation.x = 0.0174532925F * channel[0][i];
				rotation.y = 0.0174532925F * channel[1][i];
				rotation.z = 0.0174532925F * channel[2][i];
				rotation.w = EulOrdXYZs;
				Eul_ToHMatrix(rotation, matrix);
				rotation = Eul_FromHMatrix(matrix, EulOrdZXYr);
				channel[0][i] = 57.295779513F * rotation.y;
				channel[1][i] = 57.295779513F * rotation.z;
				channel[2][i] = 57.295779513F * rotation.x;
			}

			if (ProcessChannel(2.8444444444F, 0x3FF, frames, channel[0], &player->seq, &header->xkey, &header->xlength) &&
				ProcessChannel(-2.8444444444F, 0x3FF, frames, channel[2], &player->seq, &header->ykey, &header->ylength) &&
				ProcessChannel(2.8444444444F, 0x3FF, frames, channel[1], &player->seq, &header->zkey, &header->zlength) &&
				TraverseActorHierarchy(layer, node, frames, player))
				r = 1;
		}

		for (int i = 0; i < 3; i++)
			free(channel[i]);
	}

	return r;
}

long TraverseActorHierarchy(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	FbxNode* child;
	long count;

	count = node->GetChildCount();

	for (int i = 0; i < count; i++)
	{
		child = node->GetChild(i);

		if (!PackActorRotation(layer, child, frames, player))
			return 0;
	}

	return 1;
}

long PackActor(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	float* channel[3];
	long r;

	r = 0;
	player->len = 1;
	player->header = (NODELOADHEADER*)malloc(sizeof(NODELOADHEADER));

	if (player->header)
	{
		header = player->header;
		header->packmethod = 0x3DEF;

		for (int i = 0; i < 3; i++)
			channel[i] = NULL;

		if (EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, frames, &channel[0]) &&
			EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, frames, &channel[1]) &&
			EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, frames, &channel[2]) &&
			ProcessChannel(0.3333333333F, 0xFFFF, frames, channel[0], &player->seq, &header->xkey, &header->xlength) &&
			ProcessChannel(-0.3333333333F, 0xFFFF, frames, channel[2], &player->seq, &header->ykey, &header->ylength) &&
			ProcessChannel(0.3333333333F, 0xFFFF, frames, channel[1], &player->seq, &header->zkey, &header->zlength) &&
			PackActorRotation(layer, node, frames, player))
			r = 1;

		for (int i = 0; i < 3; i++)
			free(channel[i]);
	}

	return r;
}

void TransformChannel(float m, long frames, float* channel)
{
	float x, y;

	for (int i = 0; i < frames; i++)
	{
		x = m * channel[i];

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

		channel[i] = y;
	}
}

long PackCamera(FbxAnimLayer* layer, FbxNode* node, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	FbxNode* target;
	float* channel[2][3];
	long r;

	r = 0;
	player->len = 2;
	player->header = (NODELOADHEADER*)malloc(2 * sizeof(NODELOADHEADER));

	if (player->header)
	{
		target = node->GetTarget();
		header = player->header;
		header->packmethod = 0x3DEF;

		for (int i = 0; i < 3; i++)
			channel[0][i] = NULL;

		if (EvaluateChannel(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, frames, &channel[0][0]) &&
			EvaluateChannel(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, frames, &channel[0][1]) &&
			EvaluateChannel(layer, &target->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, frames, &channel[0][2]) &&
			ProcessChannel(0.5F, 0xFFFF, frames, channel[0][0], &player->seq, &header->xkey, &header->xlength) &&
			ProcessChannel(-0.5F, 0xFFFF, frames, channel[0][2], &player->seq, &header->ykey, &header->ylength) &&
			ProcessChannel(0.5F, 0xFFFF, frames, channel[0][1], &player->seq, &header->zkey, &header->zlength))
		{
			header++;
			header->packmethod = 0x3DEF;

			for (int i = 0; i < 3; i++)
				channel[1][i] = NULL;

			if (EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X, frames, &channel[1][0]) &&
				EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y, frames, &channel[1][1]) &&
				EvaluateChannel(layer, &node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z, frames, &channel[1][2]) &&
				ProcessChannel(0.5F, 0xFFFF, frames, channel[1][0], &player->seq, &header->xkey, &header->xlength) &&
				ProcessChannel(-0.5F, 0xFFFF, frames, channel[1][2], &player->seq, &header->ykey, &header->ylength) &&
				ProcessChannel(0.5F, 0xFFFF, frames, channel[1][1], &player->seq, &header->zkey, &header->zlength))
				r = 1;

			for (int i = 0; i < 3; i++)
				free(channel[1][i]);
		}

		for (int i = 0; i < 3; i++)
			free(channel[0][i]);
	}

	return r;
}

long PackExtensions(FbxAnimLayer* layer, FbxCamera* cam, long frames, FRAME_DATA* player)
{
	NODELOADHEADER* header;
	float* channel[3];
	long r;

	r = 0;
	player->len = 1;
	player->header = (NODELOADHEADER*)malloc(sizeof(NODELOADHEADER));

	if (player->header)
	{
		header = player->header;
		header->packmethod = 0x3DEF;

		for (int i = 0; i < 3; i++)
			channel[i] = NULL;

		if (ProvideChannel(layer, &cam->Roll, NULL, 0, frames, &channel[0]) &&
			ProvideChannel(layer, &cam->FieldOfView, NULL, 63.1208791208F, frames, &channel[1]) &&
			EvaluateDummyChannel(0, frames, &channel[2]) &&
			ProcessChannel(2.8444444444F, 0xFFFF, frames, channel[0], &player->seq, &header->xkey, &header->xlength) &&
			ProcessChannel(182, 0xFFFF, frames, channel[1], &player->seq, &header->ykey, &header->ylength) &&
			ProcessChannel(0, 0xFFFF, frames, channel[2], &player->seq, &header->zkey, &header->zlength))
			r = 1;

		for (int i = 0; i < 3; i++)
			free(channel[i]);
	}

	return r;
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
