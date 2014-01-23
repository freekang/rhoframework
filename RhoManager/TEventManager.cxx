//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventManager							//
//                                                                      //
// Event manager					   		//
//                                                                      //
// Author: Marcel Kunze, Bochum University, March 99			//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////
//#define DEBUG

#include <stdlib.h>
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/VAbsBit.h"
#include "RhoBase/VAbsReader.h"
#include "RhoBase/VAbsWriter.h"
#include "RhoBase/VAbsCollection.h"
#include "RhoManager/TEventManager.h"
#include "RhoManager/TDirectoryManager.h"
#include "RhoManager/TAssociator.h"

#include "RhoBase/TFactory.h"
#include "RhoBase/TEventInfo.h"
#include "RhoBase/TSimpleVertex.h"
#include "RhoConditions/TRhoTime.h"
#include "RhoConditions/TBeams.h"

#include "PAFSchema/PAFReader.h"
//#include "PAFSchema/PAFWriter.h"
#include "KangaSchema/KangaReader.h"

ClassImp(TEventManager)

#include <iostream>
using namespace std;

//----------------
// Constructors --
//----------------

TEventManager::TEventManager() : 
fReader(0),
fDirMgr( 0 ),
fDictionary(0),
fCounter(-1),
fTotal(0),
fActiveCollection( kFALSE ),
fChange( kFALSE ),
fCollection(0),
fCollectionFile(0),
fKanga(kFALSE)
{ 
    fTagExpr = TString("");
    fBitExpr = TString("");
}

//--------------
// Destructor --
//--------------
TEventManager::~TEventManager( ) 
{
}

//--------------
// Operations --
//--------------

TRhoTime TEventManager::GetTime() const 
{
    return TRhoTime(fReader->GetMajorID());
}

// _______ Fill TCandLists __________________________________

Int_t TEventManager::Fill(TCandList *l) 
{
    // Generate charged candidates and fill them into a list
    return fReader->Fill(l);
}

Int_t TEventManager::FillCharged(TCandList *l) 
{
    // Generate charged candidates and fill them into a list
    return fReader->FillCharged(l);
}

Int_t TEventManager::FillNeutral(TCandList *l) 
{
    // Generate neutral candidates and fill them into a list
    return fReader->FillNeutral(l);
}

Int_t TEventManager::FillCompositeList(TCandList *l,const char* listname)
{
    if (listname == 0)
	return fReader->FillAllCompositeList(l);
    else
	return fReader->FillCompositeList(l,listname);
} 

Int_t TEventManager::FillFromList(TCandList *l,const char* listname)
{
    return fReader->FillFromList(l,listname);
} 

//-------------------------------------------  TRUTH   -----------------------------

Int_t TEventManager::FillMcTruthList(TCandList *l, TAssociator *assoc)
{
    Int_t n = fReader->FillMcTruthList(l);
    if (n>0 && assoc!=0) assoc->Associate(*l);
    return n;
}



VAbsMicroCandidate& TEventManager::operator[](Int_t i) const
{
    // Return a handle to the Micro of the i-th track
    
    return fReader->GetMicroCandidate(i);
}

void TEventManager::InitRead(const char* dir) 
{
    // Open a single database location
    SetInputFile(dir);
}

void TEventManager::InitRead(TDirectoryManager *dm) 
{
    // Work with sets of database files using a directory manager
    // This method is for compatibility; new: UseCollection
    fDirMgr = dm;
    UseCollection(dm);
}

void
TEventManager::AddEventToCollection(VAbsCollection *theColl) 
{
    // Add the event to the collection. Kanga files carry the suffix.
    static const TString suffix("-micro");
    TString name(GetCurrentFilename());
    if (fKanga) name += suffix;
    
    theColl->Fill(name,GetCurrentIndex(),GetMinorID(),GetMajorID() );    
}

void TEventManager::PrintOn(std::ostream& o) const 
{
    // Dump the contents of the current event
    
    o << endl;
    o << "-------------------------------------------------------" << endl;
    o << "Event dump for event taken at " << GetTime() << " (GMT)" << endl;
    
    if (Tag())
	GetTag().PrintOn(o);
    
    if (Aod())
	for (int i=0;i<GetNumberOfTracks();++i) {
	    const VAbsMicroCandidate &cand = fReader->GetMicroCandidate(i);
	    cout << "\nCandidate " << i+1 << " (Charge:" << cand.GetCharge() << ")" << endl;
	    cand.PrintOn(o);
	}
}

// Private conversion functions

UInt_t
TEventManager::GetCurrentIndex() 
{
    if (fActiveCollection) {
	return fCollection->GetIndex();
    } else {
	return fCounter;
    }
}

