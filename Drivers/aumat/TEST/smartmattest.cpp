/*
 * smartmattest.cpp
 *
 *  Created on: 29/10/2010
 *      Author: Enis Bayramoglu (eba@elektro.dtu.dk)
 */

#include <iostream>
using namespace std;

#include "smartmat.hpp"
#include "iau_ukf.hpp"
#include "cmath"

class hostclass {
public:
	smartmat<3,1> statetrans(smartmat<3,1> oldstate, smartmat<2,1> noise, smartmat<1,1> input); // state transition equation for the UKF
};

smartmat<1,1> measeq(smartmat<3,1> state, smartmat<1,1> noise, smartmat<0,1> auxin); // Measurement equation for the UKF

smartmat<1,1> measerreq(smartmat<3,1> state, smartmat<1,1> noise, smartmat<1,1> meas, smartmat<0,1> auxin) {
	return meas-measeq(state,noise,auxin);
}

smartmat<2,1> transformation(smartmat<3,1> in, int auxin) {
	smartmat<2,1> result;
	result(0,0) = in(0,0)+in(1,0);
	result(1,0) = in(0,0)-in(1,0);
	return result;
}

int main (int argc, char **argv) {

	// The smartmat library provides the smartmat class used to represent matrices. The focus of this library is to
	// improve readability of the source code while increasing the performance at the same time. enabling the -O3
	// option on the .cpp files using smartmat can show up to 2 times performance improvement. Though it is worth noting
	// that the library is quite fast even without doing so. While using the library, please don't attempt to increase
	// performance by sacrificing readability, it is usually unnecessary.

	// Declaring a 3x3 matrix represented by a smartmat object
	smartmat<3,3> m1;
	// There is no need to allocate space for m1, since there are no pointers in m1, everything is allocated staticly

	// We can fill m1 the following way
	m1 = 2, 0, 0,
		 0, 2, 0,
		 0, 0, 2;
	cout<<"\nm1 after setting values:\n"<<m1; // So yes, you can print m1 like that.
	// This way of filling a matrix cannot be used during declaration. So, you can't say:
	// 	smartmat <3,3> m2 = 2, 0, 0,
	//       				0, 2, 0,
	//      				0, 0, 2; WRONG!!! This is impossible in C++ because "," is handled differently during declaration
	// Trying to put more elements into m1 than its size also causes a compiler error.
	// If you want to initialize during declaration, the following can be used:
	double data[] = {3, 0, 0,
					 0, 3, 0,
					 0, 0, 3};

	smartmat<3,3> m2(data);
	cout<<"\nm2 after declaration:\n"<<m2;


	//If you want to use a double array to insert values somewhere after declaration, you can use the following:
	m2.setValues(data);


	// It is also possible to initialize a matrix filled with a constant value:
	smartmat<3,2> m3(1);

	// Again, you can also do this after declaration with the setValues function
	m3.setValues(2);

	// The elements of the matrices can be accessed using the MATLAB notation
	cout<<"\nm2(0,0):\n"<<m2(0,0)<<"\n";

	// You can also use the same notation at the output
	m2(0,0) = 5;
	cout<<"\nm2 after the operation m2(0,0) = 5:\n"<<m2;

	// Matrix arithmetic expressions using the MATLAB notation are supported:
	smartmat<3,3> m4 = m1+m2;
	cout << "\nm4 = m1+m2:\n" << m4;
	cout << "\nm1-m2\n" << m1-m2;
	cout << "\nm1*m2\n" << m1*m2;
	cout << "\nm1*(m1+m2*(-m1-m2)):\n"<<m1*(m1+m2*(-m1-m2));
	// The unary "-" operator is also supported as in the previous case (-m1-m2). It means an extra copy operation but
	// use it if it is easier to read. You can also use "+=" and "-=". They look nice and they are faster.

	// Trying to perform incompatible matrix arithmetic operations will result in a compiler error!!! For instance:
	// m2*m3 is valid while m3*m2 or m3+m2 aren't (due to their dimensions in this example)

	// Other supported operations are inversion, transpose and trace. Cholesky decomposition on symmetric matrices is
	// also supported
	smartmat<2,3> m5; m5 = 2, 0, 0,
						   0, 2, 1;
	cout<<"\nm5.transpose():\n"<<m5.transpose(); // m4.t() works just as well and it might improve readability
	cout<<"\nm4.trace():\n"<<m4.trace()<<"\n";
	cout<<"\nm4.invgauss():\n"<<m4.invgauss();

	smartmat<3,3> msym; msym = 4, 1, 3,
							   1, 4, 2,
							   3, 2, 4;

	cout<<"\nmsym:\n"<<msym;
	cout<<"\nmsym.chol():\n"<<msym.chol();

	// Another supported arithmetic operation is multiplication and division by a double
	cout<<"\nm5/5:\n"<<m5/5;
	// "*=" and "/=" are also supported with double operands.


	// 1x1 matrices are equivalent to doubles in elementary operations. Note that matrix expressions
	// resulting in 1x1 matrices are also included. Note that the matrix multiplication/division with
	// a double is an exception to this. This is in place on purpose to avoid ill formed expressions
	// with 1x1 elements to be admitted by accident.

	scalar s=5; // scalar is an alias for smartmat<1,1>, they are completely equivalent
	cout<<"\ns>3:\n"<< (s>3) <<"\n";
	smartmat<3,1> v; v = 1,
			             2,
			             3;
	cout<<"\nv:\n"<<v;
	cout<<"\nv.t()*v<10:\n"<< (v.t()*v<10) <<"\n";
	// v.t()*v*m4; This results in a compiler error as this clearly is an error!!! If this is really
	// what you want to do, use: double(v.t()*v)*m4;


	// It is possible to extract sub matrices from matrices very conveniently. These sub matrices can be used just as matrices.
	smartmat<3,2> m6 = m2.submat<3,2>(0,1); // Here, the template arguments specify the size of the sub matrix, while the
	cout<<"\nm6=m2.submat<3,2>(0,1):\n"<<m6;// function arguments specify the beginning row and the beginning columns respectively

	// If you want to use a sub matrix at the output, you should use submatref:
	m2.submatref<3,2>(0,1) = m1*m2.submat<3,2>(0,1); // Demonstrating also the use of sub matrices inside expressions
	cout<<"\nm2 after m2.submatref<3,2>(0,1) = m1*m2.submat<3,2>(0,1):\n"<<m2;

	// There are also element wise operations. They are called as member functions, whose names all start with "ew". Element wise
	// multiplication and division are applied as follows:
	cout<<"\nm1.ewmultiply(m2):\n"<<m1.ewmultiply(m2);
	cout<<"\nm1.ewdivide(msym):\n"<<m1.ewdivide(msym);

	// It is also possible to apply a unary function to each of the matrix elements as follows:
	cout<<"\nm1.ewapply(sin):\n"<<m1.ewapply(sin);

	// The size of a smartmat must be a compile time constant, so, if you need to use variable size matrices smartmat cannot be
	// used to represent them. You can use the aumat "matrix" struct to hold them. Then it is possible to use smartmat objects
	// inside aumat functions using the getProxy() function.
	matrix * m7 = mmake(3,3);
	array2mat(m7,data,9);
	cout<<"\nm7:\n"; mprint(m7);

	madd(m2.getProxy(),m2.getProxy(),m7); // Note that there is no dimension compatibility check in this operation
	cout<<"\nm2 after madd(m2.getProxy(),m2.getProxy(),m7):\n"<<m2;

	// It is also possible to define smartmat objects that already carry around their matrix proxies, so that you can directly
	// use them inside aumat functions. They are preferable if you will use a matrix often with aumat matrices, otherwise use the
	// usual smartmat objects as they are slightly (just slightly) faster to create and pass around. When you define a smartmat
	// object with a third template argument "true", it will carry around a matrix proxy
	smartmat<3,3,true> m8 = m1;

	madd(m7,m8,m7);

	// Have fun, and remember, never sacrifice readability in an attempt to increase performance!!!

	// Test code for the generic UKF functions

	const int statedim = 3, procnoisedim = 2, inputdim = 1, measdim = 1, measnoisedim = 1;

	smartmat<statedim,1> stateest; stateest = 1,
											  2,
											  3;
	smartmat<statedim,statedim> statecov; statecov = 4,0,0,
													 0,4,0,
													 0,0,4;
	smartmat<procnoisedim,procnoisedim> noisecov; noisecov= 1,0,
															0,1;
	smartmat<inputdim,1> input=5;

	hostclass hostobj;

	cout<<"\nstateest:\n"<<stateest;
	cout<<"\nstatecov:\n"<<statecov;
	cout<<"\nnoisecov:\n"<<noisecov;
	iau_ukf::predict(stateest, statecov,input,noisecov,hostobj,&hostclass::statetrans);
	cout<<"\n UKF predict\n";
	cout<<"\nstateest:\n"<<stateest;
	cout<<"\nstatecov:\n"<<statecov;


	smartmat<measnoisedim,measnoisedim> measnoisecov = 4;
	smartmat<measdim,1> meas = 11;

	iau_ukf::update(stateest,statecov,meas,measnoisecov,smartmat<0,1>(),measeq);

	cout<<"\n UKF update\n";
	cout<<"\nstateest:\n"<<stateest;
	cout<<"\nstatecov:\n"<<statecov;

	smartmat<3,1> inest; inest = 3,2,1;
	smartmat<3,3> incov; incov = 2,0,0,
								 0,1,0,
								 0,0,1;
	smartmat<2,1> outest;
	smartmat<2,2> outcov;

	iau_ukf::unscentedtransform(inest,incov,outest,outcov,0,transformation);

	cout<<outest;
	cout<<outcov;

}


smartmat<3,1> hostclass::statetrans(smartmat<3,1> oldstate, smartmat<2,1> noise, smartmat<1,1> input) {
	smartmat<3,1> result; result=oldstate;
	result(0,0) = exp(noise(0,0))+result(0,0);
	result(1,0) = noise(0,0)+result(1,0);
	result(2,0) += input;
	return result;
}

smartmat<1,1> measeq(smartmat<3,1> state, smartmat<1,1> noise, smartmat<0,1> auxin) {
	return noise + state(0,0) + state(1,0) + state(2,0);
}
