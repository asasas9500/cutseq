#include "stdafx.h"
#include "main.h"
#include "scene.h"
#include "setup.h"

int main()
{
	SETUP_STRUCT cfg;

	if (GetConfiguration(&cfg))
		Convert(&cfg);

	return 0;
}
