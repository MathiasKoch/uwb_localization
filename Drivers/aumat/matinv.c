/** \file matinv.c
 * \brief This part of the C language library iau_mat provides some advanced 
 * functions to invert a matrix. These functions may allocate memory
 * at runtime.                                            
 *  
 * All matrices are represented by a structure with the dimensions 
 * of the matrix and a pointer to an array of pointers to each row 
 * in the matrix. The functions using stdio can be excluded by        
 * defining the symbol NO_IO to the compiler.                 
 * 
 * References: Brugervejledning til C/Matlab matrix bibliotek,     
 *             Appendix F i Noergaard og Torp (1993).              
 *
 * \author O. Ravn
 * \author A. Pjetursson
 * \author P.M. Noergaard
 * \author S. Torp
 * 
 * \date 23/01-1998
 * \version 1.1
 */ 
 
 /***********************************************************************
 * $Source: /home/lvmogensen/AUTcvs/repository/aumat/matinv.c,v $
 * $Log: not supported by cvs2svn $
 * Revision 1.1  1998/01/23 12:50:29  or
 * Matrix.c divided into 3 parts containing core, io and inv functions
 *
 * Revision 1.3  1998/01/19 09:04:08  pmn
 * Integrated with PMN's version of matrix2.c.
 *
 * Revision 1.2  1998/01/14 13:01:42  or
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1997/08/12 13:58:49  or
 * Matrix Library for RealTime use
 *
 * Revision 1.3  1995/08/03  11:14:07  ap
 * "fopen" removed from matload
 *
 * Revision 1.2  1995/06/02  14:00:38  ap
 * Small updates
 *
 * Revision 1.1.1.1  1995/05/17  12:02:07  ap
 * This realtime system is initially written by P.M. Noergaard and S. Torp. This
 * new version and software structure are made by O. Ravn and A. Pjetursson.
 *
 **********************************************************************/

#include <stdlib.h>

#include "iau_mat.h"     
#include "port.h"     

matrix *b;
matrix *indxc, *indxr, *ipiv;

#define TRUE 1     ///< Definition of a basic logical value
#define FALSE 0    ///< Definition of a basic logical value
#define FOUND 1    ///< Definition of a basic logical value
#define NOTFOUND 0 ///< Definition of a basic logical value

/** \fn void minv(matrix *ptm1, matrix *ptm2)
 * This function computes the inverse of a matrix.
 * \param[out] *ptm1 Pointer to result matrix.
 * \param[in]  *ptm2 Pointer to argument matrix.
 * 
 * \attention Notice the to matricies may NOT be the same.
 * MINV uses the two functions LUDCMP and LUBKSB.
 */
void minv(matrix *ptm1, matrix *ptm2)
{

#define TINY 1.0e-20
float d, *col;
int N = 0, i, j, *indx = NULL;


#if RUNCHK == ON
	if (ptm1 == ptm2) merror("Memory conflict error in MINV!");
#endif

	if (ptm2->row == ptm2->col) 
		N = ptm2->col;
	else 
		merror("Non square matrix in function MINV!");

	ludcmp(ptm2, N, indx, &d);

	col = (float*)malloc(N*sizeof(float));

	for (j=0; j<=(N-1); j++) {
		for (i=0; i<=(N-1); i++) col[i]=0.0;
		col[j]=1.0;
		lubksb(ptm2, N, indx, col);
		for(i=0; i<=(N-1); i++) ptm1->mat[i][j]=col[i];
	}

	free(col);
}

/** \fn void ludcmp(matrix *a, int N, int *indx, float *d)
 * This function makes a LU-decomposition.
 * This function is used by MINV.
 * \param[out] *a     Matrix to hold the result
 * \param[in]  N      Not known
 * \param[in]  *indx  Pointer Not known
 * \param[in] *d      Pointer Not known
 *
 * \attention This documentation is not checked for correctness.
 * \todo Check the documentation and complete it.
 */
void ludcmp(matrix *a, int N, int *indx, float *d)
{
	int i, imax = 0, j, k;
	float big, dum, sum, temp;
	matrix *vv;

	vv = mmake(N,1);
	*d = 1.0;
	for (i=0; i<=(N-1); i++) {
		big=0.0;
		for (j=0; j<=(N-1); j++)
			if ( (temp = abs(a->mat[i][j])) > big) big=temp;
		if (big==0.0) merror("Singular matrix in routine LUDCMP");
			vv->mat[i][0]=1.0/big;
	}

	for (j=0; j<=(N-1); j++) {
		for (i=0; i<j; i++) {
			sum = a->mat[i][j];
			for (k=0; k<i; k++)  sum -= a->mat[i][k]*a->mat[k][j];
			a->mat[i][j] = sum;
		}

		big = 0.0;
		for (i=j; i<=(N-1); i++) {
			sum = a->mat[i][j];
			for (k=0; k<j; k++)
				sum -= a->mat[i][k]*a->mat[k][j];
			a->mat[i][j] = sum;
			if ( (dum=vv->mat[i][1]*abs(sum)) >= big) {
				big = dum;
				imax=i;
			}
		}

		if (j != imax) {
			for (k=0; k<=(N-1); k++) {
				dum = a->mat[imax][k];
				a->mat[imax][k] = a->mat[j][k];
				a->mat[j][k] = dum;
			}
			*d = -(*d);
			vv->mat[imax][1] = vv->mat[j][1];
		}
		
		indx[j] = imax;
		if (a->mat[j][j] == 0.0) a->mat[j][j] = TINY;
			if (j != (N-1)) {
			dum =1.0/(a->mat[j][j]);
			for (i=j+1; i<=(N-1); i++) a->mat[i][j] *= dum;
		}

	}
	mfree(vv);
}



