#pragma once


namespace matrix
{
	/*

	=== Storage Formats for the Direct Sparse Solvers ===

	A real or complex array that contains the non-zero elements of a sparse
	matrix. The non-zero elements are mapped into the values array using
	the row-major upper triangular storage mapping described above.

	*/
	class SymmetricMumpsSquareSparseMatrix
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
		double* values;

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
		SymmetricMumpsSquareSparseMatrix(int n);

		~SymmetricMumpsSquareSparseMatrix();

		inline void setValue(int i, int j, double value);

		inline void addAndSetValue(int i, int j, double value);

		inline int getSize();

		inline int getType();

		int getOrder();

		inline double getValue(int i, int j);

		void clear();

		void changeOrder(int newOrder);

		void calcul_AXplusBY(double alpha, SymmetricMumpsSquareSparseMatrix* X, double beta, SymmetricMumpsSquareSparseMatrix* Y);

		void calcul_AX(double alpha);

		void calcul_plusAX(SymmetricMumpsSquareSparseMatrix* X, double alpha);

		void calcul_PlusMatrixVec(double* vec, double* res);

		void calcul_MinusMatrixVec(double* vec, double* res, double alpha);

	private:
		void resize(int newCapacity);
	};


	/* return the result of the given equation: Ax=b, in which implies that the order of A is equal to b's size*/
	double* solveLinearEquation(SymmetricMumpsSquareSparseMatrix* A, double* b);

	/* vec(i) = vec(i) * alpha */
	void calcul_vec_AX(double* vec, int size, double alpha);
}