const char* 
TEventManager::GetCurrentFilename()
{
    string name;
    
    if (fActiveCollection) 
    {
	if (fCollection==0) return "";
	name = fCollection->GetFileName();
    } 
    else 
    {
	name = fFile.c_str();	
    }
    
	return name.c_str();
}

UInt_t TEventManager::GetNumberOfEventsProcessed() const {
    // Return the number of events currently processed
    return fReader->GetNumberOfEventsProcessed();
}

UInt_t TEventManager::GetNumberOfTracks() const {
    // Return the total number of tracks in the event
    return fReader->GetNumberOfTracks();
}

UInt_t TEventManager::GetNumberOfChargedTracks() const {
    // Return the number of charged tracks in the event
    return fReader->GetNumberOfChargedTracks();
}

UInt_t TEventManager::GetNumberOfNeutralTracks() const {
    // Return the number of neutral tracks in the event
    return fReader->GetNumberOfNeutralTracks();
}

UInt_t TEventManager::GetRunNumber() const {
    // Return the actual run number
    return fReader->GetRunNumber();
}

UInt_t TEventManager::GetEventNumber() const {
    // Return the actual event number
    return fReader->GetEventNumber();
}

UInt_t TEventManager::GetMajorID() const {
    return fReader->GetMajorID();
}

UInt_t TEventManager::GetMinorID() const {
    return fReader->GetMinorID();
}


// Access to object handles

PAFEventBase& TEventManager::GetHeader() const {
    // Return a handle to the event header
    return fReader->GetHeader();
}    

VAbsTag& TEventManager::GetTag() const {
    // Return a handle to the TAG
    return fReader->GetTag();
}

VAbsBit& TEventManager::GetBit() const {
    // Return a handle to the BIT
    return fReader->GetBit();
}    

EidInfoR& TEventManager::GetEidInfo() const {
    // Return a handle to the EidInfo
    return fReader->GetEidInfo();
}    

PAFListBase& TEventManager::GetListBase() const {
    return fReader->GetListBase();
}

Bool_t TEventManager::Next()
{ 
    // Get the next event either from a file or a collection
    
    if ( fActiveCollection ) {	// Read a collection
	// Find out next index
	fCounter = fCollection->NextIndex(fChange);
	
	// Check, if the file has changed
	if (fChange) {
	    string newFile = fCollection->GetFileName();
	    if (newFile.length()!=0) { // more to do ?
		SetInputFile(newFile);
		fChange=kFALSE;
	    } else {
		return kFALSE;	// EoFs
	    }
	}
	
    }
    
    else
	fCounter++;   // Read from single file
    
    return (GetEvent(fCounter)!=0);
}

Int_t TEventManager::GetEvent(UInt_t n)
{
    return fReader->GetEvent(n);
}

void TEventManager::UseCollection(VAbsCollection *c)
{
    fChange=kFALSE;
    if (fCollectionFile!=0) { delete fCollectionFile; fCollectionFile=0; }
    fActiveCollection=kTRUE;
    fCollection = c;
    c->InitRead();
    string f = c->GetFileName();
	if (f.length()!=0) {
		SetInputFile(f);
    }
}

void TEventManager::UseCollection(const char* filename)
{
    fChange=kFALSE;
    if (fCollectionFile!=0) { delete fCollectionFile; fCollectionFile=0;}
    fCollectionFile = new TFile(filename);
    if (!fCollectionFile->IsOpen()) {
	cout << " *** ERROR in TEventManager::UseCollection :";
	cout << " Cannot open file '" << filename << "' " << endl;
	delete fCollectionFile; fCollectionFile=0;
	return;
    }
    fCollection = (VAbsCollection*) fCollectionFile->Get("Collection");
    if (fCollection==0) {
	cout << " *** ERROR in TEventManager::UseCollection :";
	cout << " Collection in " << filename << " must have Key 'Collection'" << endl;
	delete fCollectionFile; fCollectionFile=0;
	return;
    }
    if (!fCollection->InitRead()) {
	cout << " *** ERROR in TEventManager::UseCollection :";
	cout << " Could not initialise Collection in file " << filename << endl;
	delete fCollectionFile; fCollectionFile=0;
	return;
    }
    fActiveCollection=kTRUE; 
    if (TString(fCollection->GetFileName()).Length()!=0) {
	SetInputFile(fCollection->GetFileName());
    }
}