/** \fn void lubksb( matrix *a, int N, int *indx, float b[] )
 * LU-backsubstituting.
 * This function is used by MINV.
 *
 * \param[out] *a Pointer to the result
 * \param[in]  N Not known
 * \param[in]  *indx Pointer to not known
 * \param[in]  b[] Pointer to not known
 *
 * \attention This documentation is not checked for correctness.
 * \todo Check the documentation and complete it.
 */
void lubksb( matrix *a, int N, int *indx, float b[] )
{
	int i, ii = 0, ip, j;
	float sum;
		for (i=0; i<=(N-1); i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii)
			for (j=(ii-1); j<=i-1; j++)  sum -= a->mat[i][j]*b[j];
		else
			if (sum) ii=i+1;
		b[i] = sum;
	}
	for (i=(N-1); i>=0; i--) {
		sum = b[i];
		for (j=i+1; j<=(N-1); j++) sum -= a->mat[i][j]*b[j];
		b[i] = sum/a->mat[i][i];
	}
}


void minvinit(int n){
	b = mmake(n,n);

	indxc=mmake(1,n);

	indxr=mmake(1,n);

	ipiv=mmake(1,n);
}



/** \fn minvgauss( matrix *ptm1, matrix *ptm2 )
 * This function computes the inverse of a matrix
 * using the Gauss-Jordan elimination method.
 *
 * \param[in] *ptm2 Pointer to argument matrix.
 * \param[out] *ptm1 Pointer to result matrix.
 * 
 * \attention Notice the to matricies may NOT be the same.
 * MINVGAUSS uses the function GAUSSJ.
 */
int minvgauss( matrix *ptm1, matrix *ptm2 )
{
   int n,m;

   n = getcols(ptm2);
   m = n;
   
   mresize(b, n, n);

   mdiag(b,1);

   mset(ptm1,ptm2);

   return gaussj(ptm1,n,b,m);
   
   //mfree(b);
}

/** \def SWAP(a,b)
 * Swaps the float values a and b using a temp value.
 * \param a First value to swap
 * \param b Second value to swap
 */
#define SWAP(a,b) {float (temp)=(a);(a)=(b);(b)=(temp);}

/** \fn void gaussj( matrix *a, int n, matrix *b, int m )
 * Gauss-Jordan elimination method.
 * \param[out] *a Pointer to the result matrix.
 * \param[in]  n Dimension of the output system.
 * \param[in]  *b Pointer to the input matrix.
 * \param[in]  m Dimension of the input system
 *
 * \attention This documentation is not checked for correctness.
 * \todo Check the documentation and complete it.
 */
int gaussj( matrix *a, int n, matrix *b, int m )
{
	int i,icol = 0,irow = 0,j,k,l,ll;
	float big,dum,pivinv;

   	mresize(indxc, 1, n);

   	mresize(indxr, 1, n);

   	mresize(ipiv, 1, n);


	for (j=0;j<n;j++) ipiv->mat[0][j]=0;
	for (i=0;i<n;i++) {
		big=0.0;
		for (j=0;j<n;j++)
			if (ipiv->mat[0][j] != 1)
				for (k=0;k<n;k++) {
					if (ipiv->mat[0][k] == 0) {
						if (abs(a->mat[j][k]) >= big) {
							big=abs(a->mat[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv->mat[0][k] > 1){
						merror("GAUSSJ: Singular Matrix-1");
						return 1;
					}
				}
		++(ipiv->mat[0][icol]);
		if (irow != icol) {
			for (l=0;l<n;l++) SWAP(a->mat[irow][l],a->mat[icol][l])
			for (l=0;l<m;l++) SWAP(b->mat[irow][l],b->mat[icol][l])
		}
		indxr->mat[0][i]=irow;
		indxc->mat[0][i]=icol;
		if (a->mat[icol][icol] == 0.0){
			merror("GAUSSJ: Singular Matrix-2");
			return 1;
		}
		pivinv=1.0/a->mat[icol][icol];
		a->mat[icol][icol]=1.0;
		for (l=0;l<n;l++) a->mat[icol][l] *= pivinv;
		for (l=0;l<m;l++) b->mat[icol][l] *= pivinv;
		for (ll=0;ll<n;ll++)
			if (ll != icol) {
				dum=a->mat[ll][icol];
				a->mat[ll][icol]=0.0;
				for (l=0;l<n;l++) a->mat[ll][l] -= a->mat[icol][l]*dum;
				for (l=0;l<m;l++) b->mat[ll][l] -= b->mat[icol][l]*dum;
			}
	}
	for (l=(n-1);l>=0;l--) {
		if (indxr->mat[0][l] != indxc->mat[0][l])
			for (k=0;k<n;k++)
				SWAP(a->mat[k][(int)(indxr->mat[0][l])],a->mat[k][(int)(indxc->mat[0][l])]);
	}
	//mfree(ipiv);
	//mfree(indxr);
	//mfree(indxc);
	return 0;
}

#undef SWAP



