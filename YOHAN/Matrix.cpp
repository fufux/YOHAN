#include "stdafx.h"

#include "Matrix.h"

#include "dmumps_c.h"

using namespace yohan;

using namespace base;
using namespace matrix;

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

/* Solve Mx = b */
DATA* yohan::matrix::solveLinearEquation(SquareSparseMatrix* M, DATA* b)
{
	//for SymmetricUpperSquareSparseMatrix, using MUMPS
	if (M->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse)
	{
		/* Platform MUMPS relatives*/
		DMUMPS_STRUC_C id;

		/* JOB_INIT */

		id.job = -1;	// step: JOB_INIT
		id.par = 1;		// the host will participate the factorisation/solve phase, in SEQ version, 1 is better
		id.sym = 0;		// 2 - general symmetric; 1 - symmetric positive definite; 0 - unsymmetric
		//id.comm_fortran = -987654;
		dmumps_c(&id);

		/* Define the problem on the host */

		SymmetricMumpsSquareSparseMatrix* A = (SymmetricMumpsSquareSparseMatrix*)M;
		id.n = A->order;	//order
		id.nz = A->size;	//count of non-element
		id.irn = A->rows;
		id.jcn = A->columns;
		id.a = A->values;
		id.rhs = b;

		/* Configuration of the solver */
		//id.ICNTL(5) = 0;	// the matrix is defined in an "assembled format", (i, j, val)

		id.ICNTL(1) = -1;
		id.ICNTL(2) = -1;
		id.ICNTL(3) = -1;
		id.ICNTL(4) = 0;	// all these are for the dignostic display, put on -1 if not necessary

		/* Call the solver */
		id.job = 6;

		dmumps_c(&id);

		/* We should consider here whether we desallocate the M and the b */

		/* JOB_END */
		id.job=-2;
		dmumps_c(&id);
		
		
		/* Return */
		return b;

	}
	else if (M->getType() == SquareSparseMatrix::TYPE_SymmetricMumpsSquareSparse2)
	{
		/* Platform MUMPS relatives*/
		DMUMPS_STRUC_C id;

		/* JOB_INIT */

		id.job = -1;	// step: JOB_INIT
		id.par = 1;		// the host will participate the factorisation/solve phase, in SEQ version, 1 is better
		id.sym = 0;		// 2 - general symmetric; 1 - symmetric positive definite; 0 - unsymmetric
		//id.comm_fortran = -987654;
		dmumps_c(&id);

		/* Define the problem on the host */

		SymmetricMumpsSquareSparseMatrix2* A = (SymmetricMumpsSquareSparseMatrix2*)M;
		id.n = A->order;	//order
		id.nz = A->size;	//count of non-element
		id.irn = A->rows;
		id.jcn = A->columns;
		id.a = A->values;
		id.rhs = b;

		/* Configuration of the solver */
		//id.ICNTL(5) = 0;	// the matrix is defined in an "assembled format", (i, j, val)

		id.ICNTL(1) = -1;
		id.ICNTL(2) = -1;
		id.ICNTL(3) = -1;
		id.ICNTL(4) = 0;	// all these are for the dignostic display, put on -1 if not necessary

		/* Call the solver */
		id.job = 6;

		dmumps_c(&id);

		/* We should consider here whether we desallocate the M and the b */

		/* JOB_END */
		id.job=-2;
		dmumps_c(&id);
		
		
		/* Return */
		return b;

	}
	else
		return NULL;

	//if (A->getType() == SquareSparseMatrix::TYPE_SymmetricIntelSquareSparse)

}

/* vec(i) = vec(i) * alpha */
void yohan::matrix::calcul_vec_AX(DATA* vec, int size, DATA alpha)
{
	for (int i = 0; i < size; i++)
		vec[i] *= alpha;
}
