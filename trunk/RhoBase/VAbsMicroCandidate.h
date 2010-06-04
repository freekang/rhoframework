#ifndef VABSMICROCANDIDATE_H
#define VABSMICROCANDIDATE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsMicroCandidate	                                                //
//                                                                      //
// Definition of an abstract interface to a micro candidate.	        //
//                                                                      //
// Author: Sascha Berger and Marcel Kunze, RUB, March 1999		//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <assert.h>

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class VAbsPidInfo;

//  ========================================================================
//  ===== VAbsMicroCandidate - Abstract Class definig the AOD interface  ====
//  ========================================================================

class VAbsMicroCandidate : public TObject
{

  public:

    VAbsMicroCandidate() {}
    virtual ~VAbsMicroCandidate() {}

    // basic properties:

    virtual Int_t    GetCharge() const =0;
    virtual TVector3 GetPosition() const =0;
    virtual TVector3 GetMomentum() const =0;
    virtual TLorentzVector GetLorentzVector() const =0;
    virtual Float_t  GetEnergy() const =0;
    virtual const Float_t* GetParams() =0;
    virtual const Float_t* GetPositionError() =0;
    virtual const Float_t* GetMomentumError() =0;
    virtual const Float_t* GetErrorP7() =0;
    virtual const Float_t* GetCov() =0;
    virtual Int_t    GetMcPid() const =0;
    virtual Int_t    GetMcIndex() const =0;
    virtual Float_t  GetMcConsistency() const =0;
    //virtual Int_t   GetSubObjectStatus() const = 0; // 8.3.x

    // calorimeter properties:

    virtual Bool_t   EmcOk() const =0; // Obsolete since 8.2.x
    virtual Bool_t   IfrOk() const =0; // Obsolete since 8.2.x

    virtual Int_t    GetEmcStatus() const =0; // 8.6.x
    virtual Int_t    GetEmcNumberOfCrystals() const =0;
    virtual Int_t    GetEmcNumberOfBumps() const =0;
    virtual Float_t  GetEmcRawEnergy() const =0;
    virtual Float_t  GetEmcCalEnergy() const =0;
    virtual Float_t  GetEmcCovarianceEnergy() const =0;
    virtual Float_t  GetEmcCovarianceTheta() const =0;
    virtual Float_t  GetEmcCovariancePhi() const =0;
    virtual Float_t  GetEmcCovarianceRho() const =0;
    virtual Float_t  GetEmcS9S25() const =0;
    virtual Float_t  GetEmcS1S9() const =0;
    virtual Float_t  GetEmcLateralShape() const =0;
    virtual Float_t  GetEmcSecondMoment() const =0;
    virtual Float_t  GetEmcZernikeMoment20() const =0;
    virtual Float_t  GetEmcZernikeMoment31() const =0;
    virtual Float_t  GetEmcZernikeMoment42() const =0;
    virtual TVector3 GetEmcCentroid() const =0;
    virtual Float_t  GetEmcConsistencyValue() const  =0;
    Float_t  GetEmcTrackMatchConsistency() { return GetEmcConsistencyValue(); }
    virtual Float_t  GetEmcConsistencyLikelihood() const  =0;
    virtual Int_t    GetEmcConsistencyBits() const  =0; // 8.3.x
    virtual Int_t    GetNeutralPidHypo() const =0;

    virtual Int_t    GetIfrPattern() const =0;
    virtual Int_t    GetIfrHitLayers() const =0; // Obsolete
    virtual Int_t    GetIfrNumberOfStrips() const =0; // Calculation
    virtual Int_t    GetIfrNumberOfStrips(Int_t layer) const =0;
    virtual Int_t    GetIfrFirstHit() const =0; // Calculation
    virtual Int_t    GetIfrLastHit() const =0; // Calculation
    //virtual Int_t    GetIfrClusters() const =0; // Obsolete since 8.2.x
    virtual Float_t  GetIfrMeasuredInteractionLengths() const =0; // 8.3.x
    virtual Float_t  GetIfrExpectedInteractionLengths() const =0; // 8.3.x
    virtual Float_t  GetIfrInteractionLengthsBeforeIron() const  =0;
    virtual Float_t  GetIfrTrackMatch() const =0;
    Float_t  GetIfrTrackMatchConsistency() { return GetIfrTrackMatch(); }
    virtual Float_t  GetIfrEmcMatch() const =0;
    Float_t  GetIfrEmcMatchConsistency() { return GetIfrEmcMatch(); }
    virtual Float_t  GetIfrClusterFit() const =0; // 8.6.x
    virtual Float_t  GetIfrRpcMuonProbability() const =0; // 8.6.x
    virtual Int_t    GetIfrLastBarrel() const =0; // 8.3.x

