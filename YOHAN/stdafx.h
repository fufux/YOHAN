// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#include "targetver.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <list>

#include <windows.h>

#include <irrlicht.h>

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define CAMERA_COUNT 2

// here define some captions used by the GUI
#define CAPTION_ERROR L"Error"
#define CAPTION_HELP L"Help"

int myRandom(int a, int b);
float myRandom(float a, float b);
double myRandom(double a, double b);
void createDir(const char* dirName);
// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
