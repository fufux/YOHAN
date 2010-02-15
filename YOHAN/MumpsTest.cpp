#include "stdafx.h"

#include "Matrix.h"

#include <iostream>

using namespace std;

using namespace yohan;

using namespace base;
using namespace matrix;

int main___()
{
	int order = 100;
	SquareSparseMatrix* m = new SymmetricMumpsSquareSparseMatrix(order);

	for (int i = 0; i < order; i++)
		m->setValue(i, i, i + 1);

	DATA* b = new DATA[order];

	for (int i = 0; i < order; i++)
		b[i] = 100 - i;

	DATA* res = solveLinearEquation(m, b);

	for (int i = 0; i < order; i++)
		cout<<res[i]<<endl;

	return 0;
	
}