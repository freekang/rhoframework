//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventCollection		                                        //
//                                                                      //
// Event collection class						//
//									//
// Author List:								//
// Sascha Berger, Bochum University, Feb. 99				//
// Marcel Kunze,  RUB, Nov. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoManager/TEventCollection.h"
#include "RhoManager/TEventBlock.h"

ClassImp(TEventCollection)

#include <iostream>
using namespace std;

TEventCollection::TEventCollection(const char* c,Bool_t saveTimestamp) :
fRead(kFALSE)
{
    MakeNew(c);
    fSaveTimestamp = saveTimestamp;
}

Bool_t
TEventCollection::InitRead()
{
    fRead = kTRUE;

    fRootfile = new TFile (fFileName);
    if (!fRootfile->IsOpen()) {
	cout << " *** ERROR in TEventCollection ! Could not open File ";
	cout << " ' " << fFileName.Data() << " ' " << endl;
	return kFALSE;
    }
    fTree = (TTree*) fRootfile->Get("Woodstock2");
    if (fTree==NULL) {
	cout << " *** ERROR in TEventCollection ! Could not find Tree 'Woodstock2' ";
	cout << "in File  ' " << fFileName.Data() << " ' " << endl;
	return kFALSE;
    }
    fTree->GetBranch("block")->SetAddress(&fBlock);
    fBlocknum=0;
    fTree->GetEvent(0);
    fCurrentFileName = fBlock->GetFilename();
    fCurrent = fBlock->GetBuffer(); 
    fStop = & ( (fBlock->GetBuffer())[fBlock->GetBufLen()]   );
    
    cout << "TEventCollection: Reading " << fBlock->GetBufLen() << " events from " << fCurrentFileName << endl;

    return kTRUE;
}

Int_t 
TEventCollection::NextIndex(Bool_t &changeFile)
{
    if (fCurrent!=fStop) {
	fCurrentIndex = *fCurrent;
	fCurrent++;
	return fCurrentIndex;
    } else {
	fBlocknum++;
	Int_t nbytes = fTree->GetEvent(fBlocknum);
	if (nbytes==0) {
	    changeFile=kTRUE;
	    fCurrentFileName = TString("");
	    return 0;
	} else {
	    if (strcmp(fCurrentFileName.Data(),fBlock->GetFilename())!=0) {
		fCurrentFileName = TString(fBlock->GetFilename());
		changeFile=kTRUE;
	    }
	    fCurrent = fBlock->GetBuffer();
	    fStop = & ( (fBlock->GetBuffer())[fBlock->GetBufLen()]   );
	    fCurrentIndex = *fCurrent;
	    fCurrent++;
	}

	cout << "TEventCollection: Reading " << fBlock->GetBufLen() << " events from " << fCurrentFileName << endl;
    }
    return fCurrentIndex;
}

void
TEventCollection::Rewind()
{
    fBlocknum=0;
    fTree->GetEvent(0);
    fCurrentFileName = TString(fBlock->GetFilename());
    fCurrent = fBlock->GetBuffer();
    fStop = & ( (fBlock->GetBuffer())[fBlock->GetBufLen()-1]   );
}

string
TEventCollection::GetFileName() const 
{
    if (fCurrentFileName.Length()>0) {
	return fCurrentFileName;
    } else {
	return 0;
    }
}

//--------------
// Destructor --
//--------------
TEventCollection::~TEventCollection() {
    if (fRead) 
	delete fRootfile;
    else {
	Finish();
	delete fBlock; delete fTree;
    }
}

//----------------------
//-- public Functions --
//----------------------

void TEventCollection::Open() {
}

void TEventCollection::MakeNew(const char* s) 
{
    fBlock = new TEventBlock();
    fRootfile = new TFile(s,"recreate");
    fFileName = s;
    fTree = new TTree("Woodstock2","A Collection Tree",1);
    fTree->Branch("block","TEventBlock",&fBlock);
    fEventNumber=0;
    fBlock->ClearBuffer();
    fBlock->SetFilename("");
}

void TEventCollection::Fill(const char* fname, UInt_t index,UInt_t min, UInt_t maj) 
{
    if (!fSaveTimestamp) { min=0; maj=0; }
    if (strcmp(fname,fBlock->GetFilename())==0) {
	if (fBlock->GetBufLen()<999) {
	    fBlock->AddNumber(index,min,maj);
	    fCurrentIndex++;
	    return;
	}
    }
    if (fBlock->GetBufLen()>0) {
	fTree->Fill();
    }
    fBlock->ClearBuffer();
    fBlock->SetFilename(fname);
    fBlock->AddNumber(index,min,maj);
    
    fCurrentIndex++;
}


void TEventCollection::Finish() {
    cout << "TEventCollection::Closing " << fFileName.Data() << " with " << fCurrentIndex << " events." << endl;
    fRootfile->cd();
    fTree->Fill();
    fTree->Write("Woodstock2");
    this->Write("Collection");
    fRootfile->Close();
    delete fRootfile;
    fRootfile = 0;
}

