#ifndef TRUNCOLLECTION_H
#define TRUNCOLLECTION_H
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

#include <iostream>
#include <string>
using namespace std;

#include "RhoBase/VAbsCollection.h"

class TObjArray;
class TRunInfo;
class TDataBlock;

class TRunCollection : public VAbsCollection {
  public :
    TRunCollection();
    TRunCollection(const char *name);
    virtual ~TRunCollection();

    virtual void Rewind();
    virtual Int_t NextIndex(Bool_t &changeFile);
    virtual string GetFileName() const;
    virtual Bool_t InitRead();

    void AddRun(TRunInfo *run);
    void Finish();
    
    void PrintOn( std::ostream& o=std::cout ) const;

  private :
    TObjArray *fEntryList;
    TString fFileName; //! Not stored in root file
    TDataBlock *fCurrentBlock; //! Not stored in root file
    Int_t fCurrentBlockNo;  //! Not stored in root file
    Bool_t fStored;         //! Not stored in root file

  public :
    ClassDef(TRunCollection,1) // T run collection

};

#endif
