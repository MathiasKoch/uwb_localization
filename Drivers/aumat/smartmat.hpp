/*
 * smartmat.hpp
 *
 *  Created on: Oct 22, 2010
 *      Author: Enis Bayramoglu (eba@elektro.dtu.dk)
 */

#ifndef SMARTMAT_HPP_
#define SMARTMAT_HPP_

#include "iau_mat.h"
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;
/*
 *  TODO: add index checking complying with aumat
 *  TODO: make sure that expressions are not automatically converted to floats
 *  TODO: make sure that an expression is accepted by an aumat function
 *  TODO: make sure that all plausible expression combinations are supported, test cases:
 *  mat*mat
 *  mat*float
 *  mat*scalar
 *  float*mat
 *  scalar*mat
 *  vec*scalar
 *  scalar*vecTrans
 *  TODO: make sure that the following give a compiler error, pointing at the correct line:
 *  mat*scalarexp
 *  scalarexp*mat
 *  scalarexp*vec
 *  vecTrans*scalarexp
 *  TODO: make sure scalarmat behaves like a float when necessary
 */
class tempscalar;
template<int,int,bool=false> class smartmat;
template<int,int> class smartmatBase;
template<> class smartmat<1,1,false>;
typedef smartmat<1,1,false> scalar;
template<int total, int element,bool valid> class matrixFiller;
template<int row>class matrixproxy;




template<int row,int col> class elementBase{
protected:
	float data[row][col];
};
template<int row> class elementBase<row,row> {
protected:
	float data[row][row];
public:
	const smartmat<row,row> invgauss();
	const smartmat<row,row> chol() const;
	const float trace() const;
};
template<> class elementBase<1,1>{
protected:
	float data[1][1];
public:
	inline operator const float() const {return data[0][0];}
	inline operator float &() {return data[0][0];}
	inline smartmatBase<1,1> invgauss();
	inline const smartmat<1,1> chol() const;
	inline const float trace() const;

};

template<int row, int col, int height, int width>
class submatrix {
public:
	smartmatBase<row,col> * mat;
	int rb, cb;
	submatrix(smartmatBase<row,col> * mat, int rb, int cb):mat(mat), rb(rb), cb(cb){}
	//operator const smartmat<height, width> () const;
	submatrix & operator=(const smartmatBase<height,width> & rhs);
	submatrix & operator=(const submatrix & rhs) {return operator=((smartmat<height,width>) rhs);}
};


template<int row, int col>
class smartmatBase: public elementBase<row,col> {
protected:
	using elementBase<row,col>::data;
public:
	void setValues(float in);
	void setValues(float * in);
	template<int height, int width>
	void setValues(const submatrix<height,width,row,col> & in);
	smartmatBase(){}
	explicit smartmatBase(float in){setValues(in);};
	explicit smartmatBase(float * in){setValues(in);};
	smartmatBase(const smartmatBase & in);
	inline int rows(){return row;}
	inline int cols(){return col;}
	inline matrixFiller<row*col,1,(0<row*col)> operator=(float in);
	const smartmat<col,row> transpose() const;
	inline const smartmat<col,row> t()const{return transpose();}
	inline float & operator()(int r, int c) {return data[r][c];}
	inline const float operator()(int r, int c) const {return data[r][c];}
	smartmatBase<row,col> & operator=(const smartmatBase<row,col> &rhs);
	void setData(float* data);
	template<int col2>	const smartmat<row,col2> operator*(const smartmatBase<col,col2>& op2) const;
	const smartmat<row,col> operator+(const smartmatBase<row,col> &op2) const;
	smartmatBase<row,col> &operator+=(const smartmatBase<row,col> &op2) ;
	const smartmat<row,col> operator-(const smartmatBase<row,col> &op2) const;
	smartmatBase<row,col> &operator-=(const smartmatBase<row,col> &op2);
	const smartmat<row,col> operator-() const;
	template<int height, int width>
	const smartmat<height,width> submat(int rb, int cb) const{return smartmat<height,width>(*this,rb,cb);}
	template<int height, int width>
	submatrix<row,col,height,width> submatref(int rb, int cb) {return submatrix<row,col,height,width>(this,rb,cb);}
	inline matrixproxy<row> getProxy() {matrixproxy<row> result(&data[0][0], col); return result;}
	inline void setValues(int in){setValues((float)in);};
	template<int height, int width>
	void submatequal(const smartmatBase<height,width> & rhs, int rb, int cb) {
		for(int i=0; i<height; i++)
			for(int j=0; j<width; j++) {
				data[i+rb][j+cb] = rhs(i,j);
			}
	}
	smartmat<row,col> ewapply(float (*func)(float));
	smartmat<row,col> ewmultiply(smartmatBase<row,col> op2);
	smartmat<row,col> ewdivide(smartmatBase<row,col> op2);

};

