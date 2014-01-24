//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PAFDisplay                                                           //
//                                                                      //
// Utility class to display BABAR outline, tracks, hits,..              //
// Adapted from AliROOT (M.Kunze, June 1999)				//
//                                                                      //
// Default parameters which can be modified (on the command line):	//
//									//
// Boolean:								//
// Boolean:								//
// Label	set	true	#Draw particle labels			//
// Vertexing	set	false	#Perform vertexing operation		//
// Conditions	set	true	#Dump conditions database		//
// Logbook	set	false	#Dump electronic logbook		//
// Verbose	set	false	#Dump event				//
// DET		set	true	#Detector outline visibility		//
// MAG		set	false	#Magnet coil visibility			//
// BPQ		set	false	#Beam pipe/quadrupole visibility	//
// SVT		set	false	#Silicon vertex tracker visibility	//
// DCH		set	false	#Drift chamber visibility		//
// DRC		set	false	#DIRC visibility			//
// EMC		set	false	#Calorimeter visibility			//
// IFR		set	false	#Instrumented flux return visibility	//
//									//
// Integer								//
// BgdColor	set	0	#Color of background			//
// LblColor	set	0	#Color of labels			//
// DetColor	set	50	#Color of detector outline		//
//									//
// Double								//
// PtCut	set	0.05	#Cut on Pt (prongs)			//
// ECut		set	0.05	#Cut on EMC energy (neutrals)		//
// LblSize	set	0.025	#Size of labels				//
// LblOffset	set	0.0017	#Label offset from track		//
//									//
// String								//
// View		set	Front	#Detector and event view		//
// Mode		set	Pick	#Pick/Zoom mode				//
// Pid		set	Reco	#Truth/Selector/Reco PID mode		//
// Kine		set	Reco	#Truth/Reco kinematics mode		//
// List		set	TaggingList #Draw cadidate list			//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Dec. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#define DISPLAYFILE "PAFDisplay.tcl"

// Root headers

#include "TVirtualX.h"
#include "TVirtualViewer3D.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TButton.h"
#include "TCanvas.h"
#include "TView.h"
#include "TView3D.h"
#include "TText.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TMarker3DBox.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TList.h"
#include "TDiamond.h"
#include "TNode.h"
#include "TArc.h"
#include "TTUBE.h"
#include "TSlider.h"
#include "TSliderBox.h"
#include "TGaxis.h"
#include "TMath.h"
#include "X3DBuffer.h"
#include "TGeometry.h"
#include "TVector.h"

// PAF headers

#include <strstream>
using namespace std;

#include "PAFDisplay/PAFDisplay.h"
#include "PAFDisplay/PAFDisplayTrack.h"
#include "PAFDisplay/PAFDisplayMcTrack.h"
#include "PAFDisplay/PAFDisplayVertex.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoManager/TParameterManager.h"
#include "RhoManager/TEventManager.h"
#include "RhoManager/TAssociator.h"
#include "RhoManager/TRunDB.h"
#include "RhoManager/TLogInfo.h"
#include "RhoBase/VAbsReader.h"
#include "RhoTools/TPrintTree.h"
#include "RhoBase/VAbsVertexSelector.h"
#include "RhoConditions/TConditions.h"
#include "RhoConditions/TBeams.h"
#include "RhoBase/VAbsTag.h"

PAFDisplay *gDisplay;

const Double_t ptcutmax  = 4;
const Double_t etacutmax = 4;

ClassImp(PAFDisplay)


//_____________________________________________________________________________
PAFDisplay::PAFDisplay() :
fSpot(0), fPrimVtx(0), fAssociator(0), fLogo(0), fHitsCuts(0), 
fConsole(0)
{
    fCanvas   = 0;
    fTracks   = new TList;
    fVertices = new TList;
    fMgr      = TRho::Instance()->GetParameterManager();
}

