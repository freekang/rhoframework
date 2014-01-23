//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TIndexCollection		                                        //
//                                                                      //
// Event collection class						//
//									//
// Author List:								//
// Sascha Berger, Bochum University, Feb. 99				//
// Marcel Kunze,  RUB, Nov. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoManager/TIndexCollection.h"

ClassImp(TIndexCollection)

#include <iostream>
using namespace std;

TIndexCollection::TIndexCollection(const char* c) {
    MakeNew(c);
}

TIndexCollection::TIndexCollection() {
  fCurrentIndex=-1;
  fFilename = TString("collection.root");
  fRootfile=0;
  fTree=0;
  fCurrentIndex=0;
  fMajorID=0;
  fMinorID=0;
}


Bool_t
TIndexCollection::InitRead()
{
  fRootfile = new TFile (fFilename);
  if (!fRootfile->IsOpen()) {
    cout << " *** ERROR in TIndexcollection ! Could not open File ";
    cout << " ' " << fFilename.Data() << " ' " << endl;
    return kFALSE;
  }
  fTree = (TTree*) fRootfile->Get("Woodstock");
  if (fTree==NULL) {
    cout << " *** ERROR in TIndexcollection ! Could not find Tree 'Woodstock' ";
    cout << "in File  ' " << fFilename.Data() << " ' " << endl;
    return kFALSE;
  }
  fTree->SetBranchAddress("fEventNumber",&fCurrentIndex);
  fTree->SetBranchAddress("fMajorID",&fMajorID);
  fTree->SetBranchAddress("fMinorID",&fMinorID);
  return kTRUE;
}

Int_t 
TIndexCollection::NextIndex(Bool_t &changeFile)
{
  fEventNumber++;
  Int_t nbytes = fTree->GetEvent(fEventNumber);
  if (nbytes==0) {
    changeFile=kTRUE;
  }
  return fCurrentIndex;
}

void
TIndexCollection::Rewind()
{
  fEventNumber=0;
}

string
TIndexCollection::GetFileName() const 
{
  return fFilename;
}

//--------------
// Destructor --
//--------------
TIndexCollection::~TIndexCollection() {

}

//----------------------
//-- public Functions --
//----------------------

void TIndexCollection::Open() {
}

void TIndexCollection::MakeNew(const char* s) {
    fRootfile = new TFile(s,"recreate");
    fFilename = s;
    fTree = new TTree("Woodstock","A Collection Tree");
    fTree->Branch("fEventNumber",&fEventNumber,"fEventNumber/i");
    fTree->Branch("fMajorID",&fMajorID,"fMajorID/i");
    fTree->Branch("fMinorID",&fMinorID,"fMinorID/i");
}

void TIndexCollection::Fill(const char *file, UInt_t l, UInt_t major, UInt_t minor) {
    fEventNumber = l;
    fMajorID  = major;
    fMinorID  = minor;
    fTree->Fill();
}


void TIndexCollection::Finish() {
    fRootfile->cd();
    fTree->Write("Woodstock");
    this->Write("Collection");
    fRootfile->Close();
    delete fRootfile;
}

