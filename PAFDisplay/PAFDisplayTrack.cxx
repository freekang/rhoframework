//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PAFDisplayTrack                                                      //
//                                                                      //
// Graphics interface to micro DST		                        //
// Adapted from ALIROOT (M.Kunze, June 1999)				//
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "TVirtualX.h"
#include "TROOT.h"
#include "TText.h"
#include "TPolyLine3D.h"
#include "TMarker3DBox.h"
#include "TCanvas.h"
#include "TList.h"
#include "TMath.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "PAFDisplay/PAFDisplayTrack.h"

const Int_t kRECONS = BIT(16);

ClassImp(PAFDisplayTrack)


//_____________________________________________________________________________
PAFDisplayTrack::PAFDisplayTrack(TCandidate *a, const char* name, Double_t length) : 
fIndex(0), TNamed(name,name), fPid(0),
fScale(0.0017), fTextColor(kWhite), fTextSize(0.025)
{
    fMicro = &a->GetMicroCandidate();
    if (a->PdtEntry() != 0) fPid = a->PdtEntry()->PdgCode();
    fPosition = a->GetPosition();
    fMomentum = a->P3();
    fObjs = new TList;
    fLength = length;
}

//_____________________________________________________________________________
PAFDisplayTrack::~PAFDisplayTrack()
{
    if (fObjs != 0) fObjs->Delete();
    delete fObjs;
}

//_____________________________________________________________________________
Int_t PAFDisplayTrack::DistancetoPrimitive(Int_t px, Int_t py)
{
    Int_t dist = fLine->DistancetoPrimitive(px, py);
    if (dist < 2) {
	gPad->SetSelected(this);
	gPad->SetCursor(kCross);
	return 0;
    }
    return 999;
}

//______________________________________________________________________________
void PAFDisplayTrack::ExecuteEvent(Int_t event, Int_t , Int_t )
{
    switch (event) {
	
    case kButton1Down:
	gVirtualX->SetLineColor(-1);
	gPad->AbsCoordinates(kTRUE);
	fLine->SetLineColor(6);
	fLine->SetLineWidth(6);
	fLine->Paint(); 
	break;
	
    case kMouseMotion:
	break;
	
    case kButton1Motion:
	break;
	
    case kButton1Up:
	gVirtualX->SetLineColor(-1);
	fLine->SetLineColor(kYellow);
	fLine->SetLineWidth(1);
	fLine->Paint(); 
	gPad->AbsCoordinates(kFALSE);
    }
}

//______________________________________________________________________________
char *PAFDisplayTrack::GetObjectInfo(Int_t , Int_t ) const
{
    static char info[100];
    sprintf(info,"px=%f, py=%f, pz=%f, E=%f",
	fMicro->GetMomentum().X(),
	fMicro->GetMomentum().Y(),
	fMicro->GetMomentum().Z(),
	fMicro->GetEnergy() );
    return info;
}

//______________________________________________________________________________
TPolyLine3D *PAFDisplayTrack::HelixCurve(Float_t field, Float_t pmom, Float_t *vin, 
					   Float_t rin, Float_t zout, Float_t totlen)
{
    //    Estimate step size in function of field.
    //    Create a 3-D polyline with points computed with this step size
    
    Float_t step = 10;
    const Int_t kMAXSTEP = 500;
    Float_t sx[kMAXSTEP], sy[kMAXSTEP], sz[kMAXSTEP];
    if (pmom > 0)   step = pmom;
    if (step > 100) step = 100;
    if (step < 0.2) step = 0.2;
    
    Float_t vout[6];
    Int_t i,j;
    sx[0] = vin[0];
    sy[0] = vin[1];
    sz[0] = vin[2];
    Int_t nsteps = 1;
    Float_t length = 0.0;

    for (i=1;i<kMAXSTEP;i++) {

	HelixStep(field,step,pmom,vin,vout); // Propagate the track

	if (TMath::Abs(vout[2]) > zout) break;
	if (vout[0]*vout[0] + vout[1]*vout[1] > rin*rin) break;
	for (j=0;j<6;j++) vin[j] = vout[j];

	length += step;	// Check the visible track length
	if (length<totlen) {
	    sx[nsteps] = vout[0];
	    sy[nsteps] = vout[1];
	    sz[nsteps] = vout[2];
	    nsteps++;
	}

    }

    if (nsteps < 2) return 0; // Not a line

    TPolyLine3D *line = new TPolyLine3D(nsteps,sx, sy,sz); // The visible track
    line->SetBit(kCanDelete);

    return line;
}