//_____________________________________________________________________________
PAFDisplay::PAFDisplay(TEventManager *evtmgr, Int_t size) : 
fSpot(0), fPrimVtx(0), fTruthMode(kFALSE), fListMode(kFALSE), 
fLogo(0), fHitsCuts(0), fDraw(kFALSE), fConsole(0)
{
    // Create an event display object.
    // A canvas named "edisplay" is created with a vertical size in pixels
    //
    //    A QUICK Overview of the Event Display functions
    //    ===============================================
    //
    //  The event display can ve invoked by executing the macro "display.C"
    // A canvas like in the picture below will appear.
    //
    //  On the left side of the canvas, the following buttons appear:
    //   *Next*       to move to the next event
    //   *Previous*   to move to the previous event
    //   *Top View*   to display a top view of the current event
    //   *Side View*  to display a side view of the current event
    //   *Front View* to display a front view of the current event
    //   *All Views*  to display front/side/top/30-30 views of the current event
    //   *OpenGL*     to use OpenGl to view the current event.
    //                Note that OpenGL cannot be used across the network.
    //                Before using OpenGL, load the GL libraries
    //                by executing the macro GL.C (in $ROOTSYS/macros/GL.C.
    //                Once in GL, click the HELP button of the GL canvas.
    //   *X3D*        to use X3D to view the current event (Unix only).
    //                Once in X3D, type M to see the list of all possible options.
    //                for example type J to zoom, K to unzoom
    //                use the mouse to rotate.
    //   *Pick*       Select this option to be able to point on a track with the
    //                mouse. Once on the track, use the right button to select
    //                an action. For example, select SetMarkerAttributes to
    //                change the marker type/color/size for the track.
    //   *Zoom*       Select this option (default) if you want to zoom.
    //                To zoom, simply select the selected area with the left button.
    //   *UnZoom*     To revert to the previous picture size.
    //
    //   slider R     On the left side, the vertical slider can be used to
    //                set the default picture size.
    //   slider pcut  At the top of the canvas, a slider can be used to change
    //                the momentum cut (or range) to display tracks.
    //   slider eta   On the right side of the canvas, a vertical slider can be used
    //                to specify a rapidity range for the tracks.
    //
    //    When you are in Zoom mode, you can click on the black part of the canvas
    //  to select special options with the right mouse button.
    //  This will display a pop-up menu with items like:
    //     *Disable detector* 
    //     *Enable detector*, etc.
    //  For example select "Disable detector". You get a dialog box.
    //  Diable detector TRD for example.
    //
    //  When you are in pick mode, you can "Inspect" the object pointed by the mouse.
    //  When you are on a track, select the menu item "InspectParticle"
    //  to display the current particle attributes.
    //
    //  You can activate the Root browser by selecting the Inspect menu
    //  in the canvas tool bar menu. Then select "Start Browser"
    //  This will open a new canvas with the browser. At this point, you may want
    //  to display some histograms (from the Trees). Go to the "File" menu
    //  of the browser and click on "New canvas".
    //  In the browser, click on item "ROOT files" in the left pane.
    //  Click on gDisplay.root.
    //  Click on TH
    //  Click on TPC for example
    //  Click on any variable (eg TPC.fX) to histogram the variable.
    //
    //   If you are lost, you can click on HELP in any Root canvas or browser.
    //Begin_Html
    /*
    <img src="picts/PAFDisplay.gif">
    */
    //End_Html
    
    // Set default parameters
    
    fMgr      = TRho::Instance()->GetParameterManager();
    fMgr->SetParm("View","Front", "Detector and event view");
    fMgr->SetParm("Mode","Pick", "Pick/Zoom mode");
    fMgr->SetParm("Pid","Reco", "Truth/List/Reco PID mode");
    fMgr->SetParm("Kine","Truth", "Reco/Truth kinematics mode");
    fMgr->SetParm("Label",kTRUE, "Draw particle labels");
    fMgr->SetParm("Number",kTRUE, "Draw particle numbers");
    fMgr->SetParm("Vertexing",kFALSE, "Perform vertexing operation");
    fMgr->SetParm("Conditions",kTRUE, "Dump conditions database");
    fMgr->SetParm("Logbook",kFALSE, "Dump electronic logbook");
    fMgr->SetParm("Verbose",kFALSE, "Dump event");
    fMgr->SetParm("TxtFont",11,"Font for info box");
    fMgr->SetParm("BgdColor",kBlack,"Color of background");
    fMgr->SetParm("DetColor",50,"Color of detector outline");
    fMgr->SetParm("LblColor",0,"Color of labels");
    fMgr->SetParm("LblSize",0.025,"Size of labels");
    fMgr->SetParm("LblOffset",0.0017,"Offset of labels");
    fMgr->SetParm("McTracks","K_S0 K_L0", "Mc tracks to draw");
    fMgr->SetParm("Composites","B0", "Composites to draw");
    fMgr->SetParm("Filter","", "Require specific tracks");
    fMgr->SetParm("DET",kTRUE, "Detector outline visibility");
    fMgr->SetParm("MAG",kFALSE,"Magnet coil visibility");
    fMgr->SetParm("BPQ",kFALSE,"Beam pipe/quadrupole visibility");
    fMgr->SetParm("SVT",kFALSE,"Silicon vertex tracker visibility");
    fMgr->SetParm("DCH",kFALSE,"Drift chamber visibility");
    fMgr->SetParm("DRC",kFALSE,"DIRC visibility");
    fMgr->SetParm("EMC",kFALSE,"Calorimeter visibility");
    fMgr->SetParm("IFR",kFALSE,"Instrumented flux return visibility");
    fMgr->SetParm("PtCut",0.0,"Cut on Pt (prongs)");
    fMgr->SetParm("ECut",0.0,"Cut on EMC energy (neutrals)");
    fMgr->SetParm("List","TaggingList","Draw entries of a list");
    if (fMgr->GetStringParm("Assoc")=="none") // set default
      fMgr->SetParm("Assoc","Chi2","MC Associator to use");
/*    
    // Is there a settings file to read ?
    cout << "Reading " << DISPLAYFILE << endl;
    TFile* file=TFile::Open(DISPLAYFILE,"READ");
    if (file->IsOpen()) {
	file->Close();
	fMgr->ReadParm(DISPLAYFILE);
    }
    else {
	cout << endl;
	cout << "PAFDisplay: You can read your favorite settings from a" << endl;
	cout << "              TCL settings file '" << DISPLAYFILE << "' or save" << endl;
	cout << "              the current settings with                 " << endl;
	cout << "              gDisplay->WriteParm(\"" << DISPLAYFILE << "\")" << endl;
    }
*/    
    
    fMgr->PrintAllParm();
    
    // Access conditions database
    cout << "Reading conditions" << endl;
    fConditions = TRho::Instance()->GetConditions();
    if (fConditions != 0) fConditions->ReadDchCond(kFALSE); // We do not need the DCH here
    if (fConditions != 0) fConditions->ReadSvtPidCalib(kFALSE); // We do not need the DCH here
    
    gDisplay = this;
    fEvtMgr  = evtmgr;
    fTracks  = new TList;
    fVertexSelector = 0;
    fVertices  = new TList;
    fParticles = new TCandList;
    fList = new TCandList;
    fMcParticles = new TCandList;

    // Instantiate the MC associator to use
    cout << "Setting up associator" << endl;
    TString assoc = fMgr->GetStringParm("Assoc");
    cout << "Using " << (const char *) assoc << " associator" << endl;
    if (assoc == "Chi2")
	fAssociator = new TChi2Associator(*fMcParticles,*fParticles);
    else if (assoc == "Micro")
	fAssociator = new TMicroAssociator(*fMcParticles,*fParticles);
    else
	fAssociator = new TMapAssociator(*fMcParticles,*fParticles);

    // Create display canvas
    Int_t ysize = size;
    if (ysize < 100) ysize = 750;
    Int_t xsize = Int_t(size*830./ysize);
    fCanvas  = new TCanvas("Canvas", "PAF Event Display",14,47,xsize,ysize);
    fCanvas->SetEditable(kFALSE);
    fCanvas->ToggleEventStatus();
    
    // Create main display pad
    fPad = new TPad("viewpad", "PAF display",0.15,0.01,0.97,0.96);
    fPad->Draw();
    fPad->Modified();
    fPad->SetFillColor(fMgr->GetIntParm("BgdColor"));
    fPad->SetBorderSize(2);

    // Create user interface control pad
    DisplayButtons();
    fCanvas->cd();
    
    // Create Range and mode pad
    Double_t dxtr     = 0.15;
    Double_t dytr     = 0.45;
    fTrigPad = new TPad("trigger", "range and mode pad",0,0,dxtr,dytr);
    fTrigPad->Draw();
    fTrigPad->cd();
    fTrigPad->SetFillColor(22);
    fTrigPad->SetBorderSize(2);
    char detmode[] = "TRho::Instance()->GetDisplay()->SetDetVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"DET\"))";
    fDetButton = new TButton("DET",detmode,0.05,0.86,0.65,0.93);
    fDetButton->SetFillColor(38);
    if (fMgr->GetBoolParm("DET"))  fDetButton->SetFillColor(kGreen);
    fDetButton->Draw();
    char ifrmode[] = "TRho::Instance()->GetDisplay()->SetIfrVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"IFR\"))";
    fIfrButton = new TButton("IFR",ifrmode,0.05,0.78,0.65,0.85);
    fIfrButton->SetFillColor(38);
    if (fMgr->GetBoolParm("IFR"))  fIfrButton->SetFillColor(kGreen);
    fIfrButton->Draw();
    char magmode[] = "TRho::Instance()->GetDisplay()->SetMagVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"MAG\"))";
    fMagButton = new TButton("MAG",magmode,0.05,0.70,0.65,0.77);
    fMagButton->SetFillColor(38);
    if (fMgr->GetBoolParm("MAG"))  fMagButton->SetFillColor(kGreen);
    fMagButton->Draw();
    char emcmode[] = "TRho::Instance()->GetDisplay()->SetEmcVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"EMC\"))";
    fEmcButton = new TButton("EMC",emcmode,0.05,0.62,0.65,0.69);
    fEmcButton->SetFillColor(38);
    if (fMgr->GetBoolParm("EMC"))  fEmcButton->SetFillColor(kGreen);
    fEmcButton->Draw();
    char drcmode[] = "TRho::Instance()->GetDisplay()->SetDrcVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"DRC\"))";
    fDrcButton = new TButton("DRC",drcmode,0.05,0.54,0.65,0.61);
    fDrcButton->SetFillColor(38);
    if (fMgr->GetBoolParm("DRC"))  fDrcButton->SetFillColor(kGreen);
    fDrcButton->Draw();
    char dchmode[] = "TRho::Instance()->GetDisplay()->SetDchVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"DCH\"))";
    fDchButton = new TButton("DCH",dchmode,0.05,0.46,0.65,0.53);
    fDchButton->SetFillColor(38);
    if (fMgr->GetBoolParm("DCH"))  fDchButton->SetFillColor(kGreen);
    fDchButton->Draw();
    char svtmode[] = "TRho::Instance()->GetDisplay()->SetSvtVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"SVT\"))";
    fSvtButton = new TButton("SVT",svtmode,0.05,0.38,0.65,0.45);
    fSvtButton->SetFillColor(38);
    if (fMgr->GetBoolParm("SVT"))  fSvtButton->SetFillColor(kGreen);
    fSvtButton->Draw();
    char pibmode[] = "TRho::Instance()->GetDisplay()->SetBpqVisibility(!TRho::Instance()->GetParameterManager()->GetBoolParm(\"BPQ\"))";
    fBpqButton = new TButton("BPQ",pibmode,0.05,0.30,0.65,0.37);
    fBpqButton->SetFillColor(38);
    if (fMgr->GetBoolParm("BPQ"))  fBpqButton->SetFillColor(kGreen);
    fBpqButton->Draw();
    fRangeSlider = new TSlider("range","range",0.7,0.30,0.9,0.93);
    fRangeSlider->SetObject(this);
    
    Double_t db = 0.08;
    char truthmode[] = "TRho::Instance()->GetDisplay()->SetTruthMode()";
    fTruthButton = new TButton("T",truthmode,0.7,0.19,0.9,0.19+db);
    fTruthButton->SetFillColor(38);
    fTruthButton->Draw();
    char recomode[] = "TRho::Instance()->GetDisplay()->SetListMode(kFALSE)";
    fRecoButton = new TButton("R",recomode,0.7,0.10,0.9,0.10+db);
    fRecoButton->SetFillColor(38);
    fRecoButton->Draw();
    char listmode[] = "TRho::Instance()->GetDisplay()->SetListMode(kTRUE)";
    fListButton = new TButton("L",listmode,0.7,0.01,0.9,0.01+db);
    fListButton->SetFillColor(38);
    fListButton->Draw();
    char pickmode[] = "TRho::Instance()->GetDisplay()->SetPickMode()";
    fPickButton = new TButton("Pick",pickmode,0.05,0.19,0.65,0.19+db);
    fPickButton->SetFillColor(38);
    fPickButton->Draw();
    char zoommode[] = "TRho::Instance()->GetDisplay()->SetZoomMode()";
    fZoomButton = new TButton("Zoom",zoommode,0.05,0.10,0.65,0.10+db);
    fZoomButton->SetFillColor(38);
    fZoomButton->Draw();
    char butUnzoom[] = "TRho::Instance()->GetDisplay()->UnZoom()";
    fUnzoomButton = new TButton("UnZoom",butUnzoom,0.05,0.01,0.65,0.01+db);
    fUnzoomButton->SetFillColor(38);
    fUnzoomButton->Draw();
    //??AppendPad(); // append display object as last object to force selection
    
    // Create momentum cut slider pad
    fCanvas->cd();
    fCutPad = new TPad("MomentumSlider", "Momentum slider pad",dxtr,.96,1,1);
    fCutPad->Draw();
    fCutPad->cd();
    fCutPad->SetFillColor(22);
    fCutPad->SetBorderSize(2);
    fCutSlider   = new TSlider("pcut","Pt cut",0,0,1,1);
    fCutSlider->SetRange(fMgr->GetDoubleParm("PtCut")/ptcutmax,1);
    fCutSlider->SetObject(this);
    fCutSlider->SetFillColor(45);
    TSliderBox *sbox = (TSliderBox*)fCutSlider->GetListOfPrimitives()->First();
    sbox->SetFillColor(46);
    fCutSlider->cd();
    fCutaxis = new TGaxis(0.02,0.8,0.98,0.8,0,ptcutmax,510,"");
    fCutaxis->SetLabelSize(0.5);
    fCutaxis->SetTitleSize(0.6);
    fCutaxis->SetTitleOffset(0.5);
    fCutaxis->SetTitle("Momentum .  ");
    fCutSlider->GetListOfPrimitives()->AddFirst(fCutaxis);
    
    // Create rapidity cut slider pad
    fCanvas->cd();
    fEtaPad = new TPad("EnergySlider", "Energy slider pad",0.97,0,1,0.96);
    fEtaPad->Draw();
    fEtaPad->cd();
    fEtaPad->SetFillColor(22);
    fEtaPad->SetBorderSize(2);
    fEtaSlider   = new TSlider("etacut","Energy cut",0,0,1,1);
    fEtaSlider->SetRange(fMgr->GetDoubleParm("ECut")/etacutmax,1);
    fEtaSlider->SetObject(this);
    fEtaSlider->SetFillColor(45);
    TSliderBox *sbox2 = (TSliderBox*)fEtaSlider->GetListOfPrimitives()->First();
    sbox2->SetFillColor(46);
    fEtaSlider->cd();
    fEtaaxis = new TGaxis(0.9,0.02,0.9,0.98,0,etacutmax,510,"");
    fEtaaxis->SetLabelSize(0.5);
    fEtaaxis->SetTitleSize(0.6);
    fEtaaxis->SetTitleOffset(0.2);
    fEtaaxis->SetTitle("Energy .  ");
    fEtaSlider->GetListOfPrimitives()->AddFirst(fEtaaxis);
    fCanvas->cd();
    
    fCanvas->cd();
    fCanvas->Update();
    
    fDetGeometry = DetGeometry(fMgr->GetIntParm("DetColor"));   // Set up the detector
    fBpqGeometry = BpqGeometry();   // Set up the beam pipe
    fSvtGeometry = SvtGeometry();   // Set up the vertex tracker
    fDchGeometry = DchGeometry();   // Set up the DCH detector
    fDrcGeometry = DrcGeometry();   // Set up the DRC detector
    fEmcGeometry = EmcGeometry();   // Set up the EMC detector
    fMagGeometry = MagGeometry();   // Set up the magnet
    fIfrGeometry = IfrGeometry();   // Set up the IFR detector
    
    // Determine mode
    if (fMgr->GetStringParm("Mode")=="Pick") 
	SetPickMode();		    // Start in this mode
    else
	SetZoomMode();
    
    // Initialize display default parameters
    fDrawAllViews = kFALSE;
    SetRange();
    SetPTcut();
    SetEcut();
    
    // Determine view
    
    TString view = fMgr->GetStringParm("View");
    
    fPad->cd();
    if (view=="Side") {
	SetView(-90.,0.,-90.);
    }
    else if (view=="Top") {
	SetView(90.,90.,90.);
    }
    else {
	SetView(0.,-90.,0.); // Front view
    }
    
    // Do we display MC truth ?
    if (fMgr->GetStringParm("Kine")=="Truth") SetTruthMode();
    fMcTracks = fMgr->GetStringParm("McTracks");
    fFilter = fMgr->GetStringParm("Filter");

    // Do we want display lists ?
    if (fMgr->GetStringParm("List")!="") SetListMode();
    
    // Install the console window
    //fConsole = new TCanvas("Console", "PAF Console",14,100+ysize,xsize,200);
    //fConsole->SetEditable(kFALSE);
    //fConsole->ToggleEventStatus();

    // Draw the first event
    fDraw = kTRUE;
    ShowEvent(0);

    // Further print options
    if (fMgr->GetBoolParm("Logbook"))  fRunDB = new TRunDB; // Do we need logbook access ?

    // Register this display
    TRho *rho = const_cast<TRho*> (TRho::Instance());
    rho->SetDisplay(this);
}


