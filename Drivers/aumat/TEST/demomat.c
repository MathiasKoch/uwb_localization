/**\file demomat.c
 * \brief Demonstration of the basic matrix operations in iau_mat.
 * The idea is to use this as a benchmark for the other matrix library
 * candidates. Original demomat.c written by Ole Ravn.
 *
 * \author Lars Valdemar Mogensen
 * \author Ole Ravn
 *
 * \date 05/15-2006
 * \version 1.0
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <iau_mat.h>

/** \fn main(int argc, char **argv)
 * Test and demo rutine for the matrix library.
 * Simple operations are carried out and displayed.
 * 
 * \param[in] argc    Not used
 * \param[in] **argv  Not used
 */
int main (int argc, char **argv)
{
	matrix *a, *b, *c;
	
/*	Allocate matrices */

	a = mmake(3,3);
	b = mmake(3,3);
	c = mmake(3,3);

/* 	Initialise and do calculations */
	
	minitx(a,5.5);	// Iinitialize all elements to 5.5
	
	mdiag(b,2.0);	// Initialize to identity and multiply by 2.0
	
	smul(b,b,5);	// Scale b by 5.0
	
	madd(c,a,b);	// Perform c = a + b

/* 	Output results  */

	printf("Matrix a = \n");
	mprint(a);
	printf("Matrix b = \n");
	mprint(b);
	printf("Matrix c = a + b \n");
	mprint(c);

/*	Free memory  */

	mfree(a);
	mfree(b);
	mfree(c);
	
	return 0;
}
	
