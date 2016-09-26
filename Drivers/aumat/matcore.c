 /** \file matcore.c
 * \brief This C language library provides some simple matrix operations. 
 *  
 * All matrices are represented by a structure with the dimensions 
 * of the matrix and a pointer to an array of pointers to each row 
 * in the matrix. The functions using stdio can be excluded by        
 * defining the symbol NO_IO to the compiler.                
 * 
 * References   : Brugervejledning til C/Matlab matrix bibliotek,     
 *                Appendix F i Noergaard og Torp (1993).              
 *
 * \author O. Ravn
 * \author A. Pjetursson
 * \author P.M. Noergaard
 * \author S. Torp
 * 
 * \date 26/01-1998
 * \version 1.2
 */ 
 
 /***********************************************************************
 * $Source: /home/lvmogensen/AUTcvs/repository/aumat/matcore.c,v $
 * $Log: not supported by cvs2svn $
 * Revision 1.2  1998/01/26 10:21:33  or
 * Libraries updated and demomat program added
 *
 * Revision 1.1  1998/01/23 12:50:26  or
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
#include <string.h>
#include <math.h>

#include "iau_mat.h"
#include "log.h"


#if ERR_MATLAB == ON
  #include "/room/opt/matlab7/extern/include/mex.h"
#endif

#define TRUE 1     ///< Definition of a basic logical value
#define FALSE 0    ///< Definition of a basic logical value
#define FOUND 1    ///< Definition of a basic logical value
#define NOTFOUND 0 ///< Definition of a basic logical value

/** \fn matrix *mmake( int rows, int cols )
 * This function allocates memory for a matrix of the specified size
 * and assigns the specified dimensions to the allocated matrix.
 * \param[in] rows number of rows.
 * \param[in] cols number of columns.
 * \return *ptm pointer to the new matrix structure.
 */
matrix *mmake( int rows, int cols )
{
        matrix *ptm;
        float **row_pointers;
        register int i;

#if RUNCHK == ON

        /* Check that no dimension is zero. */
        if ((rows==0)||(cols==0)) merror("Invalid dimension error in mmake!");
#endif

        /* Memory allocation for matrix structure. */
        ptm = (matrix*)malloc(sizeof(matrix));

        /* Memory for array of pointers to rows. */;
        row_pointers = (float**)malloc(rows*sizeof(float*));

        /* Memory for all rows, initialize row pointers. */
        row_pointers[0] = (float*)malloc(rows*cols*sizeof(float));
        for (i=1;i<rows;i++)
                row_pointers[i] = row_pointers[i-1] + cols;

#if RUNCHK == ON

        /* Check if last allocation was ok ! */
        if (!row_pointers[0])
                merror("Memory allocation error in mmake!");
#endif

        ptm->row = rows;             /* Initialize matrix structure */
        ptm->col = cols;
        ptm->mat = row_pointers;     /* Pointer to row pointers     */

        return ptm;           /* Return pointer to matrix structure */
}

/** \fn void mfree( matrix *ptm )
 * This function deallocates the memory that was used for a matrix.
 * 
 * \param[in] *ptm Pointer to the matrix to be deallocated.
 */
void mfree( matrix *ptm )
{
        /* Deallocate rows */
        free(ptm->mat[0]);

        /* Deallocate row pointer array */
        free(ptm->mat);

        /* Deallocate matrix structure */
        free(ptm);
}

/** \fn void merror( char error_text[] )
 * This function displays the specified error message and exits to server
 * operating system.
 *
 * \param[in] error_text[] Text string with the error message.
 */
void merror( char error_text[] )
{

#if IO == ON     /* For compilation by the DSP ANSI-C compiler. */

        MPL_LOGE("--- Runtime error in matrix library:\r\n");
        MPL_LOGE("--- ");
        MPL_LOGE("%s\r\n",error_text);
      #if ERR_MATLAB == ON
        mexErrMsgTxt(error_text);
      #endif
#else
        error_text=error_text; /* Dummy command to avoid warnings on DSP */ 
#endif
        //exit(1);
}

#if PRINTF_FLOAT != ON
void printFloat(float v, int decimalDigits)
{
    int i = 10;
    double intP;
    int fractPart;
    double fractP = modf(v, &intP); 
    int intPart = (int)intP;
    if(v < 0.0){
        MPL_LOGE("          -0.", intPart);
    }else{
        MPL_LOGE("%12d.", intPart);
    }

    for (;decimalDigits!=0; i*=10, decimalDigits--){
        fractPart = (int)fmod(floor(abs(fractP * i)),10);
        if(fractPart < 0) fractPart *= -1;
        MPL_LOGE("%01d", fractPart);
    }
    MPL_LOGE("  ");
}
#endif

/** \fn void mprint( matrix *ptm )
 * This is a simple function to print a matrix on the screen.
 * \param[in] *ptm Pointer to the matrix that will be printed.
 */
void mprint( const matrix *ptm )
{
#if IO == ON     /* For compilation by the DSP ANSI-C compiler. */

        int i,j;
        for ( i=0; i < ptm->row; i++ ){
            MPL_LOGE("[  ");
            for( j=0; j < ptm->col; j++ ){
#if PRINTF_FLOAT == ON
                MPL_LOGE("%8.8f  ",ptm->mat[i][j]);
#else
                printFloat(ptm->mat[i][j], 8);
#endif
            }
            MPL_LOGE("]\r\n");
        }
        MPL_LOGE("\n\n");
#else
        ptm=ptm;
        exit(0); /* printf is not available in DSP compiler. */
#endif
}




/** \fn matrix *minput1( void )
 * This function reads the dimensions and content of a matrix from 
 * the keyboard.
 * \return matrix* Pointer to new matrix.
 */
matrix *minput1( void ) 
{
    #if 0
//#if IO == ON     /* For compilation by the DSP ANSI-C compiler. */

        matrix *ptm;
        int i,j,mrows,ncols, n;

        printf("%s","Number of rows    :");
        n = scanf("%d",&mrows);
        printf("%s","Number of columns :");
        n = scanf("%d",&ncols);

        ptm = mmake(mrows,ncols);

        for ( i=0; i<mrows; i++ )
        {
                for ( j=0; j<ncols; j++ )
                {
                        printf("%s %d %s %d %s","Enter element (",i,",",j,")  :");
                        n = scanf("%lf",&(ptm->mat[i][j]));
                        /* PS. scanf does not work with floating formats in Turbo C. */
                }
        }
        return ptm;
#else
        exit(0); /* scanf is not available in DSP compiler. */
#endif
}

/** \fn void minput2( matrix *ptm )
 * This function prompts for the content of a matrix of known size.
 * \param[out] *ptm Pointer to matrix to be read from keyboard.
 */
void minput2( matrix *ptm )
{

#if 0
//#if IO == ON     /* For compilation by the DSP ANSI-C compiler. */

        int i,j,n;

        printf("%s %d %s %d %s\n","This matrix has",ptm->row,"rows and",
                        ptm->col,"columns.");
        printf("%s %d %s\n","Please enter the following",
                        ptm->row*ptm->col,"elements.");

        for ( i=0; i < ptm->row; i++ )
        {
                for ( j=0; j < ptm->col; j++ )
                {
                        printf("%s %d %s %d %s","Enter element (",i,",",j,")  :");
                        n = scanf("%lf",&(*ptm).mat[i][j]);
                        /* PS. scanf does not work with floating formats in Turbo C. */
                }
        }
#else
        ptm=ptm;
        exit(0); /* scanf is not available in DSP compiler. */
#endif
}

