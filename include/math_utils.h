/* 
 * File:   math_utils.h
 * Author: Mauro Palumbo
 *
 * C++ Library with useful mathematical functions and constants
 * Created on January 22, 2015, 10:18 AM
 * 
 * TODO:
 * - put some utilities from numerics and limits libraries (ex. smallest double, ...)
 */

#ifndef MATH_UTILS_H
#define	MATH_UTILS_H

#include <cmath>

using DP = double;   // To switch from single to double precision and viceversa use DP!

using namespace std;  // make namespace std available through all the code

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//

// Definitions of useful mathematical constants 

const double pi  { 3.1415926535897932384626433832795 };		// Pi greek
const double big { 1.0e30                            };		// a big number	

const double eps { 1.0e-30                            };	// a small number (to be revised, put machine accuracy)	

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Definition of some useful inline functions

inline DP ylny(DP y)
{
    if (y<1.0e-8) return 0.0;
    else return y*log(y);
}

inline DP coth(DP y) { return cosh(y)/sinh(y); }

template<class T>
inline const T sqr(const T a) {return a*a;}

template<class T>
inline const T sqr3(const T a) {return a*a*a;}

template<class T>
inline const T sqr4(const T a) {return a*a*a*a;}

template<class T>
inline const T max(const T &a, const T &b)
{ return b > a ? (b) : (a);}

inline float max(const double &a, const float &b)
{ return b > a ? (b) : float(a);}

inline float max(const float &a, const double &b)
{ return b > a ? float(b) : (a);}

template<class T>
inline const T minimum(const T &a, const T &b)
{ return b < a ? (b) : (a);}

inline float minimum(const double &a, const float &b)
{ return b < a ? (b) : float(a);}

inline float minimum(const float &a, const double &b)
{ return b < a ? float(b) : (a);}

