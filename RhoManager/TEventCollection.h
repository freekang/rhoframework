#ifndef TEVENTCOLLECTION_H
#define TEVENTCOLLECTION_H
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

#include <iostream>
#include <string>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include "RhoBase/VAbsCollection.h"

class TEventBlock;

class TEventCollection : public VAbsCollection {
    
public:
    //Constructors

    TEventCollection(const char* c="collection.root", Bool_t saveTimestamp=kTRUE);

    //Destructor
    virtual ~TEventCollection();
    //public methods
    void Fill(const char* fname, UInt_t n, UInt_t min=0, UInt_t maj=0);
    
    void Open();
    
    void MakeNew(const char*);
    
    void Finish();

    virtual void Rewind();
    virtual Int_t NextIndex(Bool_t &changeFile);
    virtual string GetFileName() const;
    virtual Bool_t InitRead();
    
private:
    TString     fFileName;
    TString     fCurrentFileName; //! Not stored in ROOT file 
    TFile*      fRootfile;   //! Not stored in ROOT file
    TTree*      fTree;       //! Not stored in ROOT file
    UInt_t	fEventNumber;   //! Not stored in ROOT file
    UInt_t	fBlocknum;   //! Not stored in ROOT file
    TEventBlock* fBlock;      //! Not stored in ROOT file
    Int_t *fCurrent;  //! Not stored in ROOT file	
    Bool_t fSaveTimestamp;    //! Not stored in ROOT file	
    Int_t *fStop;    //! Not stored in ROOT file
    Bool_t fRead;    //! Read mode

public:
    ClassDef(TEventCollection,1) // Collection of events
};      

#endif
