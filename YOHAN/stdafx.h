// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
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
// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
