#include "stdafx.h"

#include "Matrix.h"

#include <cstdio>

using namespace yohan;
using namespace base;
using namespace matrix;

/* SymmetricMumpsSquareSparseMatrix2 */

SymmetricMumpsSquareSparseMatrix2::SymmetricMumpsSquareSparseMatrix2(int n)
{
	this->size = 0;
	this->order = n;
	this->capacity = n * 2;	// intial capacity = order * 2

	this->columns = new int[capacity];
	this->rows = new int[capacity];
	this->values = new DATA[capacity];
}



void SymmetricMumpsSquareSparseMatrix2::setValue(int i, int j, DATA value)
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

void SymmetricMumpsSquareSparseMatrix2::addAndSetValue(int i, int j, DATA value)
{
	//resizing
	if (size == capacity - 1)
		resize(capacity + order * 2);

	rows[size] = i;
	columns[size] = j;
	values[size] = value;

	++size;
}

int SymmetricMumpsSquareSparseMatrix2::getType()
{	
	return SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse2;
}

int SymmetricMumpsSquareSparseMatrix2::getOrder()
{
	return this->order;
}

SymmetricMumpsSquareSparseMatrix2::~SymmetricMumpsSquareSparseMatrix2()
{
	/* Remark: for int, DATA, we could use DELETE instead of DELETE[] to obtain some performance */
	delete[] this->columns;
	delete[] this->rows;
	delete[] this->values;
}

void SymmetricMumpsSquareSparseMatrix2::clear()
{
	this->size = 0;
}

void SymmetricMumpsSquareSparseMatrix2::resize(int newCapacity)
{
	int* newColumns = new int[newCapacity];
	int* newRows = new int[newCapacity];
	DATA* newValues = new DATA[newCapacity];

	//copy
	memcpy((void*)newColumns, (void*)this->columns, this->size * sizeof(int));
	memcpy((void*)newRows, (void*)this->rows, this->size * sizeof(int));
	memcpy((void*)newValues, (void*)this->values, this->size * sizeof(DATA));

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

void SymmetricMumpsSquareSparseMatrix2::changeOrder(int newOrder)
{
	this->order = newOrder;
	clear();
}

DATA SymmetricMumpsSquareSparseMatrix2::getValue(int i, int j)
{
	DATA val = 0;
	for (int k = 0; k < size; k++)
	{
		if (rows[k] == i && columns[k] == j)
			val += values[k];
	}

	return val;
}

/*
void SymmetricMumpsSquareSparseMatrix::calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y)
{

	if (X->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse &&
		Y->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse)
	{
		SymmetricMumpsSquareSparseMatrix* A;
		SymmetricMumpsSquareSparseMatrix* B;

		if (X->getSize() < Y->getSize())
		{
			A = (SymmetricMumpsSquareSparseMatrix*)X;
			B = (SymmetricMumpsSquareSparseMatrix*)Y;
		}
		else
		{
			A = (SymmetricMumpsSquareSparseMatrix*)Y;
			B = (SymmetricMumpsSquareSparseMatrix*)X;

			DATA tmp = alpha;
			alpha = beta;
			beta = tmp;
		}

		std::map<int,int> testMap = std::map<int,int>();

		for (int i = 0; i < A->size; i++)
		{
			int row = A->rows[i];
			int column = A->columns[i];

			this->setValue(row, column, A->values[i] * alpha + B->getValue(row, column) * beta);

			int key = row * this->order + column;
			testMap[key] = 0;
		}

		for (int i = 0; i < B->size; i++)
		{
			int row = B->rows[i];
			int column = B->columns[i];
	
			int key = row * this->order + column;
			if (testMap.find(key) != testMap.end())	// already added
				continue;

			this->setValue(row, column, B->values[i] * beta + A->getValue(row, column) * alpha);
		}
	}
	else
	{
		printf("Check your matrix, their type is not correct.");
		fetalError();
	}
}
*/

void SymmetricMumpsSquareSparseMatrix2::calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y)
{

	if (X->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse2 &&
		Y->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse2)
	{
		SymmetricMumpsSquareSparseMatrix2* A;
		SymmetricMumpsSquareSparseMatrix2* B;

		// A.size < B.size
		if (X->getSize() < Y->getSize())
		{
			A = (SymmetricMumpsSquareSparseMatrix2*)X;
			B = (SymmetricMumpsSquareSparseMatrix2*)Y;
		}
		else
		{
			A = (SymmetricMumpsSquareSparseMatrix2*)Y;
			B = (SymmetricMumpsSquareSparseMatrix2*)X;

			DATA tmp = alpha;
			alpha = beta;
			beta = tmp;
		}

		// copy all from B (the longer one) and multiply by beta 
		// use memcpy for performance

		this->clear();
		if(this->capacity < B->capacity)
			resize(B->capacity + 2 * order);

		//copy
		memcpy((void*)this->columns, (void*)B->columns, B->size * sizeof(int));
		memcpy((void*)this->rows, (void*)B->rows, B->size * sizeof(int));
		memcpy((void*)this->values, (void*)B->values, B->size * sizeof(DATA));
		this->size = B->size;

		//multiply
		for (int i = 0; i < this->size; i++)
			values[i] *= beta;

		//* this(i,j) = this(i,j) + alpha * A */
		for (int i = 0; i < A->size; i++)		
			this->addAndSetValue(A->rows[i], A->columns[i], A->values[i] * alpha);
	}
	else
	{
		printf("Check your matrix, their type is not correct.");
		fetalError();
	}
}

inline int SymmetricMumpsSquareSparseMatrix2::getSize()
{
	return this->size;
}

/* this(i,j) = this(i,j) * alpha */
void SymmetricMumpsSquareSparseMatrix2::calcul_AX(DATA alpha)
{
	for (int i = 0; i < size; i++)
		values[i] *= alpha;
}

/* this(i,j) = this(i,j) + alpha * X */
void SymmetricMumpsSquareSparseMatrix2::calcul_plusAX(SquareSparseMatrix* X, DATA alpha)
{
	if (X->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse2)
	{
		SymmetricMumpsSquareSparseMatrix2* A = (SymmetricMumpsSquareSparseMatrix2*)X;
		
		for (int i = 0; i < A->size; i++)
			this->addAndSetValue(A->rows[i], A->columns[i], A->values[i] * alpha);
	}
	else
	{
		printf("Check your matrix, their type is not correct.");
		fetalError();
	}

}

/* RES = RES + M*VEC */
/* res(i) += M(i,1) * vec(1) + M(i,2) * vec(2) + ... */
void SymmetricMumpsSquareSparseMatrix2::calcul_PlusMatrixVec(DATA* vec, DATA* res)
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

void SymmetricMumpsSquareSparseMatrix2::calcul_MinusMatrixVec(DATA* vec, DATA* res, DATA alpha)
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