template<int row, int col>
class nonScalarBase: public smartmatBase<row,col> {
protected:
	using elementBase<row,col>::data;
public:
	using smartmatBase<row,col>::operator*;
	nonScalarBase(){};
	nonScalarBase(const smartmatBase<row,col> &in):smartmatBase<row,col>(in){}
	const smartmat<row,col> operator*(tempscalar op2) const ;
	smartmatBase<row,col> &operator*=(tempscalar op2);
	const smartmat<row,col> operator/(tempscalar op2) const;
	smartmatBase<row,col> &operator/=(tempscalar op2);
};

template<int row>
class matrixproxy {
public:
	matrixproxy(float * data, int col) {init(data,col);}
	inline operator const matrix *() const {return &mat;}
	inline operator matrix *() {return &mat;}
protected:
	void init(float * data, int col);
	float * rowpointers[row];
	matrix mat;
};

template<int row, int col>
class proxymatBase: public nonScalarBase<row,col>, public matrixproxy<row> {
public:
	inline proxymatBase(): matrixproxy<row>(&(smartmatBase<row,col>::data[0][0]), col) {}
};
template<int row>
class proxymatBase<row,row>: public smartmatBase<row,row>, public matrixproxy<row> {
public:
	inline proxymatBase(): matrixproxy<row>(&(smartmatBase<row,row>::data[0][0]), row) {}
	smartmat<row,row,true> invgauss();
};


template<int row, int col>
class smartmat<row,col,true>: public proxymatBase<row,col> {
public:
	using smartmatBase<row,col>::operator=;
	inline smartmat(): proxymatBase<row,col>() {}
	inline matrixFiller<row*col,1,(0<row*col)> operator=(float in){return smartmatBase<row,col>::operator=(in);}
	smartmat &operator=(const smartmat &rhs) {smartmatBase<row,col>::operator=(rhs);return *this;};
	inline smartmat(const smartmatBase<row,col> &rhs): proxymatBase<row,col>() {*this=rhs;}
	inline smartmat(const smartmat &rhs): proxymatBase<row,col>() {*this=rhs;}
	using smartmatBase<row,col>::setValues;
	explicit smartmat(float in):proxymatBase<row,col>(){setValues(in);}
	explicit smartmat(int in):proxymatBase<row,col>(){setValues(in);};
	explicit smartmat(float * in):proxymatBase<row,col>(){setValues(in);}
	template<int height, int width>
	smartmat(const submatrix<height, width,row,col> & in):proxymatBase<row,col>(){setValues(in);}
};

template<int row, int col>
class smartmat<row,col,false>: public nonScalarBase<row,col> {
protected:
	using elementBase<row,col>::data;
public:
	using smartmatBase<row,col>::operator=;
	smartmat(){}
	smartmat(const smartmatBase<row,col> &in):nonScalarBase<row,col>(in){}
	smartmat(const smartmat &in);
	template<int height, int width>
	smartmat(const smartmatBase<height, width> & in, int rb, int cb);
	using smartmatBase<row,col>::setValues;
	explicit smartmat(float in){setValues(in);}
	explicit smartmat(int in){setValues(in);};
	explicit smartmat(float * in){setValues(in);}
	template<int height, int width>
	smartmat(const submatrix<height, width,row,col> & in){setValues(in);}

};

template<>
class smartmat<1,1,false>: public smartmatBase<1,1> {
public:
	inline smartmat<1,1,false> () {}
	inline smartmat<1,1,false> (float in) {data[0][0] = in;}
	inline smartmat(smartmatBase<1,1> const in) {data[0][0]=in(0,0);};
	inline smartmat<1,1> &operator =(float in)    {data[0][0] = in; return *this;}
	inline smartmat<1,1> &operator += (float rhs) {data[0][0]+=rhs; return *this;}
	inline smartmat<1,1> &operator -= (float rhs) {data[0][0]-=rhs; return *this;}
	template<int height, int width> smartmat(const smartmatBase<height, width> & in, int rb, int cb){data[0][0] = in(rb,cb);}
};