    // track properties:

    virtual Bool_t   SvtOk() const =0; // Obsolete since 8.2.x
    virtual Bool_t   DchOk() const =0; // Obsolete since 8.2.x
    virtual Bool_t   DrcOk() const =0; // Obsolete since 8.2.x

    virtual Float_t  GetTrackLength() const =0;
    virtual Float_t  GetTrackStartFoundRange() const =0; // 8.3.x

    virtual Int_t    GetSvtHitPattern() const =0;
    virtual Int_t    GetSvtHits() const =0;
    virtual Int_t    GetDchInnerLayer() const =0;
    virtual Int_t    GetDchOuterLayer() const  =0;
    virtual Int_t    GetDchHits() const =0;
    virtual Int_t    GetChargedPidHypo() const =0;
    virtual Int_t    GetDegreesOfFreedom() const =0;
    virtual Int_t    GetFitStatus() const =0;
    virtual Float_t  GetProbability() const =0;
    virtual Float_t  GetChiSquared() const =0;

    virtual Float_t  GetDrcThetaC() const =0;
    virtual Float_t  GetDrcThetaCError() const =0;
    //virtual Float_t  GetDrcRingProbability() const  =0; // Obsolete since 8.3.x
    //virtual Int_t    GetDrcNumberDegreesOfFreedom() const  =0; // Obsolete since 8.3.x
    virtual Int_t    GetDrcNumberOfPhotons() const =0;
    virtual Int_t    GetDrcNumberOfBackgroundPhotons() const =0; // 8.3.x
    virtual Int_t    GetDrcNumberOfExpectedPhotons(Int_t hypo) const =0; // 8.3.x
    virtual Float_t  GetDchDeltaMomentum() const =0;
    virtual Float_t  GetDrcDeltaMomentum() const =0; // 8.6.x
    virtual Int_t    GetDrcInBar() const =0; // 8.6.x
    virtual Int_t    GetDrcExitBar() const =0; // 8.6.x
    virtual Float_t  GetDrcXPos() const =0; // 8.6.x
    virtual Float_t  GetDrcPhiAtEmc() const =0;  // 8.6.x
    virtual Float_t  GetDrcThetaAtEmc() const =0; // 8.6.x
    virtual Float_t  GetDchMeanDEdx() const =0;
    virtual Float_t  GetSvtMeanDEdx() const =0;
    virtual Int_t    GetDchSamples() const =0;
    virtual Int_t    GetSvtSamples() const =0;

    // Access to PID information
    virtual VAbsPidInfo& GetDrcPidInfo() const =0;
    virtual VAbsPidInfo& GetDchPidInfo() const =0;
    virtual VAbsPidInfo& GetSvtPidInfo() const =0;
    virtual VAbsPidInfo& GetEmcPidInfo() const =0;
    virtual VAbsPidInfo& GetIfrPidInfo() const =0;

    // Modifiers (for event display)

    virtual void     SetPosition(TVector3 x) =0;
    virtual void     SetMcPid(Int_t pid) =0;
    virtual void     SetMcIndex(Int_t index) =0;
    virtual void     SetMcConsistency(Float_t cons) =0;

    // virtuals     
    
    virtual void     PrintOn( std::ostream& o=std::cout ) const;

    ClassDef(VAbsMicroCandidate,1) // Abstract base class for MicroDST candidates
};

std::ostream&  operator << (std::ostream& o, const VAbsMicroCandidate&);

#endif                                           