//_____________________________________________________________________________
PAFDisplay::~PAFDisplay()
{
    delete fCanvas; delete fConsole; delete fPad; delete fTrigPad; delete fCutPad; delete fEtaPad;
    delete fRangeSlider; delete fCutSlider; delete fEtaaxis; delete fCutaxis;
    delete fDetButton; delete fBpqButton; delete fMagButton; delete fIfrButton; delete fEmcButton; delete fDrcButton; delete fDchButton; delete fSvtButton;
    delete fPickButton; delete fZoomButton; delete fUnzoomButton;
    delete fRecoButton; delete fTruthButton; delete fListButton;
    delete fDetGeometry; delete fBpqGeometry; delete fSvtGeometry; delete fDchGeometry; delete fDrcGeometry; delete fEmcGeometry; delete fMagGeometry; delete fIfrGeometry;
    if (fTracks) fTracks->Delete(); delete fTracks;
    if (fVertices) fVertices->Delete(); delete fVertices;
    delete fSpot;
    if (fRunDB) delete fRunDB;
    delete fParticles; delete fList; delete fMcParticles;
    delete fAssociator;
    
    fMgr->WriteParm("PAFDisplay.save.tcl"); // Save the actual settings
}

//_____________________________________________________________________________

void PAFDisplay::SetDetVisibility(Bool_t vis)  
{
    fMgr->SetParm("DET",vis);
    if (vis)  fDetButton->SetFillColor(kGreen);
    if (!vis) fDetButton->SetFillColor(38);
    fDetButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetBpqVisibility(Bool_t vis)  
{
    fMgr->SetParm("BPQ",vis);
    if (vis)  fBpqButton->SetFillColor(kGreen);
    if (!vis) fBpqButton->SetFillColor(38);
    fBpqButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetIfrVisibility(Bool_t vis)    
{
    fMgr->SetParm("IFR",vis);
    if (vis)  fIfrButton->SetFillColor(kGreen);
    if (!vis) fIfrButton->SetFillColor(38);
    fIfrButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetMagVisibility(Bool_t vis)    
{
    fMgr->SetParm("MAG",vis);
    if (vis)  fMagButton->SetFillColor(kGreen);
    if (!vis) fMagButton->SetFillColor(38);
    fMagButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetEmcVisibility(Bool_t vis)    
{
    fMgr->SetParm("EMC",vis);
    if (vis)  fEmcButton->SetFillColor(kGreen);
    if (!vis) fEmcButton->SetFillColor(38);
    fEmcButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetDrcVisibility(Bool_t vis)
{
    fMgr->SetParm("DRC",vis);
    if (vis)  fDrcButton->SetFillColor(kGreen);
    if (!vis) fDrcButton->SetFillColor(38);
    fDrcButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetDchVisibility(Bool_t vis)
{
    fMgr->SetParm("DCH",vis);
    if (vis)  fDchButton->SetFillColor(kGreen);
    if (!vis) fDchButton->SetFillColor(38);
    fDchButton->Modified();
    if (fDraw) DrawEvent();
}

void PAFDisplay::SetSvtVisibility(Bool_t vis)
{
    fMgr->SetParm("SVT",vis);
    if (vis)  fSvtButton->SetFillColor(kGreen);
    if (!vis) fSvtButton->SetFillColor(38);
    fSvtButton->Modified();
    if (fDraw) DrawEvent();
}

//_____________________________________________________________________________
void PAFDisplay::Clear(Option_t *)
{
    //    Delete graphics temporary objects
    
    fTracks->Delete();
    fVertices->Delete();
}

//----------------------------------------------------------------------------
void PAFDisplay::ShowTrack(Int_t idx) {
/*
PAFDetector *TPC=(PAFDetector*)TRho::Instance()->GetDisplay()->GetModule("TPC");
TObjArray *points=TPC->Points();
int ntracks=points->GetEntriesFast();
for (int track=0;track<ntracks;track++) {
PAFPoints *pm = (PAFPoints*)points->UncheckedAt(track);
if (!pm) continue;
if (idx == pm->GetIndex()) {
pm->SetMarkerColor(2);
pm->SetMarkerStyle(22);
pm->Draw("same");
//       fPad->Update();
//       fPad->Modified();
TClonesArray *particles=TRho::Instance()->GetDisplay()->Particles();
GParticle *p = (GParticle*)particles->UncheckedAt(idx);
printf("\nTrack index %d\n",idx);
printf("Particle ID %d\n",p->GetKF());
printf("Parent %d\n",p->GetParent());
printf("First child %d\n",p->GetFirstChild());
printf("Px,Py,Pz %f %f %f\n",p->GetPx(),p->GetPy(),p->GetPz());
return;
}
}
    */
}

//----------------------------------------------------------------------------
void PAFDisplay::HideTrack(Int_t idx) {
/*
PAFDetector *TPC=(PAFDetector*)TRho::Instance()->GetDisplay()->GetModule("TPC");
TObjArray *points=TPC->Points();
int ntracks=points->GetEntriesFast();
for (int track=0;track<ntracks;track++) {
PAFPoints *pm = (PAFPoints*)points->UncheckedAt(track);
if (!pm) continue;
if (idx == pm->GetIndex()) {
pm->SetMarkerColor(5);
pm->SetMarkerStyle(1);
pm->Draw("same");
//       fPad->Update();
//       fPad->Modified();
return;
}
}
    */
}

//_____________________________________________________________________________
void PAFDisplay::DisableDetector(const char *name)
{
    //    Disable detector name from graphics views
    /*   
    PAFModule *module = (PAFModule*)TRho::Instance()->GetDisplay()->Modules()->FindObject(name);
    if (!module) return;
    module->Disable();
    if (fDraw) DrawEvent();
    */
}

//_____________________________________________________________________________
void PAFDisplay::DisplayButtons()
{
    //    Create the user interface buttons
    
    fButtons = new TPad("buttons", "newpad",0,0.45,0.15,1);
    fButtons->Draw();
    fButtons->SetFillColor(38);
    fButtons->SetBorderSize(2);
    fButtons->cd();
    
    Int_t butcolor = 33;
    Double_t dbutton = 0.08;
    Double_t y  = 0.96;
    Double_t dy = 0.014;
    Double_t x0 = 0.05;
    Double_t x1 = 0.95;
    
    TButton *button;
    char but1[] = "TRho::Instance()->GetDisplay()->ShowEvent()";
    button = new TButton("Next",but1,x0,y-dbutton,x1,y);
    button->SetFillColor(38);
    button->Draw();
    
    y -= dbutton +dy;
    char but2[] = "TRho::Instance()->GetDisplay()->ShowEvent(-1)";
    button = new TButton("Previous",but2,x0,y-dbutton,x1,y);
    button->SetFillColor(38);
    button->Draw();
    
    y -= dbutton +dy;
    char but3[] = "TRho::Instance()->GetDisplay()->SetView(90.,90.,90.)";
    button = new TButton("Top View",but3,x0,y-dbutton,x1,y);
    button->SetFillColor(butcolor);
    button->Draw();
    
    y -= dbutton +dy;
    char but4[] = "TRho::Instance()->GetDisplay()->SetView(-90.,0.,-90.)";
    button = new TButton("Side View",but4,x0,y-dbutton,x1,y);
    button->SetFillColor(butcolor);
    button->Draw();
    
    y -= dbutton +dy;
    char but5[] = "TRho::Instance()->GetDisplay()->SetView(0.,-90.,0.)";
    button = new TButton("Front View",but5,x0,y-dbutton,x1,y);
    button->SetFillColor(butcolor);
    button->Draw();
    
    y -= dbutton +dy;
    char but6[] = "TRho::Instance()->GetDisplay()->DrawAllViews()";
    button = new TButton("All Views",but6,x0,y-dbutton,x1,y);
    button->SetFillColor(butcolor);
    button->Draw();
    
    y -= dbutton +dy;
    char but7[] = "TRho::Instance()->GetDisplay()->DrawViewGL()";
    button = new TButton("OpenGL",but7,x0,y-dbutton,x1,y);
    button->SetFillColor(38);
    button->Draw();
    
    y -= dbutton +dy;
    char but8[] = "TRho::Instance()->GetDisplay()->DrawViewX3D()";
    button = new TButton("X3D",but8,x0,y-dbutton,x1,y);
    button->SetFillColor(38);
    button->Draw();
    
    // display logo
    TDiamond *diamond = new TDiamond(0.05,0.015,0.95,0.22);
    diamond->SetFillColor(50);
    diamond->SetTextAlign(22);
    diamond->SetTextColor(5);
    diamond->SetTextSize(0.11);
    diamond->Draw();
    diamond->AddText(".. ");
    diamond->AddText("BaBar");
    diamond->AddText(TString("PAF ")+TRho::Instance()->GetVersion());
    diamond->AddText("... ");
    diamond->AddText(" ");
}

//______________________________________________________________________________
Int_t PAFDisplay::DistancetoPrimitive(Int_t px, Int_t)
{
    // Compute distance from point px,py to objects in event
    
    gPad->SetCursor(kCross);
    
    if (gPad == fTrigPad) return 9999;
    if (gPad == fCutPad)  return 9999;
    if (gPad == fEtaPad)  return 9999;
    
    const Int_t big = 9999;
    Int_t dist   = big;
    Double_t xmin = gPad->GetX1();
    Double_t xmax = gPad->GetX2();
    Double_t dx   = 0.02*(xmax - xmin);
    Double_t x    = gPad->AbsPixeltoX(px);
    if (x < xmin+dx || x > xmax-dx) return dist;
    
    if (fZoomMode) return 0;
    else           return 7;
}

//_____________________________________________________________________________
void PAFDisplay::DrawEvent(Option_t *)
{
    if (fPad == 0) return;
    
    if (fDrawAllViews) {
	DrawAllViews();
    }
    else {
	fDrawAllViews = kFALSE;
	fPad->cd();
	//gSystem->Sleep(200);
	DrawView(fTheta, fPhi, fPsi);
	DrawTitle();
    }
}

//_____________________________________________________________________________
void PAFDisplay::DrawHits()
{
    //    Draw hits for all detectors
    /*
    Double_t cutmin, cutmax, etamin, etamax, pmom, smin, smax, eta, theta, r;
    Double_t *pxyz;
    Int_t ntracks,track;
    GParticle *particle;
    TObjArray *points;
    PAFPoints *pm;
    
      //Get cut slider
      smax   = fCutSlider->GetMaximum();
      smin   = fCutSlider->GetMinimum();
      cutmin = ptcutmax*smin;
      if (smax < 0.98) cutmax = ptcutmax*smax;
      else             cutmax = 100000;
      
	//Get eta slider
	smax   = fEtaSlider->GetMaximum();
	smin   = fEtaSlider->GetMinimum();
	etamin = etacutmax*(2*smin-1);
	etamax = etacutmax*(2*smax-1);
	if (smin < 0.02) etamin = -1000;
	if (smax > 0.98) etamax =  1000;
	
	  TIter next(TRho::Instance()->GetDisplay()->Modules());
	  PAFModule *module;
	  fHitsCuts = 0;
	  while((module = (PAFModule*)next())) {
	  if (!module->IsActive()) continue;
	  points = module->Points();
	  if (!points) continue;
	  ntracks = points->GetEntriesFast();
	  for (track=0;track<ntracks;track++) {
	  pm = (PAFPoints*)points->UncheckedAt(track);
	  if (!pm) continue;
	  particle = pm->GetParticle();
	  if (!particle) continue;
	  pmom = particle->GetMomentum();
	  if (pmom < cutmin) continue;
	  if (pmom > cutmax) continue;
	  // as a first approximation, take eta of first point
	  pxyz  = pm->GetP();
	  r     = TMath::Sqrt(pxyz[0]*pxyz[0] + pxyz[1]*pxyz[1]);
	  theta = TMath::ATan2(r,TMath::Abs(pxyz[2]));
	  if(theta) eta = -TMath::Log(TMath::Tan(0.5*theta)); else eta = 1e10;
	  if (pxyz[2] < 0) eta = -eta;
	  if (eta < etamin || eta > etamax) continue;
	  pm->Draw();
	  fHitsCuts += pm->GetN();
	  }
	  }
    */
}

//_____________________________________________________________________________
void PAFDisplay::DrawTitle(Option_t *option)
{
    //    Draw the event title
    
    Double_t xmin = gPad->GetX1();
    Double_t xmax = gPad->GetX2();
    Double_t ymin = gPad->GetY1();
    Double_t ymax = gPad->GetY2();
    Double_t dx   = xmax-xmin;
    Double_t dy   = ymax-ymin;
    
    if (strlen(option) == 0) {
	TPaveText *title = new TPaveText(xmin +0.01*dx, ymax-0.12*dy, xmin +0.5*dx, ymax-0.01*dy);
	title->SetBit(kCanDelete);
	title->SetFillColor(42);
	title->Draw();
	char ptitle[128];
	sprintf(ptitle,"Event:%d, Run:%d",fEvtMgr->GetEventNumber(), fEvtMgr->GetRunNumber());
	title->AddText(ptitle);
	Int_t nparticles = fEvtMgr->GetNumberOfTracks();
	sprintf(ptitle,"Nparticles = %d  Nhits = %d",nparticles, fHitsCuts);
	title->AddText(ptitle);
	title->AddText(fEvtMgr->GetCurrentFilename());
    } else {
	TPaveLabel *label = new TPaveLabel(xmin +0.01*dx, ymax-0.07*dy, xmin +0.2*dx, ymax-0.01*dy,option);
	label->SetBit(kCanDelete);
	label->SetFillColor(42);
	label->Draw();
    }
}

//_____________________________________________________________________________
void PAFDisplay::DrawMcTruth()
{
    // Show the MCTruth

    VAbsReader *reader = fEvtMgr->GetReader();

    Int_t font = fMgr->GetIntParm("TxtFont");
    if (font==0) return;

    Double_t xmin = gPad->GetX1();
    Double_t xmax = gPad->GetX2();
    Double_t ymin = gPad->GetY1();
    Double_t ymax = gPad->GetY2();
    Double_t dx   = xmax-xmin;
    Double_t dy   = ymax-ymin;
    
    TPaveText *fTruthPad = new TPaveText(xmin +0.01*dx, ymax-0.98*dy, xmin +0.98*dx, ymax-0.80*dy);;
    fTruthPad->SetFillColor(42);
    fTruthPad->SetBit(kCanDelete);
    fTruthPad->Draw();

    TPrintTree *pt = reader->GetPrintTree();
    if (fTruthMode && reader!=0 && pt!=0) {
	fTruthPad->SetTextAlign(15);
        TStringLong mc = pt->PrintAgain();
	cout << (const char *) mc << endl;
	if (mc != "") {
	    istrstream linestr((char*)(const char*)mc);
	    char code[256]; int i=0;
	    while (linestr >> code && i++<7) {
		fTruthPad->AddText(code);
	    }
	}
    }

}

//_____________________________________________________________________________
void PAFDisplay::DrawView(Double_t theta, Double_t phi, Double_t psi)
{
    gPad->cd();
    gPad->SetFillColor(fMgr->GetIntParm("BgdColor"));
    gPad->Clear();
    
    //Loop on all parts to add their products to the pad
    
    fEvtMgr->Fill(fParticles);
    if (fMgr->GetStringParm("List") != "") {
      fEvtMgr->FillFromList(fList,fMgr->GetStringParm("List"));
      cout << (const char *) fMgr->GetStringParm("List") << ": " << fList->GetNumberOfTracks() << endl;
      cout << *fList;
    }

    if (fTruthMode) {
      fEvtMgr->FillMcTruthList(fMcParticles);
    }

    if (fListMode)
	DrawTracks(fList);
    else
	DrawTracks(fParticles);

    DrawBeamspot();
    DrawPrimaryVertex();
    if (fMgr->GetBoolParm("Vertexing")) DrawVertices(fParticles);
    DrawHits();
    DrawLogo();

    //    Draw a view of the detector
    
    gPad->SetCursor(kWatch);
    
    Int_t iret;
    TView *view = gPad->GetView(); 
    if (view==0) {
	gPad->SetView(new TView3D());
	view = gPad->GetView();
    }
    Double_t range = fRrange*fRangeSlider->GetMaximum();
    if (view!=0) view->SetRange(-range,-range,-range,range, range, range);
    fZoomX0[0] = -1;
    fZoomY0[0] = -1;
    fZoomX1[0] =  1;
    fZoomY1[0] =  1;
    fZooms = 0;
    
    // Display detector geometry
    
    if (fMgr->GetBoolParm("DET")) fDetGeometry->Draw("same");
    if (fMgr->GetBoolParm("BPQ")) fBpqGeometry->Draw("same");
    if (fMgr->GetBoolParm("SVT")) fSvtGeometry->Draw("same");
    if (fMgr->GetBoolParm("DCH")) fDchGeometry->Draw("same");
    if (fMgr->GetBoolParm("DRC")) fDrcGeometry->Draw("same");
    if (fMgr->GetBoolParm("EMC")) fEmcGeometry->Draw("same");
    if (fMgr->GetBoolParm("MAG")) fMagGeometry->Draw("same");
    if (fMgr->GetBoolParm("IFR")) fIfrGeometry->Draw("same");
    
    
    // add itself to the list (must be last)
    AppendPad();
    
    if (view!=0) view->SetView(phi, theta, psi, iret);
}

//_____________________________________________________________________________
void PAFDisplay::DrawAllViews()
{
    //    Draw front,top,side and 30 deg views
    
    fDrawAllViews = kTRUE;
    fPad->cd();
    fPad->SetFillColor(15);
    fPad->Clear();
    fPad->Divide(2,2);
    //gSystem->Sleep(200);
    
    // draw 30 deg view
    fPad->cd(1);
    DrawView(30, 30, 0);
    DrawTitle();
    
    // draw front view
    fPad->cd(2);
    DrawView(0, -90,0);
    DrawTitle("Front");
    
    // draw top view
    fPad->cd(3);
    DrawView(90, 90, 90);
    DrawTitle("Top");
    
    // draw side view
    fPad->cd(4);
    DrawView(-90, 0,-90);
    DrawTitle("Side");
    
    fPad->cd();
}

//_____________________________________________________________________________
void PAFDisplay::DrawViewGL()
{
    //    Draw current view using OPENGL
    
    TPad *pad = (TPad*)gPad->GetPadSave();
    pad->cd();
    TView *view = pad->GetView();
    if (!view) return;
    TVirtualViewer3D * v = pad->GetViewer3D("ogl");
}

//_____________________________________________________________________________
void PAFDisplay::DrawViewX3D()
{
    //    Draw current view using X3D
    
    TPad *pad = (TPad*)gPad->GetPadSave();
    pad->cd();
    TView *view = pad->GetView();
    if (!view) return;
    TVirtualViewer3D * v = pad->GetViewer3D("x3d");
}

//_____________________________________________________________________________
void PAFDisplay::EnableDetector(const char *name)
{
    //    Enable detector name in graphics views
    /*   
    PAFModule *module = (PAFModule*)TRho::Instance()->GetDisplay()->Modules()->FindObject(name);
    if (!module) return;
    module->Enable();
    if (fDraw) DrawEvent();
    */
}

//______________________________________________________________________________
void PAFDisplay::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    //  Execute action corresponding to the mouse event
    
    static Double_t x0, y0, x1, y1;
    
    static Int_t pxold, pyold;
    static Int_t px0, py0;
    static Int_t linedrawn;
    Double_t temp;
    
    if (px == 0 && py == 0) { //when called by sliders
	if (event == kButton1Up) {
	    if (fDraw) DrawEvent();
	}
	return;
    }
    if (!fZoomMode && gPad->GetView()) {
	gPad->GetView()->ExecuteRotateView(event, px, py);
	return;
    }
    
    // something to zoom ?
    //   fPad->SetCursor(kCross);
    gPad->SetCursor(kCross);
    
    switch (event) {
	
    case kButton1Down:
	gVirtualX->SetLineColor(-1);
	gPad->TAttLine::Modify();  //Change line attributes only if necessary
	x0 = gPad->AbsPixeltoX(px);
	y0 = gPad->AbsPixeltoY(py);
	px0   = px; py0   = py;
	pxold = px; pyold = py;
	linedrawn = 0;
	return;
	
    case kButton1Motion:
	if (linedrawn) gVirtualX->DrawBox(px0, py0, pxold, pyold, TVirtualX::kHollow);
	pxold = px;
	pyold = py;
	linedrawn = 1;
	gVirtualX->DrawBox(px0, py0, pxold, pyold, TVirtualX::kHollow);
	return;
	
    case kButton1Up:
	gPad->GetCanvas()->FeedbackMode(kFALSE);
	if (px == px0) return;
	if (py == py0) return;
	x1 = gPad->AbsPixeltoX(px);
	y1 = gPad->AbsPixeltoY(py);
	
	if (x1 < x0) {temp = x0; x0 = x1; x1 = temp;}
	if (y1 < y0) {temp = y0; y0 = y1; y1 = temp;}
	gPad->Range(x0,y0,x1,y1);
	if (fZooms < kMAXZOOMS-1) {
	    fZooms++;
	    fZoomX0[fZooms] = x0;
	    fZoomY0[fZooms] = y0;
	    fZoomX1[fZooms] = x1;
	    fZoomY1[fZooms] = y1;
	}
	gPad->Modified(kTRUE);
	return;
    }
    
}

//___________________________________________
void PAFDisplay::LoadPoints()
{
    // Read hits info and store x,y,z info in arrays fPoints
    // Loop on all detectors
    /* 
    TRho::Instance()->GetDisplay()->ResetPoints();
    TIter next(TRho::Instance()->GetDisplay()->Modules());
    PAFModule *module;
    Int_t ntracks = TRho::Instance()->GetDisplay()->GetNtrack();
    for (Int_t track=0; track<ntracks;track++) {
    TRho::Instance()->GetDisplay()->ResetHits();
    TRho::Instance()->GetDisplay()->TreeH()->GetEvent(track);
    while((module = (PAFModule*)next())) {
    module->LoadPoints(track);
    }
    next.Reset();
    }
    */
}

//_____________________________________________________________________________
void PAFDisplay::SetPickMode()
{
    fZoomMode = 0;
    fTrigPad->Modified();
    fPickButton->SetFillColor(kGreen);
    fPickButton->Modified();
    fZoomButton->SetFillColor(38);
    fZoomButton->Modified();
}

//_____________________________________________________________________________
void PAFDisplay::SetZoomMode()
{
    fZoomMode = 1;
    fTrigPad->Modified();
    fPickButton->SetFillColor(38);
    fPickButton->Modified();
    fZoomButton->SetFillColor(kGreen);
    fZoomButton->Modified();
}

//_____________________________________________________________________________
void PAFDisplay::SetPTcut(Double_t ptcut)
{
    fMgr->SetParm("PtCut",ptcut);
    if (fDraw) DrawEvent();
}

//_____________________________________________________________________________
void PAFDisplay::SetEcut(Double_t ecut)
{
    fMgr->SetParm("ECut",ecut);
    if (fDraw) DrawEvent();
}

//_____________________________________________________________________________
void PAFDisplay::SetRange(Double_t rrange, Double_t zrange)
{
    // Set view range along R and Z
    fRrange = rrange;
    fZrange = zrange;
    if (fDraw) DrawEvent();
}

//_____________________________________________________________________________
void PAFDisplay::SetView(Double_t theta, Double_t phi, Double_t psi)
{
    //  change viewing angles for current event
    
    gPad->cd();
    fDrawAllViews = kFALSE;
    
    fPhi   = phi;
    fTheta = theta;
    fPsi   = psi;
    Int_t iret = 0;
    
    static Bool_t oldview = fPhi<-80;
    Bool_t actualview = fPhi<-80; 

    TView *view = gPad->GetView();
    if (view && actualview==oldview) view->SetView(fPhi, fTheta, fPsi, iret);
    else      if (fDraw)  DrawEvent();

    oldview = actualview;

    gPad->Modified();
}

//_____________________________________________________________________________
void PAFDisplay::ShowEvent(Int_t number)
{
    //  Display a specific event
    //  Special modes:
    //  number =  0 show next event
    //  number = -1 show previous event
    
    static TVector events(1,10000);		    // Nobody will look at more...
    static Int_t n = 1, direction = 1;
    
    // Navigate through the events
    
    switch (number) {
    case 0:
	if (events(n)>0) {			    // We have looked at those already
	    if (direction == -1) n++;		    // Already on the screen...
	    fEvtMgr->GetEvent(events(n)-1);	    // Event 1 has index 0 internally
	}
	else {
	    if (n>1) Filter(fEvtMgr->GetEventNumber());		    // Get next event from collection
	    events(n) = fEvtMgr->GetEventNumber();    // Note the current event number
	}
	n++;
	direction = 1;
	break;
    case -1:
	n--;
	if (direction == 1) n--;		    // Already on the screen...
	if (n<1) n=1;
	fEvtMgr->GetEvent(events(n)-1);		    // Get the previous event
	direction = -1;
	break;
    default:
	if (number < 1) number = 1;
	events(n) = number;			    // Note the current event number
	fEvtMgr->GetEvent(number-1);		    // Event 1 has index 0 internally
	n++;
	break;
    }
    
    if (fConditions != 0) {
	TRhoTime theTime(fEvtMgr->GetTime());
	fConditions->At(theTime); // Read the conditions
    }
    
    DrawEvent();			    // Draw the event
    DrawMcTruth();			    // Show the MCTruth info
    DumpEvent();			    // Dump the event
}

//______________________________________________________________________________
void PAFDisplay::UnZoom()
{
    if (fZooms <= 0) return;
    fZooms--;
    TPad *pad = (TPad*)gPad->GetPadSave();
    pad->Range(fZoomX0[fZooms],fZoomY0[fZooms], fZoomX1[fZooms],fZoomY1[fZooms]);
    pad->Modified();
}

//______________________________________________________________________________
void PAFDisplay::DrawLogo() 
{
    Double_t x[]={-350.0,-350.0,-350.5,-351.0,-351.0,-351.0,-352.0,-351.0,-352.0};
    Double_t z[]={350.0,352.0,351.0,352.0,350.0,351.0,352.0,351.0,350.0};
    Double_t y[]={-345.0,-345.0,-345.0,-345.0,-345.0,-345.0,-345.0,-345.0,-345.0};
    // delete fLogo; // Crash 5.26
    fLogo = new TPolyLine3D(9,x,y,z);
    fLogo->SetLineColor(kWhite);
    fLogo->SetLineWidth(2);
    fLogo->Draw();
}

//_____________________________________________________________________________
void PAFDisplay::DrawTracks(TCandList *list)
{
    //  Draw all tracks using the micro DST information
    //  The tracks are instantiated for new events and drawn
    //  from the cache if the event number did not change
    
    static Int_t oldEvent = -1;
    Bool_t newEvent = kFALSE;
    
    if (fEvtMgr->GetEventNumber() != oldEvent) {
	oldEvent = fEvtMgr->GetEventNumber();
        newEvent = kTRUE;
    }

    //    Instantiate and draw micro tracks
    //    Only particles above PTcut are drawn    
    
    Double_t cutmin, cutmax, etamin, etamax, smin, smax;
    
    //Get Pt cut slider
    smax   = fCutSlider->GetMaximum();
    smin   = fCutSlider->GetMinimum();
    cutmin = ptcutmax*smin;
    if (smax < 0.98) cutmax = ptcutmax*smax;
    else             cutmax = 1000;
    
    //Get energy slider
    smax   = fEtaSlider->GetMaximum();
    smin   = fEtaSlider->GetMinimum();
    etamin = etacutmax*smin;
    if (smax < 0.98) etamax = etacutmax*smax;
    else             etamax = 1000;
    
    // Determine scale parameter (for text)
    Double_t labelScale = fMgr->GetDoubleParm("LblOffset")/fRangeSlider->GetMaximum();
    
    // Get parameters
    Bool_t   labelMode  = fMgr->GetBoolParm("Label");
    Bool_t   numberMode = fMgr->GetBoolParm("Number");
    Int_t    labelColor = fMgr->GetIntParm("LblColor");
    Double_t labelSize  = fMgr->GetDoubleParm("LblSize");
    
    // Iterate over all particles and draw them
    
    TCandListIterator iter(*list);
    TCandidate *c;

    //if (newEvent) fTracks->Delete();

    Int_t n = 0;

    cout << endl << "PAFDisplay: Draw event #" << fEvtMgr->GetEventNumber() << endl;

    while ( (c = iter.Next()) ) {
	
	n++; // Increment number

	Int_t charge = (Int_t) c->Charge();
	Double_t pt = c->P3().Perp();
	if (charge!=0 && pt < cutmin) continue;	    // Check momentum range
	if (charge!=0 && pt > cutmax) continue;
	Double_t e = c->Energy();
	if (charge==0 && e < etamin) continue;	    // Check energy range
	if (charge==0 && e > etamax) continue;
		
	// This has to be done each time !?
	if (/*newEvent*/ kTRUE) {

	    TCandidate *theCandidate = c; // Candidate to draw

	    if (c->PdtEntry() == 0) fAssociator->SetRecoPid(c);
	    TString name = c->PdtEntry()->GetName(); // Preset name
	    cout << "Track #" << n << '\t' << (const char *) name << '\t';
	    const VAbsMicroCandidate &micro = c->GetMicroCandidate();
	    if (&micro==0) continue;

	    Double_t trackLength = 0.0;
	    if (charge != 0) {
		trackLength = c->GetMicroCandidate().GetTrackLength();
		trackLength += c->GetMicroCandidate().GetTrackStartFoundRange();
		trackLength *= 2.0; // Seems necessary for packed data
	    }

	    if (fTruthMode) {
	      TCandidate *mc = fAssociator->McFromReco(c);
	      if (mc != 0) {
	        theCandidate = mc; // Draw the MC truth
		name = mc->PdtEntry()->GetName();
	        cout << "matched to " << '\t' << (const char *) name << " " << fAssociator->GetMatchConsistency();
		// Correct for decay length
		trackLength -= (c->GetPosition()-mc->GetPosition()).Mag();
		c->GetMicroCandidate().SetPosition(mc->GetPosition());
	      }
	      else {
		theCandidate->SetType("Rootino");
		name = "";
		cout << "not matched";
	      }
	    }
	    
	    cout << endl ;
	    

	    if (!labelMode || fPhi>-80) name = ""; // Only front view works right now
	    PAFDisplayTrack *track = new  PAFDisplayTrack(theCandidate, name, trackLength);
	    if (track == 0) continue;

	    if (numberMode) // Draw track numbers
	      track->SetNumber(n);
	    else
	      track->SetNumber(0);

	    track->SetScale(labelScale);
	    track->SetTextColor(labelColor);
	    track->SetTextSize(labelSize);
	    track->DrawTrack("same",kFALSE);
	    fTracks->Add(track);    // Book keeping
	    // Print a track info
	    cout << track->GetObjectInfo(0,0) << endl;
	}
	else {
	    PAFDisplayTrack *track  = (PAFDisplayTrack*)  fTracks->At(iter.Index());
	    track->DrawTrack("same",kTRUE);  // Do not re-generate objects
	}
    }

    // Draw MCTruth tracks
    if (fTruthMode) {
      cout << endl << "PAFDisplay: Draw MC event #" << fEvtMgr->GetEventNumber() << endl;
      TCandListIterator itermc(*fAssociator->GetMcTruthList());
      while ( (c = itermc.Next()) ) {
	TString name = c->PdtEntry()->GetName();
	if (fMcTracks.Index(name) < 0) continue;
	if (!labelMode || fPhi>-80) name = ""; // Only front view works right now
	PAFDisplayMcTrack *track = new  PAFDisplayMcTrack(c, name);
	if (track == 0) continue;
	track->SetScale(labelScale);
	track->SetTextColor(labelColor);
	track->SetTextSize(labelSize);
	track->DrawTrack("same",kFALSE);
	fTracks->Add(track);    // Book keeping
	// Print a track info
	cout << (const char *) name << ":" << track->GetObjectInfo(0,0) << endl;
      }
    }

    cout << endl;
    
}

void PAFDisplay::SetVertexSelector(VAbsVertexSelector *v) 
{
    fVertexSelector = v;
    if (fVertexSelector != 0) fVertexSelector->PrintOn(cout);
}

void PAFDisplay::DumpEvent()
{
    if (fRunDB!=0&&fMgr->GetBoolParm("Logbook")) {
	TLogInfo *loginfo = fRunDB->GetLogInfo(fEvtMgr->GetRunNumber());
	if (loginfo!=0) 
	    loginfo->PrintOn(cout);
	else
	    cout << "No TLogInfo found for run #" << fEvtMgr->GetRunNumber() << endl;
    }
    
    if (fMgr->GetBoolParm("Verbose")) cout << *fEvtMgr; // Event dump
    
    if (fConditions!=0&&fMgr->GetBoolParm("Conditions")) cout << *fConditions; // Conditions dump
    
    TVector3 primVtx = fEvtMgr->GetTag().GetPrimaryVertex(); // Print the primary vertex
    cout << "Primary vertex: " << primVtx.X() << '\t' << primVtx.Y() << '\t' << primVtx.Z() << endl;
}

void PAFDisplay::SetBackgroundColor(UInt_t color) 
{ 
    fMgr->SetIntParm("BgdColor",color); 
    if (fDraw) DrawEvent(); 
}

void PAFDisplay::SetLabelColor(UInt_t color) 
{ 
    fMgr->SetIntParm("LblColor",color); 
    if (fDraw) DrawEvent(); 
}

void PAFDisplay::SetLabelSize(Double_t size) 
{ 
    fMgr->SetDoubleParm("LblSize",size); 
    if (fDraw) DrawEvent(); 
}

void PAFDisplay::SetLabelOffset(Double_t offset) 
{ 
    fMgr->SetDoubleParm("LblOffset",offset); 
    if (fDraw) DrawEvent(); 
}

void PAFDisplay::SetTruthMode(Bool_t yesNo)
{
    static TString oldPid = fMgr->GetStringParm("Pid");

    if (!fTruthMode) {
	fTrigPad->Modified();
        fTruthButton->SetFillColor(kGreen);
        fTruthButton->Modified();
	oldPid = fMgr->GetStringParm("Pid");
	fMgr->SetParm("Pid","Truth");
	fTruthMode = kTRUE;
    }
    else {
	fTrigPad->Modified();
        fTruthButton->SetFillColor(38);
        fTruthButton->Modified();
	fMgr->SetParm("Pid",oldPid);
	fTruthMode = kFALSE;
    }
    
    if (fDraw) {
	DrawEvent();
	DrawMcTruth();
    }
}

void PAFDisplay::SetListMode(Bool_t yesno)
{
    if (yesno) {
	fTrigPad->Modified();
        fListButton->SetFillColor(kGreen);
        fListButton->Modified();
        fRecoButton->SetFillColor(38);
        fRecoButton->Modified();
	fMgr->SetParm("Pid","List");
	fListMode = kTRUE;
    }
    else {
	fTrigPad->Modified();
        fRecoButton->SetFillColor(kGreen);
        fRecoButton->Modified();
        fListButton->SetFillColor(38);
        fListButton->Modified();
	fMgr->SetParm("Pid","Reco");
	fListMode = kFALSE;
    }

    if (fDraw) DrawEvent();
}

void PAFDisplay::DrawVertices(TCandList *list)
{
    static Int_t oldEvent = -1;
    Bool_t newEvent = kFALSE;
    
    if (fEvtMgr->GetEventNumber() != oldEvent) {
	oldEvent = fEvtMgr->GetEventNumber();
        newEvent = kTRUE;
    }

    // Do vertexing of the charged tracks, if needed
    
    if ( fVertexSelector!=0) {
	if (newEvent) {
	    
	    fVertices->Delete();
	    
	    Int_t n = list->GetNumberOfCandidates();
	    for (Int_t comb_i=0;comb_i<n;++comb_i) {
		TCandidate *c1 = list->Get(comb_i);
		if (c1->GetCharge()==0) continue;
		for (Int_t comb_k=comb_i+1;comb_k<n;++comb_k) {
		    TCandidate *c2 = list->Get(comb_k);
		    if (c2->GetCharge()==0 || c2->GetCharge()==c1->GetCharge()) continue;
		    if ( !c1->Overlaps(*c2) ) {
			if (fVertexSelector->Accept(*c1,*c2)) {
			    cout << "Vertexed tracks "<< comb_i+1 << "+" << comb_k+1 << endl;
			    PAFDisplayVertex *vertex = new PAFDisplayVertex(fVertexSelector);
			    if (vertex == 0) continue;
			    vertex->DrawVertex("same",kFALSE);
			    fVertices->Add(vertex);    // Book keeping
			}
		    }
		}
	    }
	}
	else {
	    TIter nextVertex(fVertices);
	    PAFDisplayVertex *vobj;
	    while(( (vobj=(PAFDisplayVertex*)nextVertex())) ) vobj->DrawVertex("same",kTRUE);
	}
	
    }
}  

void PAFDisplay::DrawPrimaryVertex()
{
    // Access and draw the primary vertex
    
    TVector3 primVtx = fEvtMgr->GetTag().GetPrimaryVertex();
    TVector3 err(0.02,0.02,0.02); // Set to 200 micron right now
    
    //Draw the primary vertex as an error sized box
    if (fPrimVtx != 0) delete fPrimVtx;
    fPrimVtx = new TMarker3DBox(primVtx.X(),primVtx.Y(),primVtx.Z(),err.X(),err.Y(),err.Z(),0.,0.);
    fPrimVtx->SetLineColor(kRed);
    fPrimVtx->Draw();
}

void PAFDisplay::DrawBeamspot()
{
    // Get the beam spot and draw it
    
    // Retrieve the actual location of the beam spot and the errors
    // from the conditions database
    
    TVector3 bs, bse;
    
    if (fConditions != 0) {
	TBeams *beams = fConditions->GetBeams();
	
	bs  = beams->Interaction3Point();
	bse = TVector3(beams->InteractionPointErr(0,0),
	    beams->InteractionPointErr(1,1),
	    beams->InteractionPointErr(2,2));
	
	//Draw the beam spot as an error sized box
	if (fSpot != 0) delete fSpot;
	fSpot = new TMarker3DBox(bs.X(),bs.Y(),bs.Z(),bse.X(),bse.Y(),bse.Z(),0.,0.);
	fSpot->SetLineColor(kYellow);
	fSpot->Draw();
    }
}    

Int_t PAFDisplay::Filter(Int_t number)
{
    if (!fTruthMode||fFilter=="") {
	fEvtMgr->NextIter();
	return fEvtMgr->GetEventNumber();
    }

    // Examine MCTruth tracks
    if (number==0) number = fEvtMgr->GetEventNumber() + 1;
    while ( (fEvtMgr->GetEvent(number++)) ) {
      cout << endl << "PAFDisplay: Filter MC event #" << fEvtMgr->GetEventNumber() << endl;
      static TCandList mcList;
      fEvtMgr->FillMcTruthList(&mcList);
      TCandListIterator itermc(mcList);
      TCandidate *c;
      while ( (c = itermc.Next()) ) {
	TString name = c->PdtEntry()->GetName();
	if (fFilter.Index(name) >= 0) { return fEvtMgr->GetEventNumber(); }
      }
    }

    number--;

    return number;
}

Double_t PAFDisplay::PTcut() {return fMgr->GetDoubleParm("ptcut");}
Double_t PAFDisplay::Ecut() {return fMgr->GetDoubleParm("ecut");}

