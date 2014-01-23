#ifndef VABSCOLLECTION_H
#define VABSCOLLECTION_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsCollection	                                                //
//                                                                      //
// Definition of an abstract interface for event and run collections    //
//                                                                      //
// Author: TB (TUD)							//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <string>
using namespace std;

#include "TObject.h"

class VAbsCollection : public TObject 
{
  public :
      VAbsCollection() : fCurrentIndex(0), fok(kFALSE) {}
    virtual ~VAbsCollection() { };

    virtual void Rewind() = 0;
    Int_t GetIndex() { return fCurrentIndex; }
    virtual Int_t NextIndex(Bool_t &changeFile) = 0;
    virtual string GetFileName() const  =0;
    virtual Bool_t InitRead();

    virtual void Fill(const char* fname, UInt_t n, UInt_t min=0, UInt_t maj=0) {}

    ClassDef(VAbsCollection,1) // Event collection base class

 protected:
    Bool_t fok;
    Int_t fCurrentIndex;
};

#endif
