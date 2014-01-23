//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRunCollection		                                        //
//                                                                      //
// Run collection class							//
//									//
// Author List:								//
// Thorsten Brandt, Dresden University, Aug. 99				//
// Marcel Kunze,  RUB, Nov. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjArray.h"
#include "TFile.h"

#include "RhoManager/TRunCollection.h"
#include "RhoManager/TDataBlock.h"
#include "RhoManager/TRunInfo.h"

ClassImp(TRunCollection)

#include <iostream>
using namespace std;

TRunCollection::TRunCollection() :
  fEntryList(0), fCurrentBlock(0),fCurrentBlockNo(0)
{ 
  fCurrentIndex=0;
  fEntryList = new TObjArray();
  fStored = kTRUE;
}

TRunCollection::~TRunCollection()
{
  fEntryList->Delete();
  delete fEntryList;
}

TRunCollection::TRunCollection(const char *name) :
 fEntryList(0), fCurrentBlock(0),fCurrentBlockNo(0),fStored(kFALSE)
{
  fFileName = TString(name);
  fEntryList = new TObjArray();
}

void
TRunCollection::Rewind() 
{
  fCurrentBlockNo=0;
  fCurrentIndex=-1;
  fCurrentBlock = (TDataBlock*) (*fEntryList)[0];
  fCurrentIndex = fCurrentBlock->Min()-1;
}


Int_t
TRunCollection::NextIndex(Bool_t &changeFile) 
{
  if (fCurrentIndex < fCurrentBlock->Max()) {
    fCurrentIndex++;
    return fCurrentIndex;
  } else {
    fCurrentBlockNo++;
    if (fCurrentBlockNo < fEntryList->GetEntries()) {
      TDataBlock *b = fCurrentBlock;
      fCurrentBlock = (TDataBlock*) (*fEntryList)[fCurrentBlockNo];
      fCurrentIndex = fCurrentBlock->Min();
      if (strcmp(b->GetFilename(),fCurrentBlock->GetFilename())) {
	changeFile =kTRUE;
      }
    } else {
      changeFile=kTRUE;
      fCurrentIndex=-1;
    }
  }
  return fCurrentIndex;
}

string
TRunCollection::GetFileName() const 
{
  if (fCurrentBlockNo < fEntryList->GetEntries()) {
     return fCurrentBlock->GetFilename();
  } else {
    return "";
  }
}

void
TRunCollection::AddRun(TRunInfo *run)
{
  TString fileName = TString(run->GetCluster()) + "/" + run->GetFile();
  TDataBlock *b= new TDataBlock(fileName,run->GetFirst(),run->GetLast());
  fEntryList->Add(b);
  fStored = kFALSE;
}

void 
TRunCollection::Finish()
{
  TString fullName(fFileName.Data());
  TFile f1(fFileName.Data(),"RECREATE");
  fEntryList->Sort();
  Write("Collection");
  fStored = kTRUE;
}

Bool_t
TRunCollection::InitRead()
{
  if (!fStored) {
    fEntryList->Sort();
  } else {
    fEntryList->Sort(0);
  }
  Rewind();
  return kTRUE;
}
  
void
TRunCollection::PrintOn(std::ostream &o) const
{
  //fEntryList->Sort();
  o << "------------- RUN - COLLECTION ---------" << endl;
  for (Int_t i=0; i<fEntryList->GetEntries(); i++) {
    TDataBlock *b = (TDataBlock*) (*fEntryList)[i];
    b->PrintOn(o);
  }
  o << "--------------------------------------- " << endl;
}
