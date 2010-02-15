#include "stdafx.h"

#include "Base.h"

#include <windows.h>	// for yohan::base::createDir(const char* dirName)

using namespace std;

using namespace yohan;
using namespace base;

void yohan::base::fetalError()
{
	printf("Fetal Error\n");
	exit(1);
}

/* This implementation is only for Windows, if you want to use it in Linux or for other platform, use mkdir instead */
void yohan::base::createDir(const char* dirName)
{
	WCHAR wsz[256];
	swprintf(wsz, L"%S", dirName);
	CreateDirectory(wsz, NULL);
}