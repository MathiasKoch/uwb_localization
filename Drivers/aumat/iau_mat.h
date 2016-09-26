/** \mainpage
 * This documentation is for the real time capable matrix library
 * developed and supported by the Department of Automation at
 * the Technical University of Denmark.
 *
 * The library is written in C and is supposed to be used for development
 * and test of control algorithms, running on real time computer systems. 
 * The library is not thoroughly tested and is a work in progress.
 * 
 * \section Installation
 * The makefile supplied with this distribution is equipped with an
 * install option that copies the library libiau_mat.a to 
 * /usr/local/lib and the header file iau_mat.h to
 * /usr/local/include.
 * 
 * \section Introduction
 * To get a feel for the syntax of the library, please refer to 
 * demomat.c and the function descriptions.
 *
 * \author Lars Valdemar Mogensen
 * \date 23/05-2006
 */

/** \file iau_mat.h
 * \brief Main header file for the IAU realtime matrix library.
 * 
 * \author O. Ravn
 * \author A. Pjetursson
 * \author P.M. Noergaard
 * \author S. Torp
 * 
 * \date 26/01-1998
 * \version 1.4
 */
 
 /*************************************************************************
 * $Source: /home/lvmogensen/AUTcvs/repository/aumat/iau_mat.h,v $
 * $Log: not supported by cvs2svn $
 * Revision 1.4  1998/01/26 10:21:23  or
 * Libraries updated and demomat program added
 *
 * Revision 1.2  1998/01/19 09:04:04  pmn
 * Integrated with PMN's version of matrix2.c.
 *
 * Revision 1.1.1.1  1997/08/12 13:58:49  or
 * Matrix Library for RealTime use
 *
 * Revision 1.3  1995/08/03  11:12:25  ap
 * Online data functions added
 *
 * Revision 1.2  1995/06/02  13:59:28  ap
 * Small updates
 *
 * Revision 1.1.1.1  1995/05/17  12:02:06  ap
 * This realtime system is initially written by P.M. Noergaard and S. Torp. This
 * new version and software structure are made by O. Ravn and A. Pjetursson.
 *
 ************************************************************************/
#if !defined(_IAU_MAT_H)
#define _IAU_MAT_H ///< Header include definition

