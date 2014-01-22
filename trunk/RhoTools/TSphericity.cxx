//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TSphericity.cc,v 1.2 1997/1/20 10:05:13 
//
// Description:
//	Class TSphericity - a simple object to find Sphericity Axis and Sphericity
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Sebong Chun     
//      Dan Azzopardi - put in check for case of no tracks, avoid fpe          //
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TSphericity.h"
#include <assert.h>
#include <math.h>
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TEventInfo.h"

ClassImp(TSphericity)

TBuffer &operator>>(TBuffer &buf, TSphericity *&obj)
{
   obj = (TSphericity *) buf.ReadObject(TSphericity::Class());
   return buf;
}

#include <iostream>
using namespace std;

static void jacobi(TMatrixD Source, TMatrixD & RotationMatrix, TVector & EigenValues);


inline void swap(int& v1, int& v2) 
{
    int temp = v1;
    v1 = v2;
    v2 = temp;
}


//----------------
// Constructors --
//----------------

TSphericity::TSphericity(TCandList& list, 
			 const TEventInfo& evtinfo,
			 Bool_t chargedTracksOnly) 
			 : _E(3)
{
    TMatrixD sph_mtrx(3,3);
    TMatrixD Rot(3,3);
    TVector E(3); 
    const TCandidate* trkptr;
    
    TCandListIterator iter(list);
    double stot=0, sxx=0, syy=0, szz=0, sxy=0, sxz=0, syz=0;
    //double sxtot=0, sytot=0, sztot=0;
    
    // start a loop over the all tracks
    while ( (trkptr = iter.Next()) ) {
	
        TLorentzVector cmList(trkptr->P4());
	TVector3 cms = evtinfo.CmFrame().BoostVector();
	cmList.Boost( -(cms));
	
	// Added chargedTracksOnly parameter to make it possible to compute
	// these variables from neutral tracks, too.  -- Alex Samuel, 9/97
	if ( !chargedTracksOnly || trkptr->Charge() != 0 )
	{
	    //I left followings in case I need to calculate sphericity axis in Lab frame       
	    //stot += (trkptr->p())*(trkptr->p());
	    //sxx += (trkptr->p3()).x()*(trkptr->p3()).x();
	    //sxy += (trkptr->p3()).x()*(trkptr->p3()).y();
	    //sxz += (trkptr->p3()).x()*(trkptr->p3()).z();
	    //syy += (trkptr->p3()).y()*(trkptr->p3()).y();
	    //syz += (trkptr->p3()).y()*(trkptr->p3()).z();
	    //szz += (trkptr->p3()).z()*(trkptr->p3()).z();
	    
	    TVector3 tmp(cmList.X(),cmList.Y(),cmList.Z());
	    stot += (tmp.Mag())*(tmp.Mag());
	    sxx += (tmp.X())*(tmp.X());
	    sxy += (tmp.X())*(tmp.Y());
	    sxz += (tmp.X())*(tmp.Z());
	    syy += (tmp.Y())*(tmp.Y());
	    syz += (tmp.Y())*(tmp.Z());
	    szz += (tmp.Z())*(tmp.Z());
        }
    } 
    
    // The following is sufficent for avoiding division by zero.
    if (stot==0.) stot = 1.;
    
    //obtain sphericity tensor explicitly
    sph_mtrx(0,0) = sxx / stot ;
    sph_mtrx(1,0) = sxy / stot ;
    sph_mtrx(2,0) = sxz / stot ;
    sph_mtrx(0,1) = sxy / stot ;
    sph_mtrx(1,1) = syy / stot ;
    sph_mtrx(2,1) = syz / stot ;
    sph_mtrx(0,2) = sxz / stot ;
    sph_mtrx(1,2) = syz / stot ;
    sph_mtrx(2,2) = szz / stot ;
    
    //test "jacobi" with an explicit matrix
    //sph_mtrx[0][0] = 1 ;
    //sph_mtrx[1][0] = 1 ;
    //sph_mtrx[2][0] = 1 ;
    //sph_mtrx[0][1] = 1 ;
    //sph_mtrx[1][1] = 1 ;
    //sph_mtrx[2][1] = 1 ;
    //sph_mtrx[0][2] = 1 ;
    //sph_mtrx[1][2] = 1 ;
    //sph_mtrx[2][2] = 1 ;
    
    jacobi(sph_mtrx,Rot,E);
    
    _E = E;
    
    double Emax = E(0);
    double Emin = E(0);
    int j = 0, k = 0;
    for ( int i = 0; i <= 2 ; ++i ) {
	if ( E(i) > Emax ) {Emax = E(i); j = i;}
	if ( E(i) < Emin ) {Emin = E(i); k = i;}
    }
    
    int l = 3-j-k;
    _sph = Emax;
    
    _sph = 3 * ( E(k) + E(l) ) / 2;
    _apl = 3 * E(k) / 2;
    _pla = E(l) - E(k);
    
    // sort the eignevalues into increasing order; rather than rearranging
    // the values themselves, sort an array of indices so that the
    // correspondence with eigenvectors may be retained
    
    int ei[3] = { 0, 1, 2 };
    if(E(ei[1]) < E(ei[0])) swap(ei[0], ei[1]);
    if(E(ei[2]) < E(ei[1])) swap(ei[1], ei[2]);
    if(E(ei[1]) < E(ei[0])) swap(ei[0], ei[1]);
    
    _sph = 1.5 * (E(ei[0]) + E(ei[1]));
    _apl = 1.5 * E(ei[0]);
    _pla = E(ei[1]) - E(ei[0]);
    
    _sphAx = TVector3 ( Rot(0,ei[2]), Rot(1,ei[2]), Rot(2,ei[2]) ) ;
    _secAx = TVector3 ( Rot(0,ei[1]), Rot(1,ei[1]), Rot(2,ei[1]) ) ;
    _thrAx = TVector3 ( Rot(0,ei[0]), Rot(1,ei[0]), Rot(2,ei[0]) ) ;
    
}