class tempscalar {
public:
	float val;
	inline tempscalar(float in) {val=in;}
};



////////// Some global functions
template<int row, int col> smartmat<row,col> operator*(tempscalar op1, nonScalarBase<row,col> op2);
template<int row, int col> inline std::ostream &operator <<(std::ostream & os , smartmatBase<row,col> in) ;
inline std::istream &operator >>(std::istream & is , smartmatBase<1,1> in) {return is>>in(0,0);}



/////////////////////////////////// Function definitions, this is needed in the header file since these are all template functions
//////////////////////////// SMARTMATBASE
template<int row, int col>
void smartmatBase<row,col>::setValues(float * in) {
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
		data[i][j] = in[i*row+j];
}
template<int row, int col>
void smartmatBase<row,col>::setValues(float in) {
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
		data[i][j] = in;
}
template<int row, int col>
smartmatBase<row,col>::smartmatBase(const smartmatBase<row,col> & in) {
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data[i][j] = in.data[i][j];
}
template<int row, int col>
const smartmat<col,row> smartmatBase<row,col>::transpose()const {
	smartmat<col,row> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result(j,i) = data[i][j];
	return result;
}
template<int row>
const float elementBase<row,row>::trace()const {
	float result=0;
	for(int i=0; i<row; i++)
		result += data[i][i];
	return result;
}
template<int row>
const smartmat<row,row> elementBase<row,row>::invgauss() {
	smartmat<row,row> res;
	minvgauss(res.getProxy(), matrixproxy<row>(&data[0][0], row));
	return res;
}
template<int row>
const smartmat<row,row> elementBase<row,row>::chol() const {
	smartmat<row,row> res;
	for(int j=0; j<row; j++) {
		res(j,j) = data[j][j];
		for(int k=0; k<j; k++ ) {
			res(j,j)-=res(j,k)*res(j,k);
		}
		res(j,j) = sqrt(res(j,j));
		for(int i=j+1; i<row; i++) {
			res(i,j) = data[i][j];
			for(int k=0; k<j; k++) {
				res(i,j)-=res(i,k)*res(j,k);
			}
			res(i,j)/=res(j,j);
		}
		// Set the untouched elements to 0
		for(int i=0; i<j; i++) {
			res(i,j)=0;
		}
	}
	return res;
}

inline const smartmat<1,1> elementBase<1,1>::chol() const {return smartmat<1,1>(sqrt(data[0][0]));}
inline const float elementBase<1,1>::trace() const {return data[0][0];}

template<int row, int col>
inline smartmat<row,col> smartmatBase<row,col>::ewapply(float (*fun)(float)) {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j] = fun(data[i][j]);
	return result;
}

template<int row, int col>
inline smartmat<row,col> smartmatBase<row,col>::ewmultiply(smartmatBase<row,col> op2) {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j] = data[i][j]*op2.data[i][j];
	return result;
}

template<int row, int col>
inline smartmat<row,col> smartmatBase<row,col>::ewdivide(smartmatBase<row,col> op2) {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j] = data[i][j]/op2.data[i][j];
	return result;
}