/** \fn void mtrans( matrix *ptm1, matrix *ptm2 )
 * This function computes the transposed of a matrix.
 * \param[out] *ptm1 Pointer to result matrix.
 * \param[in]  *ptm2 Pointer to argument matrix.
 * \attention The two matrices may NOT be the same.
 */
void mtrans( matrix *ptm1, const matrix *ptm2 )
{
        register int i,j;

#if RUNCHK == ON

        if (ptm1==ptm2) 
                merror("Memory conflict error in mtrans!");
#endif

        for ( i=0; i < ptm2->col; i++ )          /* Transpose matrix */
        {
                for ( j=0; j < ptm2->row; j++ )
                        ptm1->mat[i][j] = ptm2->mat[j][i];
        }
}

/** \fn void minit( matrix *ptm )
 * This function initializes a matrix to all zeros.
 * \param *ptm Pointer to the matrix that will be initialized.
 */
void minit( matrix *ptm )
{
        register unsigned char zero_byte = 0;
        register int bytes;
        
        /* Number of bytes in matrix data area. */
        bytes = sizeof(float)*(ptm->row)*(ptm->col);

        /* Set data area to all zeroes. */
        memset( ptm->mat[0], zero_byte, bytes );
}

/** \fn void minitx( matrix *ptm, float x )
 * This function initializes a matrix to all x's. x being a scalar.
 * \param[out] ptm Pointer to the matrix that will be initialized.
 * \param[in] x Element to be put into the matrix  
 */
void minitx( matrix *ptm, float x )
{
        register int i,j;
        for(i=0; i < ptm->row; i++)
        {
          for(j=0; j < ptm->col; j++)
          {     
            ptm->mat[i][j]=x;
          }
        }
}

/** \fn void mrand( matrix *ptm )
 * This function initializes a matrix with random numbers between -1 and 1.
 * \param[out] *ptm Pointer to the matrix that will be initialized.
 */
void mrand( matrix *ptm )
{
        register int i,j;
        for(i=0; i < ptm->row; i++){
          for(j=0; j < ptm->col; j++)
                      ptm->mat[i][j]=rand()*2.0/RAND_MAX - 1.0;
        }
}

/** \fn void mdiag( matrix *ptm, float diag )
 * This function initializes a matrix to all zeros except for the
 * main diagonal elements, which will be assigned the specified value.
 * \param[out] *ptm Pointer to matrix that will be initialized.
 * \param[in]  diag Diagonal element value.
 */
void mdiag( matrix *ptm, float diag )
{
        register int i;

#if RUNCHK == ON

        if (ptm->row != ptm->col) \
        merror("matrix not quadratic error in mdiag!");

#endif

        minit(ptm);
        for ( i=0;i<ptm->row;i++ ) 
                ptm->mat[i][i] = diag;
}

/** \fn void munit( matrix *ptm )
 * This function initializes a matrix to the unit or identity matrix.
 * \param[out] *ptm Pointer to the matrix.
 */
void munit( matrix *ptm )
{
        register int i;

#if RUNCHK == ON

        if (ptm->row != ptm->col)
                merror("matrix not quadratic error in munit!");

#endif

        minit( ptm );
        for ( i=0;i<ptm->row;i++ ) 
                ptm->mat[i][i] = 1.0;
}


/** \fn void mset( matrix *ptm1, matrix *ptm2 )
 * This function copies the content of a matrix to another matrix.
 * ( A := B ) == mset(a,b); Assignment of matrix.
 * \param[out] *ptm1 Pointer to the return matrix
 * \param[in]  *ptm2 Pointer to the source matrix
 */
void mset( matrix *ptm1, const matrix *ptm2 )
{
        //register int bytes;

#if RUNCHK == ON

        if ( !( (ptm1->row == ptm2->row) && (ptm1->col == ptm2->col) ) )
                merror("Dimension mismatch error in mset!");
        // const
#endif

        //bytes = sizeof(float)*(ptm1->row)*(ptm1->col);
        //memcpy( ptm1->mat[0], ptm2->mat[0], bytes );

        int i, j;
        for(i = 0; i < ptm1->row; i++){
            for(j = 0; j < ptm1->col; j++){
                ptm1->mat[i][j] = ptm2->mat[i][j];
            }
        }
}


/** \fn void madd( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix addition ptm1 = ptm2 + ptm3
 * 
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 * \attention  *ptm1 can be equal to *ptm1 and/or *ptm2
 */
void madd( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 )
{
        register int i,j;

#if RUNCHK == ON

        if (!((ptm1->row == ptm2->row) && (ptm2->row == ptm3->row))){
                merror("Dimension mismatch error in madd!");
            }
        if (!((ptm1->col == ptm2->col) && (ptm2->col == ptm3->col))){
                merror("Dimension mismatch error in madd!");
            }

#endif

        /* Add the two matrices element by element */
        for ( i=0;i<ptm2->row;i++ )
        {
                for ( j=0; j < ptm2->col; j++ )
                        ptm1->mat[i][j] = ptm2->mat[i][j] + ptm3->mat[i][j];
        }
}

/** \fn void omadd( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix addition ptm1 = ptm2 + ptm3 \n
 * Pointer based version, which should improve speed.
 *
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 *
 * \note *ptm1 can be equal to *ptm1 and/or *ptm2
 * \attention  NOT TESTED
 * \todo Test the function for functionality and speed. 
 */
