#include "stdafx.h"

#include "Matrix.h"

#include <cstdio>

using namespace yohan;
using namespace base;
using namespace matrix;

/* SquareSparseMatrix */

SquareSparseMatrix::~SquareSparseMatrix()
{
	//empty, need to verify
}

/* End -- SquareSparseMatrix */

/* SymmetricMumpsSquareSparseMatrix */

SymmetricMumpsSquareSparseMatrix::SymmetricMumpsSquareSparseMatrix(int n)
{
	this->size = 0;
	this->order = n;
	this->capacity = n;	// intial capacity = order

	this->columns = new int[capacity];
	this->rows = new int[capacity];
	this->values = new DATA[capacity];

	this->positionBuffer = std::map<int,int>();
}



void SymmetricMumpsSquareSparseMatrix::setValue(int i, int j, DATA value)
{
	//check if this position is already taken
	int key = i * this->order + j;
	std::map<int,int>::iterator iter = this->positionBuffer.find(key);

	if (iter != this->positionBuffer.end())
	{
		//found
		int index = iter->second;
		this->values[index] = value;
	}
	else
	{
		/* not found */

		//resizing
		if (this->size == this->capacity - 1)
			resize(this->capacity + this->order);	//one possible strategy

		//expand
		this->rows[this->size] = i;
		this->columns[this->size] = j;
		this->values[this->size] = value;

		this->positionBuffer[key] = this->size;
		this->size++;
	}	
}

void SymmetricMumpsSquareSparseMatrix::addAndSetValue(int i, int j, DATA value)
{
	//check if this position is already taken
	int key = i * this->order + j;
	std::map<int,int>::iterator iter = this->positionBuffer.find(key);

	if (iter != this->positionBuffer.end())
	{
		//found
		int index = iter->second;
		this->values[index] += value;
	}
	else
	{
		/* not found */

		//resizing
		if (this->size == this->capacity - 1)
			resize(this->capacity + this->order);	//one possible strategy

		//expand
		this->rows[this->size] = i;
		this->columns[this->size] = j;
		this->values[this->size] = value;

		this->positionBuffer[key] = this->size;
		this->size++;
	}	
}

int SymmetricMumpsSquareSparseMatrix::getType()
{	
	return SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse;
}

int SymmetricMumpsSquareSparseMatrix::getOrder()
{
	return this->order;
}

SymmetricMumpsSquareSparseMatrix::~SymmetricMumpsSquareSparseMatrix()
{
	/* Remark: for int, DATA, we could use DELETE instead of DELETE[] to obtain some performance */
	delete[] this->columns;
	delete[] this->rows;
	delete[] this->values;

	//delete this->positionBuffer;

	//super

}

void SymmetricMumpsSquareSparseMatrix::clear()
{
	this->size = 0;
	this->positionBuffer.clear();
}

void SymmetricMumpsSquareSparseMatrix::resize(int newCapacity)
{
	int* newColumns = new int[newCapacity];
	int* newRows = new int[newCapacity];
	DATA* newValues = new DATA[newCapacity];
	
	/*
	//copy - version 1

	for (int i = 0; i < this->size; i++)
	{
		newColumns[i] = this->columns[i];
		newRows[i] = this->rows[i];
		newValues[i] = this->values[i];
	}
	*/

	//copy - version 2
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

void SymmetricMumpsSquareSparseMatrix::changeOrder(int newOrder)
{
	this->order = newOrder;
	clear();
}

DATA SymmetricMumpsSquareSparseMatrix::getValue(int i, int j)
{
	int key = i * this->order + j;
	std::map<int,int>::iterator iter = this->positionBuffer.find(key);

	if (iter != this->positionBuffer.end())
	{
		//found
		int index = iter->second;
		return values[index];
	}

	//not found;
	return 0;
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

void SymmetricMumpsSquareSparseMatrix::calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y)
{

	if (X->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse &&
		Y->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse)
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

			DATA tmp = alpha;
			alpha = beta;
			beta = tmp;
		}

		// copy all from B (the longer one) 
		// could use memcpy for performance

		this->clear();
		for (int i = 0; i < B->size; i++)
			this->setValue(B->rows[i], B->columns[i], B->values[i] * beta);

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

inline int SymmetricMumpsSquareSparseMatrix::getSize()
{
	return this->size;
}

/* this(i,j) = this(i,j) * alpha */
void SymmetricMumpsSquareSparseMatrix::calcul_AX(DATA alpha)
{
	for (int i = 0; i < size; i++)
		values[i] *= alpha;
}

/* this(i,j) = this(i,j) + alpha * X */
void SymmetricMumpsSquareSparseMatrix::calcul_plusAX(SquareSparseMatrix* X, DATA alpha)
{
	if (X->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse)
	{
		SymmetricMumpsSquareSparseMatrix* A = (SymmetricMumpsSquareSparseMatrix*)X;
		
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
void SymmetricMumpsSquareSparseMatrix::calcul_PlusMatrixVec(DATA* vec, DATA* res)
{
	for (int i = 0; i < size; i++)
	{
		int column = columns[i];	
		int row = rows[i];

		res[row - 1] += values[i] * vec[column - 1];	// conform
	}
}

void SymmetricMumpsSquareSparseMatrix::calcul_MinusMatrixVec(DATA* vec, DATA* res, DATA alpha)
{
	for (int i = 0; i < size; i++)
	{
		int column = columns[i];
		int row = rows[i];

		res[row - 1] -= values[i] * vec[column - 1] * alpha;	// conform
	}
}