void TEventManager::SetInputFile(string filename)
{
    filename = FullyQualified(filename);	// Note the current file
	fFile = filename;
    
    // Check the mode of operation (Kanga or PAF)
    
	int index = fFile.find("Kanga");
    fKanga = (index >= 0);
    if (fFile.find("-micro")==fFile.length()-6) {
		fFile = fFile.substr(0,fFile.length()-6); // Remove the suffix
	fKanga = kTRUE;
    }
    
    if (fReader != 0) delete fReader; // Get rid of the old reader
    
    if (fKanga)
	fReader = new KangaReader(fFile.c_str());
    else
	fReader = new PAFReader(fFile.c_str());

    // Activate the streams to process
    TParameterManager *parmgr = TRho::Instance()->GetParameterManager();
    if (parmgr != 0) {
	fReader->SetStreamStatus("tag",parmgr->GetBoolParm("tag"));
	fReader->SetStreamStatus("aod",parmgr->GetBoolParm("aod")||parmgr->GetBoolParm("micro"));
	fReader->SetStreamStatus("mct",parmgr->GetBoolParm("mct"));
	fReader->SetStreamStatus("map",parmgr->GetBoolParm("map")||parmgr->GetBoolParm("readMaps"));
	fReader->SetStreamStatus("cmp",parmgr->GetBoolParm("cmp")||parmgr->GetBoolParm("readCmpList"));
    }
    
    fReader->InitializeStreams(parmgr->GetBoolParm("multi")); // Open the file

    // Set the tagcut formula
    fReader->SetBitFormula(fBitExpr);
    fReader->SetTagFormula(fTagExpr);
}

string TEventManager::FullyQualified(string s)
{
    // Check that we have a fully qualified Unix or Windows filename
    
    if (s.c_str()[0] == '/') return s; // UNIX
    if (s.c_str()[0] == '$') return s; // Environment
    if (s.c_str()[0] == '~') return s; // Home directory
    if (s.c_str()[0] == '.') return s; // Absolute path
    if (s.c_str()[1] == ':') return s; // Windows drive letter
    if (s.find("root:")==0) return s; // ROOTD file
    
	string base = TRho::Instance()->GetEventStore();
    s = base+"/"+s; // Prepend the base directory
    
    return s;
}

void TEventManager::SetTagFormula(const char* tagExpr)
{
    fTagExpr = tagExpr;
}

void TEventManager::SetBitFormula(const char* tagExpr)
{
    fBitExpr = tagExpr;
}

Double_t TEventManager::EvalBitFormula()
{
    return fReader->EvalBitFormula();
}

Double_t TEventManager::EvalTagFormula()
{
    return fReader->EvalTagFormula();
}

VAbsMicroCandidate& TEventManager::GetMicroCandidate(Int_t n) const
{
    return fReader->GetMicroCandidate(n);
}

VAbsTruth& TEventManager::GetMcTruth(Int_t n)
{
    return fReader->GetMcTruth(n);
}

UInt_t TEventManager::GetNumberOfMcTruthTracks() const
{
    return fReader->GetNumberOfMcTruthTracks();
}

UInt_t TEventManager::GetNumberOfLists()
{
    return fReader->GetNumberOfLists();
}

const char* TEventManager::GetListName(Int_t i)
{
    return fReader->GetListName(i);
}

TEventInfo* TEventManager::GetEventInfo() const
{
    // Assemble an event info
    // Check the event number and return cache
    static TEventInfo *eventInfo = new TEventInfo; // Generate the event info object
    static int oldEvent = -1;
    if (GetEventNumber() == oldEvent) return eventInfo;
    oldEvent = GetEventNumber();

    TConditions *theConditions = TRho::Instance()->GetConditions();
    theConditions->At(GetMajorID());	// Read the beam conditions DB

    TBeams *theBeams = theConditions->GetBeams();
        
    // First, set the CM frame
    eventInfo->SetCmFrame(theBeams->Total4Momentum());
    
    // Second, set the beam spot and its error
    eventInfo->SetBeamSpot(theBeams->Interaction3Point());
    
    // Third, set the electron and positron beam
    eventInfo->SetPepBeams(TCandidate(theBeams->EMinus4Momentum()),TCandidate(theBeams->EPlus4Momentum()));
    
    // Set the primary vertex
    TVector3 pv = GetTag().GetPrimaryVertex();
    static VAbsVertex *vertex = 0;
    if (vertex!=0) delete vertex;
    vertex = new TSimpleVertex(pv);
    eventInfo->SetPrimaryVertex(vertex);

    return eventInfo;
}

void TEventManager::WriteEvent(VAbsWriter *writer, TCandList *list)
{
    writer->WriteEvent(this,list);
}

// --------------------
// non-member functions
// --------------------

std::ostream&  operator << (std::ostream& o, const TEventManager& a) {a.PrintOn(o); return o;}

