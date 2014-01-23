#ifndef TINDEXCOLLECTION_H
#define TINDEXCOLLECTION_H
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

#include <iostream>
#include <string>
using namespace std;

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "RhoBase/VAbsCollection.h"

class TIndexCollection : public VAbsCollection{
    
public:
    //Constructors

    TIndexCollection();
    TIndexCollection(const char* c);

    //Destructor
    virtual ~TIndexCollection();
    //public methods
    void Fill(const char* fname, UInt_t, UInt_t = 0, UInt_t = 0);
    
    void Open();
    
    void MakeNew(const char*);
    
    void Finish();

    virtual void Rewind();
    virtual Int_t NextIndex(Bool_t &changeFile);
    virtual string GetFileName() const;
    virtual Bool_t InitRead();
    
private:
    TString     fFilename;
    TFile*      fRootfile;   //! Not stored in ROOT file
    TTree*      fTree;       //! Not stored in ROOT file
    UInt_t	fEventNumber;   //! Not stored in ROOT file
    UInt_t	fMajorID;    //! Not stored in ROOT file
    UInt_t	fMinorID;    //! Not stored in ROOT file

public:
    ClassDef(TIndexCollection,1) // Collection of event indices
};      

#endif
