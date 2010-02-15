// stdafx.cpp : fichier source incluant simplement les fichiers Include standard
// YOHAN.pch représente l'en-tête précompilé
// stdafx.obj contient les informations de type précompilées

#include "stdafx.h"

// TODO : faites référence aux en-têtes supplémentaires nécessaires dans STDAFX.H
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