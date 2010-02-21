#include "stdafx.h"

#include "Matrix.h"

#include "dmumps_c.h"

using namespace matrix;

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

/* Solve Mx = b */
double* matrix::solveLinearEquation(SymmetricMumpsSquareSparseMatrix* A, double* b)
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

/* vec(i) = vec(i) * alpha */
void matrix::calcul_vec_AX(double* vec, int size, double alpha)
{
	for (int i = 0; i < size; i++)
		vec[i] *= alpha;
}
