#include <iostream>
#include "../include/math_utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ludcmp(MIODP &a, VOi &indx, DP &d)
{
    const DP TINY=1.0e-20;              // a small number
    int i, imax, j, k;
    DP big, dum, sum, temp;
    
    int n=a.nrows();
    VDP vv(n);                          // vv stores the implicit scaling of each row
    d=1.0;                              // no row interchanged yet
    
    for (i=0; i<n; i++)  {               // loop over row to get the implicit scaling information
        big=0.0;
        
        for (j=0;j<n;j++)
            if ((temp=fabs(a[i][j])) > big)  big=temp;
        
        if (big == 0.0) cerr<<"Singular matrix in routine ludcmp";          // no non zero largest element
        vv[i]=1.0/big;                                                      // Save the scaling
    }
    
    for (j=0; j<n; j++)  {              // This loop is over the columns in the Crout's method
        for (i=0; i<j; i++)  {          // this eq. is 2.3.12 except for i=j
            sum=a[i][j];
            for (k=0;k<i;k++) sum -= a[i][k]*a[k][j];
            a[i][j]=sum;
        }
        big=0.0;                        // Initialize for the search of largest pivot element
        for (i=j; i<n; i++)  {
            sum=a[i][j];                // this is the i=j case in eq. 2.3.12 and i=j+1... N-1 of eq. 2.3.13
            for (k=0;k<j;k++) sum -= a[i][k]*a[k][j];
            a[i][j]=sum;
            if ((dum=vv[i]*fabs(sum)) >= big) {         // is the figure of merit of the pivot better than the best so far?
                big=dum;
                imax=i;
            }
        }
        if (j!=imax) {                  // Do we need to interchange rows?
            for (k=0;k<n;k++) {         // Yes, do so...
                dum=a[imax][k];
                a[imax][k]=a[j][k];
                a[j][k]=dum;
            }
            d = -d;                     // ...and change the parity of d
            vv[imax]=vv[j];             // Also interchange the scale factor
        }
        
        indx[j]=imax;
        if (a[j][j]==0.0) a[j][j]=TINY;         // If the pivot element is zero, the matrix is singular
                                                // (at least to the precision of the algorithm). For some 
                                                // applications on singular matrixes, it is desirable to 
                                                // substitute TINY for zero.
        if (j != n-1)  {                        // Now, finally, divide by the pivot element...
            dum=1.0/a[j][j];
            for (i=j+1;i<n;i++)  a[i][j]*=dum;
        }
    }                                           // Go back to the next column of the reduction
                                                
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void lubksb(MIODP &a, VOi &indx, VIODP &b)
{
    int i,ii=0,ip,j;
    DP sum;
    
    int n=a.nrows();
    
    for (i=0;i<n;i++) {         // When ii is set to a positive value, it will become the
                                // index of the first nonvanishing element of b. We now do
                                // do the forward substitution, equation 2.3.6. The only 
                                // new wrinkle is to unscramble the permutation as we go
        ip=indx[i];
        sum=b[ip];
        b[ip]=b[i];
        if (ii != 0)
            for (j=ii-1;j<i;j++) sum -= a[i][j]*b[j];
        else
            if (sum != 0.0)     // A non zero element was encountered, so from now on we will
                                // have to do the sums in the loop above
                ii=i+1;
        b[i]=sum;
    }
    
    for (i=n-1;i>=0;i--) {      // Now we do the backsubstitution, equation 2.3.7
        sum=b[i];
        for (j=i+1;j<n;j++)  sum -= a[i][j]*b[j];
        b[i]=sum/a[i][i];       // Store a component of the solution vector X
    }                           // All done!
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void savgol(VODP &c, const int np, const int nl, const int nr, const int ld, const int m)
{
    int j,k,imj,ipj,kk,mm;
    DP d,fac,sum;
    
    if (np < nl+nr+1 || nl < 0 || nr < 0 || ld > m || nr+nr < m) cerr<<"Something wrong with input parameters"<<endl;
    
    Vi indx(m+1);
    MDP a(m+1,m+1);
    VDP b(m+1);
    
    for (ipj=0; ipj<=(m << 1); ipj++)  {  // Set the normal equations of the desired least-squared fit 
        sum=(ipj ? 0.0 : 1.0 );
        for (k=1; k<=nr; k++) sum += pow(DP(k),DP(ipj));
        for (k=1; k<=nl; k++) sum += pow(DP(-k),DP(ipj));
        mm=minimum(ipj,2*m-ipj);
        for (imj=-mm; imj<=mm; imj+=2) a[(ipj+imj)/2][(ipj-imj)/2]=sum;
    }
    
    
    ludcmp(a,indx,d);   // Solve them by LU decomposition
    
    for (j=0; j<m+1; j++) b[j]=0.0;
    b[ld]=1.0;
    // Right hand side is unit vector, depending on which derivative we want
    lubksb(a,indx,b);   // Get one row of the inverse matrix
    for (kk=0; kk<np; kk++) c[kk]=0.0;  // Zero output array (it may be bigger than number of coefficients)
    for (k=-nl; k<=nr; k++) {   // Each Savitzky-Golay coefficient is the dot product power of an integer with the inverse matrix row
        sum=b[0];
        fac=1.0;
        for (mm=1; mm<=m; mm++) sum += b[mm]*(fac *= k);
        kk=(np-k) % np;         // Store in wrap-around order
        c[kk]=sum;
    }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void gaussjordan(MIODP &a, MIODP &b)
{
	int i,icol,irow,j,k,l,ll;
	DP big,dum,pivot;

        int n=a.nrows();
        int m=b.ncols();
	// These integer arrays (ipivot, indxc, indxr) are used for bookkeeping on the pivoting
	Vi indxc(n), indxr(n), ipivot(n);

	for (j=0;j<n;j++) ipivot[j]=0;  // Inizialization...
	for (i=0;i<n;i++) {        // This is the main loop over the columns to be reduced
		big=0.0;
		for(j=0;j<n;j++)      // This is the outer loop of the search for a pivot element
			if(ipivot[j] != 1)
				for (k=0;k<n;k++)  {
					if(ipivot[k] == 0)  {
						if (fabs(a[j][k]) >= big) {  // Search for the biggest coefficient... the pivot
							big=fabs(a[j][k]);
							irow=j;					// Coordinates of the current pivot element
							icol=k;
						}
					}
				}
		++(ipivot[icol]);

		// The pivot element was found and the corresponding row and columns are in irow and icol.
		// Now we interchange rows, if needed, to put the pivot element on the diagonal.
		// The columns are not physically interchanged, only relabeled: indxc[i], the column of the (i+1)th pivot element,
		// is the (i+1)th column that is reduced, while indxr[i] is the row in which that pivot element was originally located.
		// If indxr[i]!=indxc[i] there is an implied column interchange. With this form of bookkeeping, the solution
		// b's will end up in the correct order, and the inverse matrix will be scrambled by columns.

		if (irow != icol)  {    // If the pivot is not in the diagonal...
			for (l=0;l<n;l++) swapT(a[irow][l],a[icol][l]);
			for (l=0;l<m;l++) swapT(b[irow][l],b[icol][l]);
		}

		indxr[i]=irow;                 // We are now ready to divide the pivot row by the pivot element, located in irow and icol
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) cerr<<"Error in Gauss-Jordan elimination method: Singular matrix"<<endl;
		pivot=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=0;l<n;l++) a[icol][l] *= pivot;
		for (l=0;l<m;l++) b[icol][l] *= pivot;

		for (ll=0;ll<n;ll++)          // Next we reduce the rows...
			if (ll != icol)  {         // ...except for the pivot one, of course.
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=0;l<n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=0;l<m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}

    // This is the end of the main loop over columns of the reduction. It only remains to uncramble the solution in view of the
    // columns interchanges. This can be done by interchanging pairs of columns in the reverse order that the permutation was built up.

    for (l=n-1;l>=0;l--)  {
		if (indxr[l] != indxc[l])
			for (k=0;k<n;k++)
				swapT(a[k][indxr[l]],a[k][indxc[l]]);
	}                     // and we are done
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void covarsrt(MIODP &ccovar, VIb &ia, const int mfit)
{
	int i,j,k;

        int ma=ia.size();
	for (i=mfit;i<ma;i++)
		for (j=0;j<i+1;j++)  ccovar[i][j]=ccovar[j][i]=0.0;
	k=mfit-1;
	for (j=ma-1;j>=0;j--)  {
		if (ia[j])  {
			/// WARNING!!! The next 3 lines must be checked, could be wrong!!
			for (i=0;i<ma;i++) swapT(ccovar[i][k],ccovar[i][j]);
			for (i=0;i<ma;i++) swapT(ccovar[k][i],ccovar[j][i]);
			k--;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void mrqcof(VIDP &x, VIDP &y, VIDP &sigma, VIODP &a, VIb &ia, MIODP &alpha, 
        VODP &beta, DP &chisq, void funcs(const DP, VIDP &, DP &, VODP &))
{

	int i,j,k,l,m,mfit=0;
	DP ymod,wt,sigma2i,dy;

        int ndata=x.size();
        int ma=a.size();
        VDP dyda(ma);

	for (j=0;j<ma;j++)
		if (ia[j]) mfit++;             // Evaluate how many parameters must be optimized (mfit)
	for (j=0;j<mfit;j++)  {                         /// Initialize (symmetric) alpha, beta to zero
		for (k=0;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	chisq=0.0;                      		// Initialize the chi square to zero
	for (i=0;i<ndata;i++)  {                      /// Summation loop over all data
            funcs(x[i],a,ymod,dyda);
            sigma2i=1.0/(sigma[i]*sigma[i]);
            dy=y[i]-ymod;
            for (j=0,l=0;l<ma;l++)  {                   // Calculate the elements of the alpha matrix...
		if (ia[l])  {
                    wt=dyda[l]*sigma2i;
                    for (k=0,m=0;m<l+1;m++)
			if (ia[m]) alpha[j][k++]  += wt*dyda[m];
                        beta[j++]  +=  dy*wt;					// ... and of beta vector
                    }
		}
                chisq += dy*dy*sigma2i;                    // and calculate the chi squared
	}
	for (j=1;j<mfit;j++)                          // Fill the simmetric side
		for (k=0;k<j;k++) alpha[k][j]=alpha[j][k];

        chisq = chisq/ndata;
	////////////////////////////////////////////////////////////////////////////////
	// Just for testing....
	for (i=0;i<mfit;i++) cout<<"Beta vector element "<<i<<"="<<beta[i]<<endl;

	for (i=0;i<mfit;i++)
		for (j=0;j<mfit;j++)
			cout<<"Alpha matrix element ("<<i<<','<<j<<")= "<<alpha[i][j]<<endl<<endl;
	////////////////////////////////////////////////////////////////////////////////

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void marquardt(VIDP &x, VIDP &y, VIDP &sigma, VIODP &a, VIb &ia, MIODP &covar,
        MIODP &alpha, DP &chisq, void funcs(const DP, VIDP &, DP &, VODP &),
        DP &lambda)
{
	int i,j,k,l;
	static int mfit;
	static DP ochisq;

        int ma=a.size();
        static MDP oneda(ma,1);
        static VDP atemp(ma),beta(ma),da(ma);
	if (lambda<0.0)  {              // If lambda is negative, then initialize...
		for (mfit=0,j=0;j<ma;j++)
			if (ia[j]) mfit++;       // Counts how many parameters must be fitted (ia!=0)
		lambda=0.001;               // Initial value of lambda, for the first iteration
		mrqcof(x,y,sigma,a,ia,alpha,beta,chisq,funcs);
		ochisq=chisq;
		for (j=0;j<ma;j++) atemp[j]=a[j];
	}
        MDP temp(mfit,mfit);
	for (j=0;j<mfit;j++)   {      // Alter linearized fitting matrix, by augmenting diagonal elements
		for (k=0;k<mfit;k++) covar[j][k]=alpha[j][k];
		covar[j][j]=alpha[j][j]*(1.0+lambda);
                for (k=0;k<mfit;k++) temp[j][k]=covar[j][k];
		oneda[j][0]=beta[j];
	}

	gaussjordan(temp,oneda);      // Matrix solution by the Gauss-Jordan method.

	for (j=0;j<mfit;j++) {
            for (k=0;k<mfit;k++) covar[j][k]=temp[j][k];
                da[j]=oneda[j][0];
        }

	if (lambda == 0.0)  {				 // Once converged, evaluate covariance matrix
		covarsrt(covar,ia,mfit);
		covarsrt(alpha,ia,mfit);		// Spread out alpha to its full size too
		return;
	}

	for (j=0,l=0;l<ma;l++)         // Did the trial succeed?
		if (ia[l]) atemp[l]=a[l]+da[j++];

        for (i=0;i<mfit;i++)
		for (j=0;j<mfit;j++)
			cout<<"Covar matrix element ("<<i<<','<<j<<")= "<<covar[i][j]<<endl<<endl;

	mrqcof(x,y,sigma,atemp,ia,alpha,beta,chisq,funcs);

        for (i=0;i<mfit;i++)
		for (j=0;j<mfit;j++)
			cout<<"Covar matrix element ("<<i<<','<<j<<")= "<<covar[i][j]<<endl<<endl;

	if (chisq < ochisq)  {        // Success, accept the new solution
		lambda *= 0.1;
		ochisq=chisq;
		for (j=0;j<mfit;j++)  {
			for (k=0;k<mfit;k++) alpha[j][k]=covar[j][k];
			beta[j]=da[j];
		}
                for (l=0;l<ma;l++) a[l]=atemp[l];
	}  else  {                     // Failure, increase lambda and return
		lambda *= 10.0;
		chisq=ochisq;
//                for (l=0;l<ma;l++) atemp[l]=a[l];
	}
    
       for (l=0;l<ma;l++) cout<<"a["<<l<<"] = "<<atemp[l]<<endl;
}
