//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDirectoryManager							//
//                                                                      //
// Directory manager					    		//
// Organize chains of files according to physics topics and periods	//
// of interest.								//
//                                                                      //
// Author: Marcel Kunze, Bochum University, April 99			//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "RhoManager/TDirectoryManager.h"
#include "RhoBase/TRho.h"

ClassImp(TDirectoryManager)

#include <iostream>
using namespace std;

TDirectoryManager::TDirectoryManager() : 
fBaseDirectory(TRho::Instance()->GetEventStore()), fNumberFiles(0), 
fCounter(0),fActualFile(0), fEntries(0)
{    
    TRho *rho = const_cast<TRho*> (TRho::Instance());
    rho->SetDirectoryManager(this);
}

TDirectoryManager::TDirectoryManager(const char* directory) : 
fBaseDirectory(directory), fNumberFiles(0), 
fCounter(0),fActualFile(0), fEntries(0)
{
    TRho *rho = const_cast<TRho*> (TRho::Instance());
    rho->SetDirectoryManager(this);
}

TDirectoryManager::~TDirectoryManager()
{
}

void TDirectoryManager::AddFile(const char* s) 
{
    TString name = ExpandFilename(s);
    fFiles.Add(new TObjString(name));
    fNumberFiles++;
    cout << "TDirectoryManager::AddFile " << name.Data() << endl;
}


void TDirectoryManager::PrintOn ( std::ostream & oout ) const {
    // Dump a directory of what's going on
    oout << "TDirectoryManager: " << fNumberFiles << " file(s) in " << fBaseDirectory << endl;
    for (int o=0;o<fNumberFiles;o++) oout << (((TObjString*) fFiles.At(o))->GetString()).Data() << endl;
    oout << endl;
}

// Here comes the VAbsCollection interface

void TDirectoryManager::Rewind() 
{ 
    fCounter = 0;
    fEntries = 0;
    fActualFile = 0;
}

Int_t TDirectoryManager::NextIndex(Bool_t &changeFile) 
{
    if (fCounter >= fEntries) {
	changeFile = kTRUE;
	fCounter = 0;
	fActualFile++;
	if (fActualFile>=fNumberFiles) return -1;
	while (!InitRead()) {
	    cerr << "TDirectoryManager::NextIndex: Can not open " << ((TObjString*) fFiles.At(fActualFile-1))->GetString() << endl;
	    Bool_t newFile;
	    if (NextIndex(newFile)==-1) {
		cerr << "TDirectoryManager::NextIndex: No valid collection" << endl;
		return -1;
	    }
	}
    }

    fCurrentIndex = fCounter;
    return fCounter++; 
}

string TDirectoryManager::GetFileName() const 
{
    if (fFiles.At(fActualFile) == 0) return "";
	TObject *o = fFiles.At(fActualFile);
	TObjString* os = (TObjString*) o;
	string s = string(os->GetString().Data());
	return s;
}

Bool_t TDirectoryManager::InitRead() 
{
    // Count the entries in the file
    fEntries = 0;
    TString filename, treename;
    Bool_t status = kTRUE;

    TString s(((TObjString*) fFiles.At(fActualFile))->GetString());

    if (s.Index("-micro")==s.Length()-6) { // A Kanga file
	filename = s + TString(".root");
	treename = "microDST";
    }
    else if (s.Index("Kanga")>=0) { // Kanga shortcut
        filename = s + TString("-micro.root");
        treename = "microDST";
    }
    else {
	filename = s + TString("Tag.root"); // A PAF file
	treename = "TagTree";
    }

    if (fBaseDirectory.Index("root:")>=0) TRho::Authorize();

    TFile *fFile = TFile::Open(filename);

    if (fFile->IsOpen()) {
      
      TTree* newTree = (TTree*) fFile->Get(treename);
      
      if (newTree==0) {
	cout << "ERROR ! Could not find " << treename.Data() 
	     << " in File " << filename.Data() << endl;
	fFile->Close();
	status = kFALSE;
      }

      fEntries = (Int_t) newTree->GetEntries();
    }
    else
      status = kFALSE; // File does not exist

    fFile->Close();

    if (!status) { // Try another file
	Bool_t newFile;
	if (NextIndex(newFile)!=-1) status = kTRUE;
    }

    return status;
}

// --------------------
// non-member functions
// --------------------

std::ostream&  operator << (std::ostream& o, const TDirectoryManager& a) {a.PrintOn(o); return o;}

// Experiment specific directory conversion routine

TString TDirectoryManager::ExpandFilename(TString name)
{
    // Special treatment of BFROOT
    if (name.Index("BFROOT")>=0) {
	TString base="";
	if (getenv("BFROOT")) 
	    base = getenv("BFROOT");
	else if (getenv("RHOROOT"))
	    base = getenv("RHOROOT");
	

	if (base!="") {
	    if (name.Index("$env(BFROOT)")>=0) name = base + name(12,name.Length()-12);
	    if (name.Index("$BFROOT")>=0) name = base + name(7,name.Length()-7);
	}
	else {
	    cerr << "TDirectoryManager: BFROOT and RHOROOT not set. Unable to locate " << name << endl;
	}
    }

    Bool_t kanga =  name.Index("Kanga") >= 0;

    // It could as well be that a Kanga file is given relative to $EVENT_STORE with a leading "/"
    if (kanga && name.Index("/groups")==0) name = fBaseDirectory + name;
    if (kanga && name.Index("/user")==0)   name = fBaseDirectory + name;
    if (kanga && name.Index("/system")==0) name = fBaseDirectory + name;

    return FullyQualified(name);
}

TString TDirectoryManager::FullyQualified(TString s)
{
    // Check that we have a fully qualified Unix or Windows filename

    if (s.Data()[0] == '/') return s; // UNIX
    if (s.Data()[0] == '$') return s; // Environment
    if (s.Data()[0] == '~') return s; // Home directory
    if (s.Data()[0] == '.') return s; // Absolute path
    if (s.Data()[1] == ':') return s; // Windows drive letter
    if (s.Index("root:")==0) return s; // ROOTD file

    s = fBaseDirectory+"/"+s; // Prepend the base directory

    return s;
}

