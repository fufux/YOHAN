// stdafx.cpp�: fichier source incluant simplement les fichiers Include standard
// YOHAN.pch repr�sente l'en-t�te pr�compil�
// stdafx.obj contient les informations de type pr�compil�es

#include "stdafx.h"

// TODO�: faites r�f�rence aux en-t�tes suppl�mentaires n�cessaires dans STDAFX.H
// absents de ce fichier
int myRandom(int a, int b)
{
	return ( a + (int)( (double)rand() / (double)RAND_MAX * (b - a) ) );
}

float myRandom(float a, float b)
{
	return ( a + (float)( (double)rand() / (double)RAND_MAX * (b - a) ) );
}

double myRandom(double a, double b)
{
	return ( a + (double)( (double)rand() / (double)RAND_MAX * (b - a) ) );
}