//______________________________________________________________________________
void PAFDisplayTrack::HelixStep(Float_t field, Float_t step, Float_t pmom, Float_t *vin, Float_t *vout)
{
    //     extrapolate track with parameters in vector vin in a constant field
    //     oriented along Z axis (in tesla/meters).
    //     Output in vector vout
    //     vin[0-->6] = x,y,z,px,py,pz
    //     translated to C++ from GEANT3 routine GHELX3
    
    Float_t sint, sintt, tsint, cos1t, sin2;
    //      units are tesla,centimeters,gev/c
    const Float_t ec = 2.9979251e-3;
    Float_t h4  = field*ec;
    Float_t hp  = vin[2];
    Float_t tet = -h4*step/pmom;
    if (TMath::Abs(tet) > 0.15) {
	sint  = TMath::Sin(tet);
	sintt = sint/tet;
	tsint = (tet-sint)/tet;
	sin2  = TMath::Sin(0.5*tet);
	cos1t = 2*sin2*sin2/tet;
    } else {
	tsint = tet*tet/6;
	sintt = 1 - tsint;
	sint  = tet*sintt;
	cos1t = 0.5*tet;
    }
    Float_t f1 = step*sintt;
    Float_t f2 = step*cos1t;
    Float_t f3 = step*tsint*hp;
    Float_t f4 = -tet*cos1t;
    Float_t f5 = sint;
    Float_t f6 = tet*cos1t*hp;
    
    vout[0] = vin[0] + (f1*vin[3] - f2*vin[4]);
    vout[1] = vin[1] + (f1*vin[4] + f2*vin[3]);
    vout[2] = vin[2] + (f1*vin[5] + f3);
    
    vout[3] = vin[3] + (f4*vin[3] - f5*vin[4]);
    vout[4] = vin[4] + (f4*vin[4] + f5*vin[3]);
    vout[5] = vin[5] + (f4*vin[5] + f6);
}

//_____________________________________________________________________________
void PAFDisplayTrack::SetMarker(Int_t color,Float_t x,Float_t y,Float_t z,Float_t dx,Float_t dy,Float_t dz,Float_t theta,Float_t phi)
{
    TMarker3DBox *m = new TMarker3DBox(x,y,z,dx,dy,dz,theta,phi);
    if (m == 0) return;
    m->SetUniqueID(fIndex++);
    m->SetLineColor(color);	    
    fObjs->Add(m);
}