# ifdef __cplusplus
  extern "C" {
# endif

#define ON 1  ///< Definition of a basic logical value
#define OFF 0 ///< Definition of a basic logical value

#define PRINTF_FLOAT       ON   ///< Defines if printf is capable of printing floats
#define RUNCHK             ON   ///< Defines if run-time check is ON/OFF
#define IO                 ON   ///< Defines if IO functions are switched ON/OFF
#define ERR_MATLAB         OFF  ///< Defines if MATLAB error handling is ON/OFF

#define VARIABLENAMESIZE   20   ///< Defines the variable name size

/* Inline functions with similar output as the library functions listed below. */
/* ( No run-time checks is performed, when inline functions are used )         */

/** Returns the number of rows, see getrows() \n ( No run-time checks is performed, when inline functions are used ) */
#define nof_rows(ptm)                      (ptm->row)
/** Returns the number of columns, see getcols() \n ( No run-time checks is performed, when inline functions are used ) */
#define nof_cols(ptm)                      (ptm->col)
/** Returns the length of a vector, see length() \n ( No run-time checks is performed, when inline functions are used ) */
#define vec_len(ptv)                       (ptv->row+ptv->col-1)
/** Returns the value from a matrix, see mget() \n ( No run-time checks is performed, when inline functions are used ) */
#define get_val(ptm,row_pos,col_pos)       (ptm->mat[row_pos][col_pos])
/** Set the value of a matrix element, see mput() \n ( No run-time checks is performed, when inline functions are used ) */
#define put_val(ptm,row_pos,col_pos,value) ((ptm->mat[row_pos][col_pos])=value)
/** Get the element from a row vector, see vget() \n ( No run-time checks is performed, when inline functions are used ) */
#define rvget(ptv,element)                 (ptv->mat[0][element])
/** Get the element from a column vector, see vget() \n ( No run-time checks is performed, when inline functions are used ) */
#define cvget(ptv,element)                 (ptv->mat[element][0])
/** Put element in row vector, see vput() \n ( No run-time checks is performed, when inline functions are used ) */
#define rvput(ptv,element,value)           ((ptv->mat[0][element])=value)
/** Put element in column vector, see vput() \n ( No run-time checks is performed, when inline functions are used ) */
#define cvput(ptv,element,value)           ((ptv->mat[element][0])=value)

/* Declaration of the "abstract" data-type. */

/** \struct matrix iau_mat.h
 * \brief Matrix structure for the IAU C library.
 * The members of the sruct are meant to be "private" and 
 * should only be accessed via the "member functions" below.
 * 
 * All matrices are represented by a structure with the dimensions 
 * of the matrix and a pointer to an array of pointers to each row 
 * in the matrix.
 */
typedef struct {
	int row;      ///< Number of rows in the matrix
	int col;      ///< Number of cloumns in the matrix
	float **mat; ///< Array of pointers to the cloumns in the matrix (data storage).
} matrix;

/** \struct varlisttype iau_mat.h
 * \brief Struct to hold list of variables used in calculations.
 */
typedef struct {
	char name[VARIABLENAMESIZE]; ///< Name of the matrix.
	matrix *m_addr;              ///< Pointer to the matrix struct.
	int idnumber;                ///< Number to identify the matrix.
	char status;                 ///< Variable status.
	void *next;                  ///< Pointer to the next element in the variable list.
} varlisttype;

/* Declaration of the "member functions".   */

// *******************************************
// Prototypes from matcore.c

matrix *mmake( int rows, int cols );
void mfree( matrix *ptm );
void merror( char error_text[] );
void mprint( const matrix *ptm );
matrix *minput1( void ); 
void minput2( matrix *ptm );
void mtrans( matrix *ptm1, const matrix *ptm2 );
void minit( matrix *ptm );
void minitx( matrix *ptm, float x );
void mrand( matrix *ptm );
void mdiag( matrix *ptm, float diag );
void munit( matrix *ptm );
void mset( matrix *ptm1, const matrix *ptm2 );
void madd( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );
void msub( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );
void mmul( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );
void smul( matrix *ptm1, const matrix *ptm2, float factor );
void mmuladd( matrix *ptv1, const matrix *ptv2, const matrix *ptm, const matrix *ptv3, char pm[] );
float trace( matrix* const ptm );
float sprod( const matrix* ptv1, const matrix* ptv2 );
float sprod2( const matrix* ptv1, const matrix* ptv2, int beg1, int end1, int beg2, int end2 );
void mput( matrix *ptm, int row_pos, int col_pos, float value );
void vput( matrix *ptv, int pos, float value );
float mget( const matrix *ptm, int row_pos, int col_pos );
float vget( const matrix* ptv, int pos );
void addcols( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );
void addrows( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );
void concat( matrix* ptv0,matrix* ptv1,matrix* ptv2,int beg1,int end1,int beg2,int end2 );
void mat2mat( matrix *ptm1, int row, int col, const matrix *ptm2 );
void shift( matrix *ptm, float new_element );
void submat( matrix *ptm1, const matrix *ptm2, int rowbeg, int rowend, int colbeg, int colend );
void subvec( matrix *ptm1, const matrix *ptm2, int elem1, int elem2);
int getrows( const matrix *ptm );
int getcols( const matrix *ptm );
int length( const matrix* ptv );
int mscmp( matrix *mat, char *string );
matrix *mfind( matrix *ptm, float elem );
matrix *mnofind( matrix *ptm, float elem );


void ommul( matrix *ptm1, matrix *ptm2, matrix *ptm3);
void omsub( matrix *ptm1, matrix *ptm2, matrix *ptm3 );
void omadd( matrix *ptm1, matrix *ptm2, matrix *ptm3 );

void reverse( matrix *ptv );
void setcols( matrix *ptm, int cols );
void array2mat( matrix *ptv, float *values, int n );
void madddiag( matrix *ptm1, const matrix *ptm2, const matrix *ptm3 );


#define END -1
#define ZERO_VAL (-123456.789f)

void mremoverow(matrix *ptm, int row);
void maddrow(matrix *ptm, int row, float *values);
void mremovecol(matrix *ptm, int col);
void maddcol(matrix *ptm, int col, float *values);
void mresize(matrix *ptm, int rows, int cols);

#if PRINTF_FLOAT != ON
void printFloat(float v, int decimalDigits);
#endif



// Matrix init
void minvinit(int n);



// End prototypes from matcore.c
// *******************************************

// *******************************************
// Prototypes from matio.c

void makelist( varlisttype *varlist, char *name, int index );
void freelist( varlisttype *varlist );
float sload( char file_name[], char scal_name[] );
matrix *mload( char file_name[], char mat_name[] );
matrix *mlload( varlisttype *varlist, char mat_name[] );
float slload( varlisttype *varlist, char scal_name[] );
#if IO == ON
 #include <stdio.h>
 void matsave( FILE *fp, matrix *ptm, char mat_name[] );
 void matload( FILE *fp, varlisttype *varlist );
#endif
long longconv( long inval );
void matconv( int mn, float *ptd );

// End Prototypes from matio.c
// *******************************************

// *******************************************
// Prototypes from matinv.c

void minv(matrix *ptm1, matrix *ptm2);
void ludcmp( matrix *a, int N, int *indx, float *d );
void lubksb( matrix *a, int N, int *indx, float b[] );
int minvgauss( matrix *ptm1, matrix *ptm2 );
int gaussj( matrix *a, int n, matrix *b, int m );

// End Prototypes from matinv.c
// *******************************************
# ifdef __cplusplus
 }  // extern C
# endif 

#endif /* _IAU_MAT_H */