void omadd( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
{
        register float *a,*b,*c,*d;

#if RUNCHK == ON

        if (!((ptm1->row == ptm2->row) && (ptm2->row == ptm3->row)))
                //printf2("ptm1->row [%d] == ptm2->row [%d]\t[%s]\r\n", ptm1->row, ptm2->row, (ptm1->row == ptm2->row)? "T" : "F");
                //printf2("ptm2->row [%d] == ptm3->row [%d]\t[%s]\r\n", ptm2->row, ptm3->row, (ptm2->row == ptm3->row)? "T" : "F");
                merror("Dimension mismatch error in madd!");
        if (!((ptm1->col == ptm2->col) && (ptm2->col == ptm3->col)))
                merror("Dimension mismatch error in madd!");
#endif
        a = ptm1->mat[0];
        b = ptm2->mat[0];
        c = ptm3->mat[0];
        d = &(ptm1->mat[ptm1->row-1][ptm1->col-1]) + 1;
        
        /* Add the two matrices element by element */
        while ( a != d ) {
                *a++ = *b++ + *c++;
        }
}

/** \fn void msub( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix subtraction ptm1 = ptm2 - ptm3
 *
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 * \attention  *ptm1 can be equal to *ptm1 and/or *ptm2
 */
void msub( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 )
{
        register int i,j;

#if RUNCHK == ON

        if (!((ptm1->row == ptm2->row) && (ptm2->row == ptm3->row)))
                merror("Dimension mismatch error in msub!");
        if (!((ptm1->col == ptm2->col) && (ptm2->col == ptm3->col)))
                merror("Dimension mismatch error in msub!");

#endif

        /* Subtract the two matrices element by element */
        for ( i=0;i<ptm2->row;i++ )
        {
                for ( j=0; j < ptm2->col; j++ )
                        ptm1->mat[i][j] = ptm2->mat[i][j] - ptm3->mat[i][j];
        }
}

/** \fn void omsub( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix subtraction ptm1 = ptm2 - ptm3 \n
 * Pointer based version, which should improve speed.
 *
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 *
 * \note  *ptm1 can be equal to *ptm1 and/or *ptm2
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed. 
 */
void omsub( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
{
        register float *a,*b,*c,*d;

#if RUNCHK == ON

        if (!((ptm1->row == ptm2->row) && (ptm2->row == ptm3->row)))
                merror("Dimension mismatch error in madd!");
        if (!((ptm1->col == ptm2->col) && (ptm2->col == ptm3->col)))
                merror("Dimension mismatch error in madd!");
#endif
        a = ptm1->mat[0];
        b = ptm2->mat[0];
        c = ptm3->mat[0];
        d = &(ptm1->mat[ptm1->row-1][ptm1->col-1]) + 1;
        
        /* Subtract the two matrices element by element */
        while ( a != d ) {
                *a++ = *b++ - *c++;
        }
}

/** \fn void mmul( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix multiplication ptm1 = ptm2 * ptm3
 *
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 * \attention  *ptm1 can not be equal to *ptm2 
 */
void mmul( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 )
{
        register int i,j,k;

#if RUNCHK == ON

        if ((ptm1==ptm2) || (ptm1==ptm3)){
                //printf2("Memory conflict error in mmul!");
                merror("Memory conflict error in mmul!");
            }

        if ( !( (ptm2->col == ptm3->row) && \
        (ptm2->row == ptm1->row) && (ptm3->col == ptm1->col) ) ){
                //printf2("Dimension mismatch error in mmul!\r\n");
                //printf2("ptm2->col [%d] == ptm3->row [%d]\t[%s]\r\n", ptm2->col, ptm3->row, (ptm2->col == ptm3->row)? "T" : "F");
                //printf2("ptm2->row [%d] == ptm1->row [%d]\t[%s]\r\n", ptm2->row, ptm1->row, (ptm2->row == ptm1->row)? "T" : "F");
                //printf2("ptm3->col [%d] == ptm1->col [%d]\t[%s]\r\n", ptm3->col, ptm1->col, (ptm3->col == ptm1->col)? "T" : "F");
                merror("Dimension mismatch error in mmul!");
        }

#endif

        for ( i=0; i < ptm2->row; i++ )
        {
                for ( j=0; j < ptm3->col; j++ )
                {
                        ptm1->mat[i][j] = 0.0;
                        for ( k=0; k < ptm2->col; k++ )
                                ptm1->mat[i][j] += ptm2->mat[i][k] * ptm3->mat[k][j];
                }
        }
}

/** \fn void ommul( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Matrix multiplication ptm1 = ptm2 * ptm3 \n
 * Pointer based version, which should improve speed.
 *
 * \param[out] *ptm1 Pointer to result matrix
 * \param[in]  *ptm2 Pointer to first argument matrix
 * \param[in]  *ptm3 Pointer to second argument matrix
 *
 * \note  *ptm1 can not be equal to *ptm2 
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed. 
 */
void ommul( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
{
        register int i,j,coloff;
        register float *a, *b, *c, prod;

#if RUNCHK == ON

        if ((ptm1==ptm2) || (ptm1==ptm3))
                merror("Memory conflict error in mmul!");

        if ( !( (ptm2->col == ptm3->row) && \
        (ptm2->row == ptm1->row) && (ptm3->col == ptm1->col) ) )
                merror("Dimension mismatch error in mmul!");

#endif
        coloff = ptm3->row;
        
        for ( i=0; i < ptm2->row; i++ )
        {
                for ( j=0; j < ptm3->col; j++ )
                {
                        a = ptm2->mat[i];
                        b = ptm3->mat[0];
                        c = a + coloff;
                        prod = 0.0;
                        while ( a != c )
                                prod += *a++ * *b;
                                b += coloff;                    
                }
        }
}

/** \fn void smul( matrix *ptm1, matrix *ptm2, float factor)
 * Multiply matrix with scalar ptm1 = factor*ptm2
 *
 * \param[out] *ptm1 Pointer to result matrix.
 * \param[in]  *ptm2 Pointer to argument matrix.
 * \param[in]  factor Scalar factor to be multiplied with argument matrix.
 */
void smul( matrix *ptm1, const matrix *ptm2, float factor)
{
        register int i,j;

#if RUNCHK == ON

        if (!((ptm1->row==ptm2->row) && (ptm1->col==ptm2->col)))
                merror("Dimension mismatch error in smul!");

#endif

        for( i=0; i < ptm2->row; i++ )
        {
                for( j=0; j < ptm2->col; j++ ) ptm1->mat[i][j] = factor*ptm2->mat[i][j];
        }
}

/** \fn void osmul( matrix *ptm1, matrix *ptm2, float factor)
 * Multiply matrix with scalar ptm1 = factor*ptm2 \n
 * Pointer based version, which should improve speed.
 *
 * \param[out] *ptm1 Pointer to result matrix.
 * \param[in]  *ptm2 Pointer to argument matrix.
 * \param[in]  factor Scalar factor to be multiplied with argument matrix.
 *
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed. 
 */
void osmul( matrix *ptm1, matrix *ptm2, float factor)
{
        register float *a, *b, *c;

#if RUNCHK == ON

        if (!((ptm1->row==ptm2->row) && (ptm1->col==ptm2->col)))
                merror("Dimension mismatch error in smul!");

#endif

        a = ptm1->mat[0];
        b = ptm2->mat[0];
        c = &(ptm1->mat[ptm1->row-1][ptm1->col-1]) + 1;

        while ( a != c ) {
                *a++ = factor * *b++;
    }
}

/** \fn void mmuladd( matrix *ptv1, matrix *ptv2, matrix *ptm, matrix *ptv3, char pm[] )
 * Matrix-vector multiplication + vector:\n \n
 * 
 * vec1 = +vec2 + mat*vec3 \n
 * vec1 = +vec2 - mat*vec3 \n
 * vec1 = -vec2 + mat*vec3 \n
 * vec1 = -vec2 - mat*vec3 \n
 *
 * \param[out] *ptv1 Pointer to result vector
 * \param[in]  *ptv2 Pointer to first argument vector
 * \param[in]  *ptm  Pointer to argument matrix
 * \param[in]  *ptv3 Pointer to second argument vector
 * \param[in]  pm Signs ("++", "+-", "-+", "--") 
 * \attention  *ptv1 can not be equal to *ptv3
 */
void mmuladd( matrix *ptv1, const matrix *ptv2, const matrix *ptm, const matrix *ptv3, char pm[] )
{
        register int i,j;
// 	register int k;
//     register float tmp, *a, *b, *c, *d;

#if RUNCHK == ON

        if (ptv1==ptv3) merror("Memory conflict error in mmuladd!");

        if ( !( (ptm->col == ptv3->row) && (ptv1->row == ptv2->row) && \
        (ptv1->row == ptm->row) && (ptv1->col == 1) &&\
        (ptv2->col == 1) && (ptv3->col == 1)) ) \
        merror("Dimension mismatch error in mmuladd!");

#endif
        if (pm[0]=='+' && pm[1]=='+')
        {
          for ( i=0; i < ptv1->row; i++ )
          {
                ptv1->mat[i][0] = ptv2->mat[i][0];
                for ( j=0; j < ptm->col; j++ )
                {
                        ptv1->mat[i][0] += ptm->mat[i][j] * ptv3->mat[j][0];
                }
/*      d = ptv1->mat[i];
                *d = ptv2->mat[i][0];
                a = ptm->mat[i];
                b = ptv3->mat[0];
                c = &ptv3->mat[ptv3->row-1][ptv3->col-1] + 1;
                while ( a != c ) {
                        *d += *a++ * *b++;
                }
*/
/*      tmp = ptv2->mat[i][0];
                a = ptm->mat[i];
                b = ptv3->mat[0];
                c = &ptv3->mat[ptv3->row-1][ptv3->col-1] + 1;
                while ( a != c ) {
                        tmp += *a++ * *b++;
                }
                ptv1->mat[i][0] = tmp;
*/
          }
        }
        else if (pm[0]=='+' && pm[1]=='-')
        {
          for ( i=0; i < ptv1->row; i++ )
          {
                ptv1->mat[i][0] = ptv2->mat[i][0];
                for ( j=0; j < ptm->col; j++ )
                {
                        ptv1->mat[i][0] -= ptm->mat[i][j] * ptv3->mat[j][0];
                }
/*      tmp = ptv2->mat[i][0];
                a = ptm->mat[i];
                b = ptv3->mat[0];
                c = &ptv3->mat[ptv3->row-1][ptv3->col-1] + 1;
                while ( a != c ) {
                        tmp -= *a++ * *b++;
                }
                ptv1->mat[i][0] = tmp;
*/
          }
        }
        else if (pm[0]=='-' && pm[1]=='+')
        {
          for ( i=0; i < ptv1->row; i++ )
          {
                ptv1->mat[i][0] = -ptv2->mat[i][0];
                for ( j=0; j < ptm->col; j++ )
                {
                        ptv1->mat[i][0] += ptm->mat[i][j] * ptv3->mat[j][0];
                }
/*      tmp = -ptv2->mat[i][0];
                a = ptm->mat[i];
                b = ptv3->mat[0];
                c = &ptv3->mat[ptv3->row-1][ptv3->col-1] + 1;
                while ( a != c ) {
                        tmp += *a++ * *b++;
                }
                ptv1->mat[i][0] = tmp;
*/
          }
        }
        else if (pm[0]=='-' && pm[1]=='-')
        {
          for ( i=0; i < ptv1->row; i++ )
          {
                ptv1->mat[i][0] = -ptv2->mat[i][0];
                for ( j=0; j < ptm->col; j++ )
                {
                        ptv1->mat[i][0] -= ptm->mat[i][j] * ptv3->mat[j][0];
                }
/*      tmp = - ptv2->mat[i][0];
                a = ptm->mat[i];
                b = ptv3->mat[0];
                c = &ptv3->mat[ptv3->row-1][ptv3->col-1] + 1;
                while ( a != c ) {
                        tmp -= *a++ * *b++;
                }
                ptv1->mat[i][0] = tmp;
*/
          }
        }
        else
          merror("Sign error in mmuladd!");
}



/** \fn float trace( matrix* ptm )
 * Calculates the trace (the sum of the main diagonal elements) of a matrix.
 * \param[in] *ptm Pointer to the matrix.
 * \return Trace of the matrix.
 */
float trace( matrix* ptm )
{
        register float ltrace = 0.0;
        register int i;

#if RUNCHK == ON

        /* Check whether matrix is square */
        if (ptm->row != ptm->col)
                merror("Matrix not square error in trace!");

#endif

        /* Calculate sum of diagonal elements */
        for ( i=0;i<ptm->row;i++)
                ltrace += ptm->mat[i][i];

        /* Return with trace */
        return ltrace;
}


/** \fn float sprod( matrix* ptv1, matrix* ptv2 )
 * This function calculates the scalar-product of two vectors of equal
 * length. The vectors may be either row- or column- vectors or any 
 * combination of those. (The usual mmul routine can be used to calculate
 * scalar-products, but it requires a row- and a column-vector.)
 * \param[in] *ptv1 Pointer to first factor vector.
 * \param[in] *ptv2 Pointer to second factor vector.
 * \return    Scalar product of the two vectors.
 */
float sprod( const matrix* ptv1, const matrix* ptv2 )
{
        register float prod = 0.0;
        register int i, elements;

#if RUNCHK == ON

        /* Check whether the arguments are vectors.   */
        if (!(((ptv1->row==1)||(ptv1->col==1))&&((ptv2->row==1)||(ptv2->col==1))))
                merror("One of the inputs is not a vector error in sprod!");

        /* Check whether vectors has the same length. */
        if ((ptv1->row + ptv1->col) != (ptv2->row + ptv2->col))
                merror("Dimension mismatch error in sprod!");

#endif
        /* Calculate scalar product of the vectors.   */

        elements = ptv1->row + ptv1->col - 1;

        for ( i=0; i<elements; i++ )
                prod += (vget(ptv1,i))*(vget(ptv2,i));

        return prod;
}

/** \fn float osprod( matrix* ptv1, matrix* ptv2 )
 * This function calculates the scalar-product of two vectors of equal
 * length. The vectors may be either row- or column- vectors or any 
 * combination of those. (The usual mmul routine can be used to calculate
 * scalar-products, but it requires a row- and a column-vector.) \n
 * Pointer based version, which should improve speed.
 *
 * \param[in] *ptv1 Pointer to first factor vector.
 * \param[in] *ptv2 Pointer to second factor vector.
 * \return    Scalar product of the two vectors.
 *
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed. 
 */
float osprod( matrix* ptv1, matrix* ptv2 )
{
        register float prod = 0.0, *a, *b, *c;

#if RUNCHK == ON

        /* Check whether the arguments are vectors.   */
        if (!(((ptv1->row==1)||(ptv1->col==1))&&((ptv2->row==1)||(ptv2->col==1))))
                merror("One of the inputs is not a vector error in sprod!");

        /* Check whether vectors has the same length. */
        if ((ptv1->row + ptv1->col) != (ptv2->row + ptv2->col))
                merror("Dimension mismatch error in sprod!");

#endif
        /* Calculate scalar product of the vectors.   */

        a = ptv1->mat[0];
        b = ptv2->mat[0];
        c = &(ptv1->mat[ptv1->row-1][ptv1->col-1]) + 1;

        while ( a != c )
                prod += *a++ * *b++;

        return prod;
}

/** \fn float sprod2( matrix* ptv1, matrix* ptv2, int beg1, int end1, int beg2, int end2 )
 * This function calculates the scalar-product of two vectors of equal
 * length. The vectors may be either row- or column- vectors or any 
 * combination of those. (The usual mmul routine can be used to calculate
 * scalar-products, but it requires a row- and a column-vector.) In this
 * function, a starting and ending index can be specified for each vector.
 * In which case, only the specified part of the vectors will be used for
 * calculating the scalar-product.
 *
 * \param[in] *ptv1 Pointer to first factor vector.
 * \param[in] *ptv2 Pointer to second factor vector.
 * \param[in] beg1 Beginning index of vector 1.
 * \param[in] beg2 Beginning index of vector 2.
 * \param[in] end1 Ending index of vectors 1. 
 * \param[in] end2 Ending index of vectors 2.
 * \return    Scalar product of the two vectors.
 */
float sprod2( const matrix* ptv1, const matrix* ptv2, int beg1, int end1, int beg2, int end2 )
{
        register float prod = 0.0;
        register int i, elements;

#if RUNCHK == ON

        /* Check whether the arguments are vectors.   */
        if (!(((ptv1->row==1)||(ptv1->col==1))&&((ptv2->row==1)||(ptv2->col==1))))
                merror("One of the inputs is not a vector error in sprod!");

        /* Check whether vectors has the same length. */
        if ((end1 - beg1) != (end2 - beg2))
                merror("Dimension mismatch error in sprod!");

#else
        end2 = end2;  /* Avoid a warning. */
        
#endif
        /* Calculate scalar product of the vectors.   */

        elements = end1 - beg1 + 1;

        for ( i=0; i<elements; i++ )
                prod += (vget(ptv1,i+beg1))*(vget(ptv2,i+beg2));

        return prod;
}

/** \fn float osprod2( matrix* ptv1, matrix* ptv2, int beg1, int end1, int beg2, int end2 )
 * This function calculates the scalar-product of two vectors of equal
 * length. The vectors may be either row- or column- vectors or any 
 * combination of those. (The usual mmul routine can be used to calculate
 * scalar-products, but it requires a row- and a column-vector.) In this
 * function, a starting and ending index can be specified for each vector.
 * In which case, only the specified part of the vectors will be used for
 * calculating the scalar-product. \n
 * Pointer based version, which should improve speed.
 *
 * \param[in] *ptv1 Pointer to first factor vector.
 * \param[in] *ptv2 Pointer to second factor vector.
 * \param[in] beg1 Beginning index of vector 1.
 * \param[in] beg2 Beginning index of vector 2.
 * \param[in] end1 Ending index of vectors 1. 
 * \param[in] end2 Ending index of vectors 2.
 * \return    Scalar product of the two vectors.
 *
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed.
 */
float osprod2( matrix* ptv1, matrix* ptv2, int beg1, int end1, int beg2, int end2 )
{
        register float prod = 0.0, *a, *b, *c;

#if RUNCHK == ON

        /* Check whether the arguments are vectors.   */
        if (!(((ptv1->row==1)||(ptv1->col==1))&&((ptv2->row==1)||(ptv2->col==1))))
                merror("One of the inputs is not a vector error in sprod!");

        /* Check whether vectors has the same length. */
        if ((end1 - beg1) != (end2 - beg2))
                merror("Dimension mismatch error in sprod!");

#endif
        /* Calculate scalar product of the vectors.   */

        a = ptv1->mat[beg1];
        b = ptv2->mat[beg2];
        c = ptv1->mat[end1] + 1;

        while ( a != c )
                prod += *a++ * *b++;

        return prod;
}

/** \fn void mput( matrix *ptm, int row_pos, int col_pos, float value )
 * This function assigns a specified value to a specified element in
 * a matrix.
 * \param     *ptm Pointer to the matrix.
 * \param[in] value Value to assign in matrix.
 * \param[in] row_pos The row in which the element is to be assigned.
 * \param[in] col_pos The column in which the element is to be assigned.
 */
void mput( matrix *ptm, int row_pos, int col_pos, float value )
{

#if RUNCHK == ON

        /* Check if indices are inside matrix bounds. */
        if ( !((row_pos >= 0) && (row_pos < ptm->row)) )
                merror("Index out of range error in mput!");
        if ( !((col_pos >= 0) && (col_pos < ptm->col)) )
                merror("Index out of range error in mput!");

#endif

        /* Assign the value to the element */
        ptm->mat[row_pos][col_pos] = value;
}

        
/** \fn void vput( matrix *ptv, int pos, float value )
 * This function assigns a specified value to a specified element in
 * a vector.
 * \param *ptv Pointer to the vector.
 * \param value Value to assign in matrix.
 * \param pos The position in which the element is to be assigned.
 */
void vput( matrix *ptv, int pos, float value )
{

#if RUNCHK == ON

        /* Check if index is inside vector bounds. */
        if ( !((pos>=0) && (pos <= ptv->row + ptv->col - 2)) )
                merror("Index out of range error in vput!");

#endif

        /* Assign the value to the element */
        if (ptv->row == 1)
                ptv->mat[0][pos] = value; /* Row vector */

        else if (ptv->col == 1)
                ptv->mat[pos][0] = value; /* Column vector */

        else 
                merror("Input is not a vector error in vput!");

}



/** \fn float mget( matrix *ptm, int row_pos, int col_pos )
 * This function returns the value of a specified element in a matrix.
 * \param[in] *ptm Pointer to the matrix.
 * \param[in] row_pos Row of element.
 * \param[in] col_pos Column of element.
 * \return    Value of specified element.
 */
float mget( const matrix *ptm, int row_pos, int col_pos )
{

#if RUNCHK == ON

        /* Check whether indices is inside matrix bounds. */
        if ( !((row_pos >= 0) && (row_pos < ptm->row)) )
                merror("Index out of range error in mget!");
        if ( !((col_pos >= 0) && (col_pos < ptm->col)) )
                merror("Index out of range error in mget!");

#endif

        /* Get address of specified element */
        return (ptm->mat[row_pos][col_pos]);
}


/** \fn float vget( matrix* ptv, int pos )
 * Gets an element of either a column- or row-vector.
 * \param[in] *ptv Pointer to the vector.
 * \param[in] pos Position of element.
 * \return    Value of the specified element.
 */
float vget( const matrix* ptv, int pos )
{
        register float *pte = NULL;

#if RUNCHK == ON

        /* Check if index is inside matrix bounds. */
        if ( pos > ptv->row + ptv->col - 2 )
                merror("Index out of range error in vget!");

#endif

        if ( ptv->row == 1 )
                pte = &(ptv->mat[0][pos]);

        else if ( ptv->col == 1 )
                pte = &(ptv->mat[pos][0]);

        else 
                merror("Input is not a vector in vget!");

        return *pte;
}


/** \fn void addcols(matrix *ptm1, matrix *ptm2, matrix *ptm3)
 * Create a new matrix, by putting two matrices next to one another
   \verbatim
           [    :    ]
      M1 = [ M2 : M3 ]
           [    :    ]
   \endverbatim
 * \attention The number of rows in M1, M2 and M3 must be the same, and the number 
 *      of columns in M1 must be equal to the sum of rows in M2 and M3.
 *
 * \param[out] *ptm1 Pointer to result matrix (can be equal to *ptm2)
 * \param[in]  *ptm2 Pointer to the matrix to be placed left
 * \param[in]  *ptm3 Pointer to the matrix to be placed right
 */
void addcols(matrix *ptm1, const matrix *ptm2, const matrix *ptm3)
{
        register int i, j;

#if RUNCHK == ON

        /* Check whether M1, M2 and M3 have the same number of rows        */
        if ( !( (ptm2->row == ptm3->row) && (ptm2->row == ptm1->row) ) )
                merror("Dimension mismatch in addcols!");
        
        /* Check that there aren't too many columns in M1                  */
        if ( ptm1->col != (ptm2->col + ptm3->col) )
                merror("Number of columns out of range in addcols!");

#endif

        /* Copy M2 to the left side of M1 */
        for( j = 0; j < ptm2->col; j++ )
        {
                for( i = 0; i < ptm1->row; i++ ) 
                        ptm1->mat[i][j] = ptm2->mat[i][j];
        }

        /* Copy M3 to the right side of M1 */
        for( j=0;j<ptm3->col;j++)
        {
                for( i = 0; i < ptm1->row; i++ )
                        ptm1->mat[i][(j+ptm2->col)] = ptm3->mat[i][j];
        }

}

/** \fn void addrows( matrix *ptm1, matrix *ptm2, matrix *ptm3 )
 * Create a new matrix, by putting two matrices on top of one another
 \verbatim
                [ M2 ]
           M1 = [----]
                [ M3 ]
 \endverbatim
 * \attention The number of columns in M2 and M3 must be the same, and the size of 
 *      M1 must fit M2 and M3.
 *
 * \param[out] *ptm1 Pointer to result matrix 
 * \param[in]  *ptm2 Pointer to matrix to be placed in the top
 * \param[in]  *ptm3 Pointer to matrix to be placed in the bottom
 */
void addrows( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 )
{
        register int i, j;

#if RUNCHK == ON
       
        /* Check whether M1, M2 and M3 have the same number of columns     */
        if (!((ptm2->col == ptm3->col) && (ptm1->col == ptm2->col)))
                merror("Dimension mismatch in addrows!");

        /* Check whether M1 has the right dimensions to fit M2 & M3        */
        if (ptm1->row != (ptm2->row + ptm3->row))
                merror("Dimension mismatch in addrows!");

#endif

        /* Copy M2 to the top of M1 */
        for ( i=0;i<ptm2->row;i++ )
        {
                for ( j = 0; j < ptm1->col; j++ ) 
                        ptm1->mat[i][j] = ptm2->mat[i][j];
        }

        /* Copy M3 to the bottom of M1 */
        for ( i = 0; i < ptm3->row; i++ )
        {
                for ( j = 0; j < ptm1->col; j++ )
                        ptm1->mat[i+(ptm2->row)][j] = ptm3->mat[i][j];
        }
}


/** \fn void concat(matrix* ptv0,matrix* ptv1,matrix* ptv2,int beg1,int end1,int beg2,int end2)
 * Concatenates parts of two vectors (either rows or columns). The 
 * part of each vector is specified by the beginning and ending index
 * of the part, which is to concatenated with the other.
 *
 * \param[out] *ptv0 Pointer to resulting vector.
 * \param[in]  *ptv1 Pointer to first argument vector.
 * \param[in]  *ptv2 Pointer to second argument vector.
 * \param[in]  beg1 Beginning index of first vector.
 * \param[in]  end1 Ending index of first vector.
 * \param[in]  beg2 Beginning index of second vector.
 * \param[in]  end2 Ending index of second vector.
 */
void concat(matrix* ptv0,matrix* ptv1,matrix* ptv2,int beg1,int end1,int beg2,int end2)
{
        register int i, n1, n2;

        n1 = end1 - beg1 + 1;   /* Number of elements in first vector.  */
        n2 = end2 - beg2 + 1;   /* Number of elements in second vector. */

#if RUNCHK == ON

        /* Check that all arguments are vectors. */
        if (!(((ptv0->row==1)||(ptv0->col==1))&&((ptv1->row==1)||(ptv1->col==1))&&((ptv2->row==1)||(ptv2->col==1))))
                merror("Input is not a vector error in concat!");
        
        /* Check whether indices are inside bounds. */
        if ((end1 > (ptv1->row + ptv1->col - 2))||(end2 > (ptv2->row + ptv2->col - 2)))
                merror("Index is outside vector bounds error in concat!");

        /* Check the dimension matching. */
        if ((n1 + n2) != (ptv0->row + ptv0->col - 1))
                merror("Dimension mismatch error in concat!");
#endif

        /* Copy part of first vector to beginning of result vector. */
        for ( i = 0; i < n1; i++ )
                vput( ptv0, i, vget( ptv1, i+beg1 ) );

        /* Copy part of second vector to end of result vector. */
        for ( i = 0; i < n2; i++ )
                vput( ptv0, i+n1, vget( ptv2, i+beg2 ) );
}


/** \fn void mat2mat( matrix *ptm1, int row, int col, matrix *ptm2 )
 * Inserts a matrix (Matrix 2) into another matrix (Matrix 1).
 \verbatim
            [      ]
       M1 = [  [M2]]
            [      ]
 \endverbatim
 * \param[out] *ptm1   Pointer Matrix 1
 * \param[in]  row Row coordinates to the element where the
 *                upper left corner of matrix 2 is placed
 * \param[in]  col Column coordinates to the element where the
 *                upper left corner of matrix 2 is placed
 * \param[in]  *ptm2   Pointer to Matrix 2
 */
void mat2mat( matrix *ptm1, int row, int col, const matrix *ptm2 )
{
        register int i2,j2;
        int i1, j1, rows2, cols2;

        rows2 = ptm2->row;     /* Number of rows in Matrix 2    */
        cols2 = ptm2->col;     /* Number of columns in Matrix 2 */

#if RUNCHK == ON
        /* Check whether the insertion is possible. */
        if( row+rows2 > ptm1->row || col+cols2 > ptm1->col )
                merror("Dimension mismatch in function MAT2MAT");
#endif

        /* Insert Matrix 2 into Matrix 1 */
        
        for( i1=row,i2=0; i2<rows2; i1++,i2++ )
        {
                for( j1=col,j2=0; j2<cols2; j1++,j2++  )
                        ptm1->mat[i1][j1] = ptm2->mat[i2][j2];
        }
}

/** \fn void shift( matrix *ptm, float new_element )
 * Shifts all elements of a vector and inserts a new value 
 * in the first element. The last element is lost. The
 * function works on both column vectors and row vectors.
 *
 * \param[out] *ptm Pointer to the vector to be shifted
 * \param[in]  new_element New element to be inserted
 */
void shift( matrix *ptm, float new_element )
{
        register int i,j;

        if (ptm->col == 1)                                         /* Column vector        */
        {
                for(i=ptm->row-2,j=i+1; j>0; i--,j--)              /* Shift elements down  */
                        ptm->mat[j][0] = ptm->mat[i][0];
                ptm->mat[0][0] = new_element;                      /* Insert new element   */
        }
        else if (ptm->row == 1)                                    /* Row vector           */
        {
                for(i=ptm->col-2,j=i+1; j>0; i--,j--)              /* Shift elements right */
                        ptm->mat[0][j] = ptm->mat[0][i];
                ptm->mat[0][0] = new_element;                      /* Insert new element   */
        }
        else 
                merror("Dimension mismatch error in funcion shift!"); /* Not a vector         */      
}

/** \fn void oshift( matrix *ptm, float new_element )
 * Shifts all elements of a vector and inserts a new value 
 * in the first element. The last element is lost. The
 * function works on both column vectors and row vectors. \n
 * Pionter based version, which should improve speed.
 *
 * \param[out] *ptm Pointer to the vector to be shifted
 * \param[in]  new_element New element to be inserted
 *
 * \attention NOT TESTED
 * \todo Test the function for functionality and speed.
 */
void oshift( matrix *ptm, float new_element )
{
        register float *a,*b;

#if RUNCHK == ON
        if ((ptm->col != 1) && (ptm->col != 1))
                merror("Dimension mismatch error in funcion shift!");      
#endif
        a = &(ptm->mat[ptm->row-1][ptm->col-1]) - 1;
        b = ptm->mat[0];
        
        while ( a != b ) {
                *(a+1) = *a;
                a--;
        }
        *a = new_element;
}

/** \fn void submat( matrix *ptm1, matrix *ptm2, int rowbeg, int rowend, int colbeg, int colend )
 * This function picks a submatrix from a larger matrix.
 *
 * \param[out] *ptm1 Pointer to the result matrix 
 * \param[in]  *ptm2 Pointer to input matrix
 * \param[in]  rowbeg Number of the first row
 * \param[in]  rowend Number of the last row
 * \param[in]  colbeg Number of the first column
 * \param[in]  colend Number of the last column
 */
void submat( matrix *ptm1, const matrix *ptm2, int rowbeg, int rowend, int colbeg, int colend )
{
        register int i1,i2,j1,j2;

#if RUNCHK == ON

        if ( rowend > ptm2->row-1 || colend > ptm2->col-1 )
                merror("Argument out of range in function submat!");
        
        if (((rowend-rowbeg)!=(ptm1->row-1))||((colend-colbeg)!=(ptm1->col-1)))
                merror("Dimension mismatch error in function submat!");

#endif

        for ( i1=0,i2=rowbeg; i2<=rowend; i1++,i2++ )
        {
                for ( j1=0,j2=colbeg; j2<=colend; j1++,j2++ )
                        ptm1->mat[i1][j1] = ptm2->mat[i2][j2];
        }
}


/** \fn void subvec( matrix *ptm1, matrix *ptm2, int elem1, int elem2 )
 * Extract a subvector from a vector and store it in another vector. Works
 * on both row anb column vectors.
 *
 * \param[out] *ptm1 Pointer to the result vector
 * \param[in]  *ptm2 Pointer to input vector
 * \param[in]  elem1 Number of the first element
 * \param[in]  elem2 Number of the last element
 */
void subvec( matrix *ptm1, const matrix *ptm2, int elem1, int elem2 )
{
        register int i,j;

#if RUNCHK == ON

        /* Check whether elem2 is inside bound */
        if ( elem2 > ptm2->row + ptm2->col - 2 )
                merror("Argument out of range in function subvec!");

        /* Check whether sub-vector fits output-vector. */
        if ((ptm1->row + ptm1->col) != ( elem2 - elem1 + 2 ))
                merror("Wrong number of elements error in subvec!");


#endif

        if ( ptm2->col == 1 )                                 /* Column vector */
        {
                for ( i=0,j=elem1; j<=elem2; i++,j++ )
                        ptm1->mat[i][0] = ptm2->mat[j][0];
        }

        else if ( ptm2->row == 1 )                            /* Row vector    */
        {
                for ( i=0,j=elem1; j<=elem2; i++,j++ )
                        ptm1->mat[0][i] = ptm2->mat[0][j];
        }
        else 
                merror("Dimension mismatch in function subvec"); /* Not a vector  */
}

/** \fn int getrows( matrix *ptm )
 * This function gets the number of rows in a matrix.
 *
 * \param[in] *ptm - Pointer to matrix
 * \return Number of rows in matrix
 */
int getrows(const  matrix *ptm )
{
        return ptm->row;
}

/** \fn int getcols( matrix *ptm )
 * This function gets the number of columns in a matrix.
 *
 * \param[in] *ptm - Pointer to matrix
 * \return Number of columns in matrix
 *
 */
int getcols( const matrix *ptm )
{
        return ptm->col;
}

/** \fn int length( matrix* ptv )
 * Determines the length of a row- or column- vector.
 *
 * \param[in] *ptv Pointer to the vector.
 * \return  Length of the vector. (The number of elements.)
 */
int length( const matrix* ptv )
{
        register int llength;

#if RUNCHK == ON

        if ( !( (ptv->row==1) || (ptv->col==1) ) )
                merror("Input argument is not a vector error in length!");

#endif
        llength = ptv->row + ptv->col - 1;

        return llength;
}


/** \fn int mscmp( matrix *mat, char *string )
 * This function compares a string and a row vector containing a 
 * MATLAB string loaded by mload.
 * If the to strings are identical 1 is returned. Otherwise the 
 * function returns 0.
 *
 * \param[in] *mat Pointer to "string vector"
 * \param[in] *string String expression
 * \return Integer with comparation result (1 is equal, 0 is not equal)
 */
int mscmp( matrix *mat, char *string )
{
        int i, matlength;

        matlength=length(mat);
        for( i=0;(char)get_val(mat,0,i) == *(string+i) && i<matlength ; i++ )
                ;
        if( (i==matlength) && *(string+i)=='\0') 
                return 1;
        else 
                return 0;
}

/** \fn matrix *mfind( matrix *ptm, float elem )
 * This function finds a given element in a matrix and returns
 * the row and column coordinates to the locations. If no
 * instances of the element are found, a zero by zero matrix is
 * returned.
 * 
 * \param[in] *ptm Pointer to matrix.
 * \param[in] elem Element to be searched for (float)
 * \return NEW 2 column matrix. 1st column contains row indices,
 *                   2nd column contains the column indices.
 */
matrix *mfind( matrix *ptm, float elem )
{
        register int i,j,k;
        matrix *tmp, *retmat;
        k=0;
        tmp = mmake((ptm->row)*(ptm->col),2);

        /* Seach entire matrix */
        for(i=0; i < ptm->row; i++)
          {
            for(j=0; j < ptm->col; j++)
              {
                if(ptm->mat[i][j] == elem)
                  {
                    tmp->mat[k][0]=i;
                    tmp->mat[k][1]=j;
                    k++;
                  }
              }
          }
        if(k==0)   /* The element is not recognized */
        {
          retmat = mmake(1,1);
          retmat->row=0;
          retmat->col=0;
        }          /* Copy coordinates to a [ 2 | # of instances] matrix */
        else
          {
          retmat = mmake(k,2);
          for(i=0; i < k; i++)
          {
            retmat->mat[i][0] = tmp->mat[i][0];
            retmat->mat[i][1] = tmp->mat[i][1];
          }
        }
        mfree(tmp);
        return retmat;
}



/** \fn matrix *mnofind( matrix *ptm, float elem )
 * This function finds all entries in a matrix not equal to a given element
 * and returns the row and coloumn coordinates to the locations. If all entries
 * equal the element, a zero by zero matrix is returned.
 * 
 * \param[in] *ptm Pointer to matrix.
 * \param[in] elem Element to be searched for (float)
 * \return NEW 2 column matrix. 1st column contains row indices,
 *                   2nd column contains the column indices.
 */
matrix *mnofind( matrix *ptm, float elem )
{
        register int i,j,k;
        matrix *tmp, *retmat;
        k=0;
        tmp = mmake((ptm->row)*(ptm->col),2);

        /* Seach entire matrix */
        for(i=0; i < ptm->row; i++)
          {
            for(j=0; j < ptm->col; j++)
              {
                if(ptm->mat[i][j] != elem)
                  {
                    tmp->mat[k][0]=i;
                    tmp->mat[k][1]=j;
                    k++;
                  }
              }
          }
        if(k==0)   /* No entries different from the element */
        {
          retmat = mmake(1,1);
          retmat->row=0;
          retmat->col=0;
        }          /* Copy coordinates to a [ 2 | # of instances] matrix */
        else
          {
          retmat = mmake(k,2);
          for(i=0; i < k; i++)
          {
            retmat->mat[i][0] = tmp->mat[i][0];
            retmat->mat[i][1] = tmp->mat[i][1];
          }
        }
        mfree(tmp);
        return retmat;
}

/** \fn void reverse( matrix *ptv )
 * Reverse the order of the elements in a vector. Works for both row
 * and column vectors.
 *
 * \param *ptv Pointer vector to be reversed
 */
void reverse( matrix *ptv )
{
        register int i;
        float tmp;
        if (ptv->row == 1)
        {
                for (i=0; i<ptv->col/2; i++) 
                {
                        tmp = ptv->mat[0][i];
                        ptv->mat[0][i] = ptv->mat[0][ptv->col-1-i];
                        ptv->mat[0][ptv->col-1-i] = tmp;
                }
        }
        else if( ptv->col==1 )
        {
                for( i=0;i<ptv->row/2;i++ ) 
                {
                        tmp = ptv->mat[i][0];
                        ptv->mat[i][0] = ptv->mat[ptv->row-1-i][0];
                        ptv->mat[ptv->row-1-i][0] = tmp;
                }
        }
        else 
                merror("Dimension mismatch in function reverse");
}

/** \fn void setcols( matrix *ptm, int cols )
 * Set the number of columns in a matrix or vector.
 * \param[in] *ptm Pointer to the matrix.
 * \param[in] cols Number of columns wanted in vector.
 * \attention This operation should be used with care.
 */
void setcols( matrix *ptm, int cols )
{
        ptm->col = cols;
}

/** \fn void array2mat( matrix *ptm, float *values, int n )
 * Copy n values from a C array to a matrix.
 * \param[out] *ptm Pointer to the matrix.
 * \param[in]  *values Pointer to the vector holding the values.
 * \param[in]  n number of values to be copied.
 * \attention This operation should be used with care.
 */
void array2mat( matrix *ptm, float *values, int n )
{
        if ((ptm->row)*(ptm->col) < n)
                merror("Dimension mismatch in function array2mat");

        int i, j;
        for(i = 0; i < ptm->row; i++){
            for(j = 0; j < ptm->col; j++){
                ptm->mat[i][j] = values[i*ptm->col + j];
            }
        }
        //memcpy(ptm->mat[0],values,sizeof(float)*n);
}



///////////////////////// Functions added as extended part for this 3d localization project /////////////////////////////

/*
 *  ptm3 should be a vector, containing the diagonal elements of a diagonal matrix
 *
 */
void madddiag( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 )
{
        register int i,j;

#if RUNCHK == ON

        if (!((ptm1->row == ptm2->row) && (ptm2->row == ptm3->row))){
                merror("Dimension mismatch error in madd!");
            }
        if (!((ptm1->col == ptm2->col) && (ptm2->col == ptm3->row))){
                merror("Dimension mismatch error in madd!");
            }
#endif

        /* Add the two matrices element by element */
        for ( i=0;i<ptm2->row;i++ )
        {
                for ( j=0; j < ptm2->col; j++ )
                    if(i == j)
                        ptm1->mat[i][j] = ptm2->mat[i][j] + ptm3->mat[i][0];
                    else
                        ptm1->mat[i][j] = ptm2->mat[i][j];
        }
}


void mremoverow(matrix *ptm, int row){
    if(row == END)
        row = ptm->row;

    int i, j;
    if(row > ptm->row || row < 0){
        return;
    }else{
        for(i = row; i < ptm->row-1; i++){
            for(j = 0; j < ptm->col; j++){
                ptm->mat[i][j] = ptm->mat[i+1][j];
            }
        }
    }
    ptm->row = ptm->row-1;
}

void maddrow(matrix *ptm, int row, float *values){
    if(row == END)
        row = ptm->row;

    int i, j;
    if(row > ptm->row || row < 0){
        return;
    }else{
        for(j = 0; j < ptm->col; j++){
            for(i = ptm->row-1; i >= row; i--){
                ptm->mat[i+1][j] = ptm->mat[i][j];
            }
            if(values[0] == ZERO_VAL){
                ptm->mat[row][j] = 0.0f;
            }else{
                ptm->mat[row][j] = values[j];
            }
        }
    }
    ptm->row = ptm->row+1;
}

void mremovecol(matrix *ptm, int col){
    if(col == END)
        col = ptm->col;

    int i, j;
    if(col > ptm->col || col < 0){
        return;
    }else{
        for(i = col; i < ptm->col-1; i++){
            for(j = 0; j < ptm->row; j++){
                ptm->mat[j][i] = ptm->mat[j][i+1];
            }
        }
    }
    ptm->col = ptm->col-1;
}

void maddcol(matrix *ptm, int col, float *values){
    if(col == END)
        col = ptm->col;

    int i, j;
    if(col > ptm->col || col < 0){
        return;
    }else{
        for(j = 0; j < ptm->row; j++){
            for(i = ptm->col-1; i >= col; i--){
                ptm->mat[j][i+1] = ptm->mat[j][i];
            }
            if(values[0] == ZERO_VAL){
                ptm->mat[j][col] =  0.0f;
            }else{
                ptm->mat[j][col] = values[j];
            }
        }
    }
    ptm->col = ptm->col+1;
}

/*  Resizes matrixes to a given size!
 *  This function always adds extra rows and columns to the end of matrix.
 *  NB: Resizing to larger matrix by the use of this function inputs zeros as initial values!
 */
void mresize(matrix *ptm, int rows, int cols){

    if(ptm->row == rows && ptm->col == cols)
        return;
    
    int i, cnt;
    float z[] = {ZERO_VAL};
    
    if(rows > ptm->row){
        cnt = (rows - ptm->row);
        for(i = 0; i < cnt; i++){
            maddrow(ptm, END, z);            
        }
    }else{
        cnt = (ptm->row - rows);
        for(i = 0; i < cnt; i++){
            mremoverow(ptm, END);            
        }
    }

    if(cols > ptm->col){
        cnt = (cols - ptm->col);
        for(i = 0; i < cnt; i++){
            maddcol(ptm, END, z);            
        }
    }else{
        cnt = (ptm->col - cols);
        for(i = 0; i < cnt; i++){
            mremovecol(ptm, END);            
        }
    }
}   