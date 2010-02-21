#include "stdafx.h"

#include "Matrix.h"

#include <cstdio>

using namespace matrix;

/* SymmetricMumpsSquareSparseMatrix */

SymmetricMumpsSquareSparseMatrix::SymmetricMumpsSquareSparseMatrix(int n)
{
	this->size = 0;
	this->order = n;
	this->capacity = n * 2;	// intial capacity = order * 2

	this->columns = new int[capacity];
	this->rows = new int[capacity];
	this->values = new double[capacity];
}



void SymmetricMumpsSquareSparseMatrix::setValue(int i, int j, double value)
{
	for (int k = 0; k < size; k++)
	{
		if (rows[k] == i && columns[k] == j)
			rows[k] = -1;	// will be ignore by MUMPS
	}

	//resizing
	if (size == capacity - 1)
		resize(capacity + order * 2);

	rows[size] = i;
	columns[size] = j;
	values[size] = value;

	++size;

}

void SymmetricMumpsSquareSparseMatrix::addAndSetValue(int i, int j, double value)
{
	//resizing
	if (size == capacity - 1)
		resize(capacity * 2);

	rows[size] = i;
	columns[size] = j;
	values[size] = value;

	++size;
}

int SymmetricMumpsSquareSparseMatrix::getOrder()
{
	return this->order;
}

SymmetricMumpsSquareSparseMatrix::~SymmetricMumpsSquareSparseMatrix()
{
	/* Remark: for int, double, we could use DELETE instead of DELETE[] to obtain some performance */
	delete[] this->columns;
	delete[] this->rows;
	delete[] this->values;
}

void SymmetricMumpsSquareSparseMatrix::clear()
{
	memset((void*)columns, 0, size * sizeof(int));
	memset((void*)rows, 0, size * sizeof(int));
	memset((void*)values, 0, size * sizeof(double));

	this->size = 0;
}

void SymmetricMumpsSquareSparseMatrix::resize(int newCapacity)
{
	int* newColumns = new int[newCapacity];
	int* newRows = new int[newCapacity];
	double* newValues = new double[newCapacity];

	//copy
	memcpy((void*)newColumns, (void*)this->columns, this->size * sizeof(int));
	memcpy((void*)newRows, (void*)this->rows, this->size * sizeof(int));
	memcpy((void*)newValues, (void*)this->values, this->size * sizeof(double));

	//release the olds
	delete[] this->columns;
	delete[] this->rows;
	delete[] this->values;

	//config
	this->capacity = newCapacity;
	this->columns = newColumns;
	this->rows = newRows;
	this->values = newValues;
}

void SymmetricMumpsSquareSparseMatrix::changeOrder(int newOrder)
{
	this->order = newOrder;
	clear();
}

double SymmetricMumpsSquareSparseMatrix::getValue(int i, int j)
{
	double val = 0;
	for (int k = 0; k < size; k++)
	{
		if (rows[k] == i && columns[k] == j)
			val += values[k];
	}

	return val;
}

void SymmetricMumpsSquareSparseMatrix::calcul_AXplusBY(double alpha, SymmetricMumpsSquareSparseMatrix* X, double beta, SymmetricMumpsSquareSparseMatrix* Y)
{
	SymmetricMumpsSquareSparseMatrix* A;
	SymmetricMumpsSquareSparseMatrix* B;

	// A.size < B.size
	if (X->getSize() < Y->getSize())
	{
		A = (SymmetricMumpsSquareSparseMatrix*)X;
		B = (SymmetricMumpsSquareSparseMatrix*)Y;
	}
	else
	{
		A = (SymmetricMumpsSquareSparseMatrix*)Y;
		B = (SymmetricMumpsSquareSparseMatrix*)X;

		double tmp = alpha;
		alpha = beta;
		beta = tmp;
	}

	// copy all from B (the longer one) and multiply by beta 
	// use memcpy for performance

	this->clear();
	if(this->capacity < B->size + A->size)
		resize(B->size + A->size);

	//copy
	memcpy((void*)this->columns, (void*)B->columns, B->size * sizeof(int));
	memcpy((void*)this->rows, (void*)B->rows, B->size * sizeof(int));
	memcpy((void*)this->values, (void*)B->values, B->size * sizeof(double));
	this->size = B->size;

	//multiply
	for (int i = 0; i < this->size; i++)
		values[i] *= beta;

	//* this(i,j) = this(i,j) + alpha * A */
	for (int i = 0; i < A->size; i++)		
		this->addAndSetValue(A->rows[i], A->columns[i], A->values[i] * alpha);

}

inline int SymmetricMumpsSquareSparseMatrix::getSize()
{
	return this->size;
}

/* this(i,j) = this(i,j) * alpha */
void SymmetricMumpsSquareSparseMatrix::calcul_AX(double alpha)
{
	for (int i = 0; i < size; i++)
		values[i] *= alpha;
}

/* this(i,j) = this(i,j) + alpha * X */
void SymmetricMumpsSquareSparseMatrix::calcul_plusAX(SymmetricMumpsSquareSparseMatrix* X, double alpha)
{
	SymmetricMumpsSquareSparseMatrix* A = (SymmetricMumpsSquareSparseMatrix*)X;
	
	for (int i = 0; i < A->size; i++)
		this->addAndSetValue(A->rows[i], A->columns[i], A->values[i] * alpha);
}

/* RES = RES + M*VEC */
/* res(i) += M(i,1) * vec(1) + M(i,2) * vec(2) + ... */
void SymmetricMumpsSquareSparseMatrix::calcul_PlusMatrixVec(double* vec, double* res)
{
	for (int i = 0; i < size; i++)
	{
		int column = columns[i];	
		int row = rows[i];

		if (row < 0 || column < 0)
			continue;

		res[row - 1] += values[i] * vec[column - 1];	// conform
	}
}

void SymmetricMumpsSquareSparseMatrix::calcul_MinusMatrixVec(double* vec, double* res, double alpha)
{
	for (int i = 0; i < size; i++)
	{
		int column = columns[i];
		int row = rows[i];

		if (row < 0 || column < 0)
			continue;

		res[row - 1] -= values[i] * vec[column - 1] * alpha;	// conform
	}
}


