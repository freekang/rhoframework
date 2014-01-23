#ifndef TDIRECTORYMANAGER_H
#define TDIRECTORYMANAGER_H
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

#include <iostream>
#include <string>
using namespace std;

#include "TFile.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TList.h"
#include "RhoBase/VAbsCollection.h"

class TDirectoryManager : public VAbsCollection 
{
    
public:
    TDirectoryManager();
    TDirectoryManager(const char* directory);
    virtual ~TDirectoryManager();               
    void SetDataDirectory(const char* directory) { fBaseDirectory = directory; }
    const char* GetDataDirectory() const { return fBaseDirectory; }
    void AddFile(const char*);
    TObjArray  GetFileList() { return fFiles;}
    Int_t GetNumberOfFiles() const { return fNumberFiles; }
    void PrintOn ( std::ostream& o=std::cout ) const;
    TDirectoryManager& operator=(const TDirectoryManager &v ) { return *this; }

//  Collection interface
    virtual void Rewind();
    virtual Int_t NextIndex(Bool_t &changeFile);
    virtual string GetFileName() const;
    virtual Bool_t InitRead();

protected:
    virtual TString ExpandFilename(TString name);

private:
    TString FullyQualified(TString name);
    TString fBaseDirectory;	// Location of database
    TObjArray fFiles;		// List of filenames
    Int_t fNumberFiles;		// Number of files
    Int_t fCounter;
    Int_t fActualFile;
    Int_t fEntries;
    
    ClassDef(TDirectoryManager,1) // Directory manager
};      

// standalone print
std::ostream&  operator << (std::ostream& o, const TDirectoryManager&);

#endif