//_____________________________________________________________________________
void PAFDisplayTrack::DrawTrack(Option_t *option, Bool_t useCache)
{
    //    Draw the micro information (tracking, DRC, EMC, IFR)
    //    Particle trajectory is computed along an helix in a constant field
 
    if (useCache) {
	TIter next(fObjs);
	TObject *item;
	while(item=next()) {
	    item->Draw(option);
	}
	return;
    }
    else {
	fObjs->Delete();
    }
    
    Float_t pmom, energy, vx, vy, vz, pt;
    Float_t vin[6];
    Float_t field = TRho::Instance()->GetMagnetField();
    Int_t charge;
    
    charge = fMicro->GetCharge();
    TVector3 P = fMomentum;
    TVector3 R = fPosition;

    pt   = P.Perp();
    pmom = P.Mag();
    
    energy = fMicro->GetEmcRawEnergy();		    // Get cluster energy
    
    vx = R.X();
    vy = R.Y();
    vz = R.Z();
    vin[0] = vx;				    // Vertex
    vin[1] = vy;
    vin[2] = vz;
    vin[3] = P.X()/pmom;			    // Unit momentum vector
    vin[4] = P.Y()/pmom;
    vin[5] = P.Z()/pmom;

    if (charge != 0 && fLength <= 0.0) fLength = fMicro->GetTrackLength();
    if (fLength <= 0.0) fLength = 500.; // Defaults to 5 m

    fLine = HelixCurve(charge*field, pmom, vin, 110., 220.,fLength);
    if (fLine == 0) return;

    fLine->SetUniqueID(fIndex++);
    fLine->SetLineStyle(1);
    Int_t width = (Int_t) (1.0 + pmom); // Draw track fat with momentum
    fLine->SetLineWidth(width);

    if (charge == 0) {
	fLine->SetLineColor(kCyan);
    }
    else {
	switch (TMath::Abs(fPid)) {
	case 11:  fLine->SetLineColor(kGreen); break;   // Electron
	case 13:  fLine->SetLineColor(kBlue); break;    // Muon
	case 211:  fLine->SetLineColor(kRed); break;     // Pion
	case 321:  fLine->SetLineColor(kMagenta); break; // Kaon
	case 2212:  fLine->SetLineColor(kYellow); break;  // Proton
	default: fLine->SetLineColor(TMath::Abs(fPid));
	}
    }

    fObjs->Add(fLine);
    
    
    // Add DRC info to the track
    // Draw a blue bar
    
    if (charge != 0) {
	
	Float_t nphot  = fMicro->GetDrcNumberOfPhotons();
	Float_t thetac = fMicro->GetDrcThetaC();
	if (nphot == 0 && thetac > 0.0) nphot = 25; // For 8.2.x micro
	if (nphot > 0) {
	    
	    Float_t x = vin[0];			// End of track
	    Float_t y = vin[1];
	    Float_t z = 0;			// Center in the middle of detector
	    Float_t r = TMath::Sqrt(x*x+y*y);	// Adjust distance
	    x *= 85./r;
	    y *= 85./r;
	    Float_t phi = TMath::ATan(y/x) * 180./3.1415962;;
	    
	    Float_t d = TMath::Sqrt(nphot);	// Thickness ~ sqrt(nphot)
	    Float_t dx = d;
	    Float_t dy = d;
	    Float_t dz = 183.75;		// Length of bar
	    
	    SetMarker(kBlue,x,y,z,dx,dy,dz,0.0,phi);	    
	}
    }
    
    // Draw the particle name
    TString name = this->GetName();
    if (name != "" && fScale > 0.0) {      
      Bool_t greek = kFALSE;
      if (name.Index("gamma") == 0) 
	{ name = "g"; greek = kTRUE; }
      else if (name.Index("pi0") == 0)  
	{ name = "p0"; greek = kTRUE; }
      else if (name.Index("pi+") == 0) 
	{ name = "p+"; greek = kTRUE; }
      else if (name.Index("pi-") == 0) 
	{ name = "p-"; greek = kTRUE; }
      else if (name.Index("mu+") == 0) 
	{ name = "m+"; greek = kTRUE; }
      else if (name.Index("mu-") == 0) 
	{ name = "m-"; greek = kTRUE; }

      if (fNumber > 0) {
        char num[10];
        sprintf(num,"%i",fNumber);
        name = name + " (" + num + ")";
      }

      fText = new TText(fScale*vin[0],fScale*vin[1],name);
      if (fTextColor == 0)
	  fText->SetTextColor(fLine->GetLineColor());
      else
	  fText->SetTextColor(fTextColor);
      fText->SetTextSize(fTextSize);
      if (greek) fText->SetTextFont(122);
      fObjs->Add(fText);
    }
    // Add EMC info to the track
    // Draw a little cube (volume ~ energy deposit)
    
    
    if (energy > 0.0) {			    // Draw the energy deposit in the EMC
	
	Float_t x = vin[0] + 20*vin[3];	    // Propagate a little further
	Float_t y = vin[1] + 20*vin[4];
	Float_t z = vin[2] + 10*vin[5];
	Float_t phi = TMath::ATan(y/x) * 180./3.1415962;;

	Float_t d = TMath::Power(energy,1./3.); // Calculate cube edge length
	Float_t dx = 10.*d;
	Float_t dy = 10.*d;
	Float_t dz = 10.*d;
	
	SetMarker(kCyan,x,y,z,dx,dy,dz,0.0,phi);	
    }
    
    // Add IFR info to the track
    // Draw a small blue cube for each hit layer
    
    if (charge != 0) {

      Int_t strips = 0;			// Number of IFR strips
      Int_t layers = 0;			// Number of IFR layers

      strips = fMicro->GetIfrNumberOfStrips();
      layers = fMicro->GetIfrHitLayers(); // This is zero since 8.2.x
      //cout << strips << '\t' << layers << endl;
    
      Bool_t newMicro = (layers == 0 && strips > 0);

      if (strips > 0) {
	
	if (newMicro) {
	  layers = 20; // For 8.2.x micro
	}

        Float_t d = TMath::Power(strips,1./3.); // Calculate cube edge length
	
	Float_t dx = 5*d;
	Float_t dy = 5*d;
	Float_t dz = 5*d;
	
	for (int j=0;j<layers;j++) {
	    Float_t x = vin[0] + (100+5*j)*vin[3]; // Propagate a little further
	    Float_t y = vin[1] + (100+5*j)*vin[4];
	    Float_t z = vin[2] + ( 50+5*j)*vin[5];
	    Float_t phi = TMath::ATan(y/x) * 180./3.1415962;

	    if (newMicro) {   // For 8.2.x micro
		if (fMicro->GetIfrNumberOfStrips(j)==0) continue;
		d = fMicro->GetIfrNumberOfStrips(j);
		//cout << j << '\t' << fMicro->GetIfrNumberOfStrips(j) << endl;
		dx = dy = dz = d;
		SetMarker(kBlue,x,y,z,dx,dy,dz,0.0,phi);
	    }
	    else    // 8.1.x micro
		SetMarker(kBlue,x,y,z,dx,dy,dz,0.0,phi);
	}
	
      }
    }

    this->DrawTrack("",kTRUE); // Draw all objects from the cache
}
 
//______________________________________________________________________________
void PAFDisplayTrack::SetLineAttributes()
{
    //*-*-*-*-*-*-*-*-*Invoke the DialogCanvas Line attributes*-*-*-*-*-*-*
    //*-*              =======================================
    
    gROOT->SetSelectedPrimitive(fLine);
    fLine->SetLineAttributes();
}

//______________________________________________________________________________
void PAFDisplayTrack::HideText()
{
  // TBD
}