template<class T>
inline const T sign(const T &a, const T &b)
{ return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float sign(const float &a, const double &b)
{ return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float sign(const double &a, const float &b)
{ return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

template<class T>
inline void swapT(T &a, T &b)
{ T dum=a; a=b; b=dum;}

template<class T>
inline void shft(T &a,T &b,T &c,T &d)
{ a=b; b=c; c=d; }

//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Definition of the VECTOR class and related functions

template<class T>
class VECTOR {
private:
    int nn;
    T *v;
public:
    VECTOR();
    explicit VECTOR(int n);                   // Initialize to zero
    VECTOR(const T &a, int n);                // Initialize to constant
    VECTOR(const T *a, int n);                // Initialize to array
    VECTOR(const VECTOR &b);                    // Copy constructor
    VECTOR & operator=(const VECTOR &b);        // Assign the whole vector b
    VECTOR & operator=(const T &a);           // Assign a to every element of the array
    inline T & operator[](const int i);       // subscripting: pointer to element i
    inline const T & operator[](const int i) const;
    inline int size() const;
    ~VECTOR();
};

template<class T>
VECTOR<T>::VECTOR() : nn(0), v(0) {}

template<class T>
VECTOR<T>::VECTOR(int n) : nn(n), v(new T[n]) {}

template<class T>
VECTOR<T>::VECTOR(const T &a, int n) : nn(n), v(new T[n])
{
    for (int i=0; i<n; i++)
            v[i]=a;
}

template<class T>
VECTOR<T>::VECTOR(const T *a, int n) : nn(n), v(new T[n])
{
    for (int i=0; i<n; i++)
            v[i]=*a++;
}

template<class T>
VECTOR<T>::VECTOR(const VECTOR &b) : nn(b.nn), v(new T[nn])
{
    for (int i=0; i<nn; i++)
            v[i]=b[i];
}

template<class T>
VECTOR<T> & VECTOR<T>::operator =(const VECTOR& b)
// Normal assignement via copying has been performed
// if the vector and b have different sizes, then the vector
// is resized to match the size of b
{
    if (this != &b) {
        if (nn != b.nn) {
            if (v!=0)  delete[] (v);
            nn=b.nn;
            v=new T[nn];
        }
        for (int i=0; i<nn; i++)
            v[i]=b[i];
    }
    return *this;
}

template<class T>
VECTOR<T> & VECTOR<T>::operator =(const T& a)
{
    for (int i=0; i<nn; i++)
            v[i]=a;
    return *this;
}

template<class T>
inline T & VECTOR<T>::operator[](const int i)
{
    return v[i];
}

template<class T>
inline const T & VECTOR<T>::operator[](const int i) const
{
    return v[i];
}

template<class T>
inline int VECTOR<T>::size() const
{
    return nn;
}

template<class T>
VECTOR<T>::~VECTOR()
{
    if (v!=0)
        delete[] (v);
}

//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Definition of the MATRIX class and related functions

template<class T>
class MATRIX {
private:
    int nn;
    int mm;
    T **v;
public:
    MATRIX();
    MATRIX(int n, int m);                     // Initialize to zero
    MATRIX(const T &a, int n, int m);         // Initialize to constant
    MATRIX(const T *a, int n, int m);         // Initialize to matrix
    MATRIX(const MATRIX &b);                    // Copy constructor
    MATRIX & operator=(const MATRIX &b);        // Assign the whole matrix b
    MATRIX & operator=(const T &a);           // Assign a to every element of the matrix
    inline T* operator[](const int i);      // subscripting: pointer to row i
    inline const T* operator[](const int i) const;
    inline int nrows() const;
    inline int ncols() const;
    ~MATRIX();
};

template<class T>
MATRIX<T>::MATRIX() : nn(0), mm(0), v(0) {}

template<class T>
MATRIX<T>::MATRIX(int n, int m) : nn(n), mm(m), v(new T*[n])
{
    v[0]=new T[m*n];
    for (int i=1; i<n; i++)
        v[i]=v[i-1]+m;
}

template<class T>
MATRIX<T>::MATRIX(const T &a, int n, int m) : nn(n), mm(m), v(new T*[n])
{
    int i,j;

    v[0]=new T[m*n];
    for (i=1; i<n; i++)
        v[i]=v[i-1]+m;
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            v[i][j]=a;
}

template<class T>
MATRIX<T>::MATRIX(const T *a, int n, int m) : nn(n), mm(m), v(new T*[n])
{
    int i,j;

    v[0]=new T[m*n];
    for (i=1; i<n; i++)
        v[i]=v[i-1]+m;
    for (i=0; i<n; i++)
        for (j=0; j<m; j++)
            v[i][j]=*a++;
}

template<class T>
MATRIX<T>::MATRIX(const MATRIX &b) : nn(b.nn), mm(b.mm), v(new T*[nn])
{
    int i,j;

    v[0]=new T[mm*nn];
    for (i=1; i<nn; i++)
        v[i]=v[i-1]+mm;
    for (i=0; i<nn; i++)
        for (j=0; j<mm; j++)
            v[i][j]=b[i][j];
}

template<class T>
MATRIX<T> & MATRIX<T>::operator =(const MATRIX& b)
// Normal assignement via copying has been performed
// if the matrix and b have different sizes, then the matrix
// is resized to match the size of b
{
    if (this != &b) {
        int i,j;
        if (nn != b.nn || mm != b.mm) {
            if (v!=0) {
                delete[] (v[0]);
                delete[] (v);
            }
            nn=b.nn;
            mm=b.mm;
            v=new T*[nn];
            v[0]=new T[mm*nn];
        }
        for (i=1;i<nn;i++)
            v[i]=v[i-1]+mm;
        for (i=0; i<nn; i++)
            for (j=0;j<mm;j++)
            v[i][j]=b[i][j];
    }
    return *this;
}

template<class T>
MATRIX<T> & MATRIX<T>::operator =(const T& a)
{
    for (int i=0; i<nn; i++)
        for (int j=0;j<mm;j++)
            v[i][j]=a;
    return *this;
}

template<class T>
inline T* MATRIX<T>::operator[](const int i)
{
    return v[i];
}

template<class T>
inline const T* MATRIX<T>::operator[](const int i) const
{
    return v[i];
}

template<class T>
inline int MATRIX<T>::nrows() const
{
    return nn;
}

template<class T>
inline int MATRIX<T>::ncols() const
{
    return mm;
}

template<class T>
MATRIX<T>::~MATRIX()
{
    if (v!=0) {
        delete[] (v[0]);
        delete[] (v);
    }
}


// Vector types, names for different vector types to make the code easily generalizable

typedef const VECTOR<bool> VIb;
typedef VECTOR<bool> Vb, VOb, VIOb;

typedef const VECTOR<char> VIc;
typedef VECTOR<char> Vc, VOc, VIOc;

typedef const VECTOR<int> VIi;
typedef VECTOR<int> Vi, VOi, VIOi;

typedef const VECTOR<unsigned int> VIui;
typedef VECTOR<unsigned int> Vui, VOui, VIOui;

typedef const VECTOR<long> VIl;
typedef VECTOR<long> Vl, VOl, VIOl;

typedef const VECTOR<unsigned int> VIui;
typedef VECTOR<unsigned int> Vui, VOui, VIOui;

typedef const VECTOR<float> VISP;
typedef VECTOR<float> VSP, VOSP, VIOSP;

typedef const VECTOR<DP> VIDP;
typedef VECTOR<DP> VDP, VODP, VIODP;

// Matrix types

typedef const MATRIX<bool> MIb;
typedef MATRIX<bool> Mb, MOb, MIOb;

typedef const MATRIX<char> MIc;
typedef MATRIX<char> Mc, MOc, MIOc;

typedef const MATRIX<int> MIi;
typedef MATRIX<int> Mi, MOi, MIOi;

typedef const MATRIX<unsigned int> MIui;
typedef MATRIX<unsigned int> Mui, MOui, MIOui;

typedef const MATRIX<long> MIl;
typedef MATRIX<long> Ml, MOl, MIOl;

typedef const MATRIX<unsigned int> MIui;
typedef MATRIX<unsigned int> Mui, MOui, MIOui;

typedef const MATRIX<float> MISP;
typedef MATRIX<float> MSP, MOSP, MIOSP;

typedef const MATRIX<DP> MIDP;
typedef MATRIX<DP> MDP, MODP, MIODP;

//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Given a matrix a[0..n-1][0..n-1], this routine replaces it by the LU decomposition of
 * a rowwise permutation of itself. a is input. On output, it is arranged as in eq. (ref.)
 * above; indx[0..n-1] is an output vector that records the row permutation effected by
 * the partial pivoting; d is the output as +-1 depending on whether the number of row 
 * interchanges was even or odd respectively. This routine is used in combination 
 * with lubksb to solve linear equations or invert a matrix.
 *
 */
void ludcmp(MIODP &a, VOi &indx, DP &d);


/* Solves the set of linear equations A X=B. Here a[0..n-1][0..n-1] is input, not as the
 * matrix A but rather as its LU decomposition, determined by the routine ludcmp.
 * indx[0..n-1] is input as the permutation vector returned by ludcmp. b[0..n-1] is input
 * as the right-hand side vector B and returns with the solution vector X. a and indx are
 * not modified by this routine and can be left in place for successive calls with different
 * right-hand sides b vectors. This routine takes into account the possibility that b will 
 * begin with many zero elements, so it is efficient for use in matrix inversion.
 */
void lubksb(MIODP &a, VOi &indx, VIODP &b);

// 
// Numerical routine for numerical derivatives with Savitsky-Golay method
//
void savgol(VODP &c, const int np, const int nl, const int nr, const int ld, const int m);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following routine is a linear equations solution by Gauss-Jordan elimination, 
// with full (explicit) pivoting. a[1..n][1..n] is the input matrix. b[1..n][1..m] 
// is input containing the m right-hand side vectors (m systems of linear equations
// are solved at the same time). On output, a is replaced by its matrix inverse
// and b is replaced by the corresponding set of solution vectors. The inverse matrix 
// obtained in output can be used to calculate a new set of solutions for another
// vector b[1..n] not included in the call to this function, but this is not recommended.
// (Explicit pivoting means that the biggest coefficient in each column is selected 
// as pivot element. Implicit pivoting requires a preliminary normalization of the 
// coefficients before selecting the pivot, but only a minor performance improvement 
// can be obtained.) This routine is suitable to be used with the Levemberg-Marquardt
// routine in this library.
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void gaussjordan(MIODP &a, MIODP &b);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// This routine is used by Levemberb-Marquardt routine. Expand in storage the 
// covariance matrix covar, so as to take into account parameters that are being
// held fixed. (For the latter, return zero covariances)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void covarsrt(MIODP &ccovar, VIb &ia, const int mfit);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// This routine is used by Levermberg-Marquardt routine to evaluate the linearized 
// fitting matrix alpha, and vector beta and calculate the chi squared.
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void mrqcof(VIDP &x, VIDP &y, VIDP &sigma, VIODP &a, VIb &ia, MIODP &alpha, 
        VODP &beta, DP &chisq, void funcs(const DP, VIDP &, DP &, VODP &));

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 This is an implementation of the Levenberg-Marquadt method, attempting to reduce the value of the "chi squared" of a fit between
 a set of data points x[1..ndata], y[1..ndata] with individual standard deviations sigma[1..ndata], and a nonlinear function dependent on
 ma coefficients a[1..ma]. The input array ia[1..ma] indicates the nonzero entries those components that should be fitted for,
 and by zero entries those components that should be held fixed at their input values. The program returns current best-fit
 values of the parameters a[1..ma] and "chi squared"=chisq. The matrices covar[1..ma][1..ma], alpha[1..ma][1..ma] are used as
 working space during most iterations. The caller must supply a routine funcs(x,a,yfit,dyda,ma) that evaluates the fitting
 function yfit and its derivatives dyda[1..ma] with respect to the fitting parameters a at x. On the first call provide an initial
 guess for the parameters a, and set lambda<0 for initialization (which then sets alamnda=0.001). lambda is the coefficient to switch
 between a steepest descent step to an inverse hessian step. If a step succeeds chisq becomes
 smaller and lambda decreases by a factor of 10. If a step fails lambda grows by a factor of 10. This routine must be called
 repeatedly until convergence is achieved. Then, make one final call with lambda=0, so that covar[1..ma][1..ma] returns
 the covariance matrix, and alpha the curvature matrix. Parameters held fixed will return zero covariance.
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void marquardt(VIDP &x, VIDP &y, VIDP &sigma, VIODP &a, VIb &ia, MIODP &covar,
        MIODP &alpha, DP &chisq, void funcs(const DP, VIDP &, DP &, VODP &),
        DP &lambda);

#endif	/* MATH_UTILS_H */