//--------------
// Destructor --
//--------------

TSphericity::~TSphericity() {}

//-------------------
//Member Function ---
//-------------------


/*-------------------------------------------------*/
/*         Jacobi                                  */
/* Template function jacobi computes eigenvalues   */
/* and eigenvectors of a real symmetric matrix     */
/* The output consists of the linear tranformation */
/* to the principal axes and a vector containing   */
/* the matrice's eigenvalues.                      */
/*-------------------------------------------------*/

#define JACOBI_ROTATION(A,i,j,k,l) g=A(i,j); h=A(k,l); A(i,j) = g-s*(h+g*tau); A(k,l) = h + s*(g-h*tau);

static void jacobi(TMatrixD Source, TMatrixD & RotationMatrix, TVector & EigenValues) {
    
    //  AS IN NUMERICAL RECIPES IN C, PAGE 467
    
    int irow,icol;
    double Threshold;
    
    int mtrxSize=Source.GetNrows();
    RotationMatrix =TMatrixD(mtrxSize,mtrxSize);
    int i,j;
    for (i = RotationMatrix.GetRowLwb(); i <= RotationMatrix.GetRowUpb(); i++)
	for(j = RotationMatrix.GetColLwb(); j <= RotationMatrix.GetColUpb(); j++)
	    RotationMatrix(i,j) = 1.0;
	for (irow=0;irow<mtrxSize;irow++) EigenValues(irow)=Source(irow,irow);
	TVector Z(mtrxSize),B(mtrxSize);
	B=EigenValues;
	int NumberRotations = 0;
	for (int Iteration=0;Iteration<50;Iteration++) {
	    double SumOffDiagonal=0.0;     
	    for (irow=0;irow<mtrxSize;irow++) {
		for (icol=irow+1;icol<mtrxSize;icol++) {
		    SumOffDiagonal+= fabs(Source(irow,icol));
		}
	    }
	    if (SumOffDiagonal==0.0) break;
	    if (Iteration<4) 
		Threshold = 0.2*SumOffDiagonal/(mtrxSize*mtrxSize);
	    else
		Threshold = 0.0;
	    for (irow=0;irow<mtrxSize;irow++) {
		for (icol=irow+1;icol<mtrxSize;icol++) {
		    double t,g;
		    g = 100.0*fabs(Source(irow,icol));
		    if (Iteration>4 && (double) (fabs(EigenValues(irow))+g) == (double) fabs(EigenValues(irow)) && (double) (fabs(EigenValues(icol))+g) == (double) fabs(EigenValues(icol))) {
			Source(irow,icol)=0.0;
		    }
		    else if (fabs(Source(irow,icol))>Threshold) {
			double h = EigenValues(icol) - EigenValues(irow);
			if ((double) (fabs(h) + g) == (double) fabs(h)) {
			    t = Source(irow,icol)/h;
			}
			else {
			    double Theta = 0.5*h/Source(irow,icol);
			    t = 1.0/(fabs(Theta) + sqrt(1.0 + Theta*Theta));
			    if (Theta<0) t = -t; 
			}
			double c = 1.0/sqrt(1.0+t*t);
			double s = t*c;
			double tau = s/(1.0 + c);
			h = t*Source(irow,icol);
			Z(irow) -= h;
			Z(icol) += h;
			EigenValues(irow) -= h;
			EigenValues(icol) += h;
			Source(irow,icol) = 0.0;
			
			int j;
			for (j=0;j<irow;j++) {
			    JACOBI_ROTATION(Source,j,irow,j,icol)
			}
			for (j=irow+1;j<icol;j++) {
			    JACOBI_ROTATION(Source,irow,j,j,icol)
			}
			for (j=icol+1;j<mtrxSize;j++) {
			    JACOBI_ROTATION(Source,irow,j,icol,j)
			}
			for (j=0;j<mtrxSize;j++) {
			    JACOBI_ROTATION(RotationMatrix,j,irow,j,icol)
			}
			NumberRotations++;
		    }
		}            
	    }    
	    B+=Z;
	    EigenValues=B;
	    Z=TVector(mtrxSize);
	}
	//Sort not needed in this application and it is buggy
	/*  HepMatrix ColJ(1,mtrxSize);
	for (int i=1;i<=mtrxSize;i++) {
	for (int j=i+1;j<=mtrxSize;j++) {
	if (EigenValues[j]>EigenValues[i]) {
	ColJ=RotationMatrix.sub(1,mtrxSize,i,i);
	RotationMatrix.sub(1,i,RotationMatrix.sub(1,mtrxSize,j,j));
	RotationMatrix.sub(1,j,ColJ);
	double fI = EigenValues[i], fJ=EigenValues[j];
	EigenValues[i] = fJ; EigenValues[j] = fI;
	}
	}
}*/
}