template<int row, int col>
inline smartmatBase<row,col> & smartmatBase<row,col>::operator=(const smartmatBase<row,col>& rhs){
	for(int i=0; i<row; i++)
		for(int j=0; j<col;j++)
			data[i][j]=rhs.data[i][j];
	return *this;
}
template<int row, int col>
void smartmatBase<row,col>::setData(float * in) {
	for(int i=0; i<row*col; i++)
		(&data[0][0])[i] = in[i];
	return *this;
}
template<int row, int col>
template<int col2>
const smartmat<row,col2> smartmatBase<row,col>::operator*(const smartmatBase<col,col2> &op2) const {
		smartmat<row,col2> result(0.0);
		for(int i=0; i<row; i++)
			for(int j=0; j<col2; j++)
				for(int k=0; k<col; k++)
					result(i,j) += data[i][k]*op2(k,j);
		return result;
}
template<int row, int col>
inline const smartmat<row,col> nonScalarBase<row,col>::operator*(tempscalar op2) const {
	smartmat<row,col> result(*this);
	result*=op2;
	return result;
}
template<int row, int col>
smartmatBase<row,col> & nonScalarBase<row,col>::operator*=(tempscalar op2){
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
		data[i][j] *= op2.val;
	return *this;
}
template<int row, int col>
inline const smartmat<row,col> nonScalarBase<row,col>::operator/(tempscalar op2) const{
	smartmat<row,col> result(*this);
	result/=op2.val;
	return result;
}
template<int row, int col>
inline smartmatBase<row,col> & nonScalarBase<row,col>::operator/=(tempscalar op2){
	*this*=1/op2.val;
	return *this;
}
template<int row, int col>
inline const smartmat<row,col> smartmatBase<row,col>::operator+(const smartmatBase<row,col>& op2) const {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j]=data[i][j]+op2.data[i][j];
	return result;
}
template<int row, int col>
inline smartmatBase<row,col> &smartmatBase<row,col>::operator+=(const smartmatBase<row,col>& op2) {
	for(int i = 0; i<row; i++)
		for(int j=0; j<col; j++)
			data[i][j]+=op2.data[i][j];
	return *this;
}
template<int row, int col>
inline const smartmat<row,col> smartmatBase<row,col>::operator-(const smartmatBase<row,col> &op2) const {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j]=data[i][j]-op2.data[i][j];
	return result;
}
template<int row, int col>
smartmatBase<row,col> &smartmatBase<row,col>::operator-=(const smartmatBase<row,col> &op2) {
	for(int i = 0; i<row; i++)
		for(int j=0; j<col; j++)
			data[i][j]-=op2.data[i][j];
	return *this;
}
template<int row, int col>
inline const smartmat<row,col> smartmatBase<row,col>::operator-() const {
	smartmat<row,col> result;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			result.data[i][j]=-data[i][j];
	return result;
}
template<int row, int col>
inline matrixFiller<row*col,1,(0<row*col)> smartmatBase<row,col>::operator=(float in){
	matrixFiller<row*col,0, (0<row*col)> process (&smartmatBase<row,col>::data[0][0]);
	return (process,in);
}

///////////////////////////////////////// MATRIXPROXY

template<int row>
void matrixproxy<row>::init(float *data, int col) {
    for (int i=0;i<row;i++)
            rowpointers[i] =  data+i*col;

	mat.mat = rowpointers;
	mat.col = col;
	mat.row = row;
}

////////////////////////////////////// SMARTMAT

template<int row, int col>
smartmat<row,col,false>::smartmat(const smartmat<row,col> & in){
		for(int i=0; i<row; i++)
			for(int j=0; j<col; j++)
				data[i][j] = in.data[i][j];
}
template<int row>
inline smartmat<row,row,true> proxymatBase<row,row>::invgauss() {
	smartmat<row,row,true> res;
	minvgauss(res,*this);
	return res;
}

template<int total, int element,bool valid>
class matrixFiller {
};

template<int total, int element>
class matrixFiller<total,element, true> {
private:
	float * dataIn;
public:
	inline matrixFiller(float * data) {
		dataIn = data;
	}
	inline matrixFiller<total,element+1, (element<total)> operator,(float in) {
		dataIn[element] = in;
		matrixFiller<total,element+1, (element<total)> nextFiller(dataIn);
		return nextFiller;
	}
};


////////////////////////////////////// ELEMENTBASE

inline smartmatBase<1,1> elementBase<1,1>::invgauss(){
	return smartmatBase<1,1>(1/data[0][0]);
}

template<int row, int col, int height, int width>
submatrix<row,col,height,width> & submatrix<row,col,height,width>::operator=(const smartmatBase<height,width> & rhs) {
	mat->submatequal(rhs,rb,cb);
	return *this;
}


template<int height, int width>
template<int row, int col>
smartmat<height,width>::smartmat(const smartmatBase<row,col> & in, int rb, int cb) {
	for(int i=0; i<height; i++)
		for(int j=0; j<width; j++)
			data[i][j] = in(i+rb,j+cb);
}

template<int row, int col>
template<int height, int width>
void smartmatBase<row,col>::setValues(const submatrix<height,width,row,col> & in) {
	int rb = in.rb, cb= in.cb;
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data[i][j] = in.mat->data[i+rb][j+cb];
}

////////////////////////////////////// OTHER FUNCTIONS


template<int row, int col>
smartmat<row,col> operator*(tempscalar op1, nonScalarBase<row,col> op2){
	smartmat<row,col> result=op2;
	return result*=op1;
}


template<int row, int col>
inline std::ostream &operator <<(std::ostream & os , smartmatBase<row,col> in)
{
	for(int i =0; i<row; i++) {
		for(int j=0; j<col; j++)
			os<<in(i,j)<<" ";
		os<<"\n";
	}
	return os;
}

#endif /* SMARTMAT_HPP_ */
