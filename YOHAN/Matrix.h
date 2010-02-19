#ifndef _YOHAN_BASE_
#include "Base.h"
#endif

#define _YOHAN_MATRIX_

using namespace yohan;
using namespace base;

namespace yohan
{
	namespace matrix
	{
		/*

		=== Storage Formats for the Direct Sparse Solvers ===

		A real or complex array that contains the non-zero elements of a sparse
		matrix. The non-zero elements are mapped into the values array using
		the row-major upper triangular storage mapping described above.
	
		*/
		class SymmetricMumpsSquareSparseMatrix : public SquareSparseMatrix
		{
		public:
			/* The order of the square matrix */
			int order;

			/* The count of non-zero element */
			int size;

			/*
			[values]
			Element I of the integer array columns is the number of the column
			that contains the I-th element in the values array.
			*/
			DATA* values;

			/*
			[columns]
			Element j of the integer array rowIndex gives the index of the element
			in the values array that is first non-zero element in a row j.
			*/
			int* columns;

			/*
			[rows]
			The length of the values and columns arrays is equal to the number of non-zero elements in
			the matrix.
			*/
			int* rows;

			/* 
			A index to quickly find out whether a give (i,j) has already some value
			Key: i * n + j (unique)
			Value: its index in the column/row/values
			*/
			std::map<int,int> positionBuffer;

		private:
			/* The space allocated, this option could be used to optimise */
			int capacity;


		public:
			SymmetricMumpsSquareSparseMatrix(int n);

			~SymmetricMumpsSquareSparseMatrix();

			inline void setValue(int i, int j, DATA value);

			inline void addAndSetValue(int i, int j, DATA value);

			inline int getSize();

			inline int getType();

			inline int getOrder();

			inline DATA getValue(int i, int j);

			void clear();

			void changeOrder(int newOrder);

			void calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y);

			void calcul_AX(DATA alpha);

			void calcul_plusAX(SquareSparseMatrix* X, DATA alpha);

			void calcul_PlusMatrixVec(DATA* vec, DATA* res);

			void calcul_MinusMatrixVec(DATA* vec, DATA* res, DATA alpha);

			void show(char* fileName);

		private:
			void resize(int newCapacity);
		};


		/*

		=== Storage Formats for the Direct Sparse Solvers ===

		A real or complex array that contains the non-zero elements of a sparse
		matrix. The non-zero elements are mapped into the values array using
		the row-major upper triangular storage mapping described above.
	
		*/
		class SymmetricMumpsSquareSparseMatrix2 : public SquareSparseMatrix
		{
		public:
			/* The order of the square matrix */
			int order;

			/* The count of non-zero element */
			int size;

			/*
			[values]
			Element I of the integer array columns is the number of the column
			that contains the I-th element in the values array.
			*/
			DATA* values;

			/*
			[columns]
			Element j of the integer array rowIndex gives the index of the element
			in the values array that is first non-zero element in a row j.
			*/
			int* columns;

			/*
			[rows]
			The length of the values and columns arrays is equal to the number of non-zero elements in
			the matrix.
			*/
			int* rows;

		private:
			/* The space allocated, this option could be used to optimise */
			int capacity;


		public:
			SymmetricMumpsSquareSparseMatrix2(int n);

			~SymmetricMumpsSquareSparseMatrix2();

			inline void setValue(int i, int j, DATA value);

			inline void addAndSetValue(int i, int j, DATA value);

			inline int getSize();

			inline int getType();

			inline int getOrder();

			inline DATA getValue(int i, int j);

			void clear();

			void changeOrder(int newOrder);

			void calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y);

			void calcul_AX(DATA alpha);

			void calcul_plusAX(SquareSparseMatrix* X, DATA alpha);

			void calcul_PlusMatrixVec(DATA* vec, DATA* res);

			void calcul_MinusMatrixVec(DATA* vec, DATA* res, DATA alpha);

			void show(char* fileName);

		private:
			void resize(int newCapacity);
		};


		/* return the result of the given equation: Ax=b, in which implies that the order of A is equal to b's size*/
		DATA* solveLinearEquation(SquareSparseMatrix* A, DATA* b);

		/* vec(i) = vec(i) * alpha */
		void calcul_vec_AX(DATA* vec, int size, DATA alpha);

	}
}