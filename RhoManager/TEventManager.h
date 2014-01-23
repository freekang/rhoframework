#ifndef TEVENTMANAGER_H
#define TEVENTMANAGER_H
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

#include <iostream>
#include <string>
using namespace std;

#include "TFile.h"
#include "TString.h"
#include "TBranch.h"
#include "TTreeFormula.h"
#include "RhoConditions/TRhoTime.h"
#include "RhoBase/TRho.h"
#include "RhoBase/VAbsTag.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoManager/TParameterManager.h"

class TCandList;
class TDirectoryManager;
class TEventInfo;
class VAbsCollection;
class VAbsTruth;
class PAFProxy;
class TAssociator;
class PAFEventBase;
class VAbsBit;
class PAFListBase;
class EidInfoR;
class VAbsReader;
class VAbsWriter;

//   ---------------------
//   -- Class Interface --
//   ---------------------
class TEventManager : public TObject {
    
    //--------------------
    // Instance Members --
    //--------------------
    
public:
    
    // Constructors
    TEventManager();
    virtual ~TEventManager( );
    
    // Operations
    
    // Tag access
    VAbsTag&	GetTag() const;    
    VAbsBit&	GetBit() const;    

    // Eid Access
    EidInfoR&     GetEidInfo() const;
    PAFEventBase& GetHeader() const;    
    UInt_t GetMajorID() const;
    UInt_t GetMinorID() const;
    TRhoTime GetTime() const;

    // List access
    PAFListBase&  GetListBase() const;

    // Event status
    UInt_t GetRunNumber() const;    
    UInt_t GetEventNumber() const;    
    UInt_t GetNumberOfTracks() const;   
    UInt_t GetNumberOfChargedTracks() const;   
    UInt_t GetNumberOfNeutralTracks() const;   
    UInt_t GetNumberOfCandidates() const 
	{ return GetNumberOfTracks(); }   
    UInt_t GetNumberOfEventsProcessed() const;
    UInt_t GetNumberOfMcTruthTracks() const;
    UInt_t GetNumberOfLists();
    const char* GetListName(Int_t i);
    TEventInfo* GetEventInfo() const;
   
    // ProxyDict access
    PAFProxy* SetProxyDict(PAFProxy* dict) { return fDictionary = dict; }
    PAFProxy* GetProxyDict() { return fDictionary; }

    // Data Management
    void InitRead(const char* s);
    void InitRead(TDirectoryManager *);
    void UseCollection(VAbsCollection *collection);
    void UseCollection(const char* filename);
    Int_t GetEvent(UInt_t);    
    Bool_t Next(); 
    Bool_t NextIter() { return Next(); }
    TDirectoryManager* GetDirectoryManager() { return fDirMgr; }
    const char* GetCurrentFilename();
    UInt_t  GetCurrentIndex();
    VAbsReader *GetReader() { return fReader; }

    // Evaluation of tag based cuts
    void SetTagFormula(const char* expr);
    void SetBitFormula(const char* expr);
    Double_t EvalTagFormula();
    Double_t EvalBitFormula();

    // Fill TCandLists from micro
    Int_t Fill( TCandList* );
    Int_t FillCharged( TCandList* );
    Int_t FillNeutral( TCandList* );
    Int_t FillFromList(TCandList *l,const char* listname);
    Int_t FillCompositeList(TCandList *l,const char* listname);
    Int_t FillMcTruthList(TCandList *, TAssociator* assoc = 0);

    // Access Aod of Cand #n ; Charged cands are numberd 0..nc-1, neutrals nc .. nt
    // (nc = # charged cands, nt = # of all cands)			     
    VAbsMicroCandidate& GetMicroCandidate(Int_t n) const;
    VAbsMicroCandidate& operator[](Int_t) const;  	
    VAbsTruth& GetMcTruth(Int_t);
    void AddEventToCollection(VAbsCollection *theColl);
    void WriteEvent(VAbsWriter *fWriter, TCandList *list=0);
    void PrintOn( std::ostream& o=std::cout ) const;

    // Leave this for backwards compatibility (Obsolete !!)
    Bool_t Tag() const { return TRho::Instance()->GetParameterManager()->GetBoolParm("tag"); }
    Bool_t Aod() const { return TRho::Instance()->GetParameterManager()->GetBoolParm("aod"); }
    Bool_t Mct() const { return TRho::Instance()->GetParameterManager()->GetBoolParm("mct"); }
    Bool_t Map() const { return TRho::Instance()->GetParameterManager()->GetBoolParm("map"); }
    void EnableTag() { TRho::Instance()->GetParameterManager()->SetParm("tag",kTRUE); }
    void EnableAod() { TRho::Instance()->GetParameterManager()->SetParm("aod",kTRUE); }
    void EnableMct() { TRho::Instance()->GetParameterManager()->SetParm("mct",kTRUE); }
    void EnableMap() { TRho::Instance()->GetParameterManager()->SetParm("map",kTRUE); }
    void DisableTag() { TRho::Instance()->GetParameterManager()->SetParm("tag",kFALSE); }
    void DisableAod() { TRho::Instance()->GetParameterManager()->SetParm("aod",kFALSE); }
    void DisableMct() { TRho::Instance()->GetParameterManager()->SetParm("mct",kFALSE); }
    void DisableMap() { TRho::Instance()->GetParameterManager()->SetParm("map",kFALSE); }


protected:
    void CreateVAbsTruthList();
    
private:
    void SetInputFile(string filename);
    string FullyQualified(string);

    VAbsReader		    *fReader;	// !Concrete implementation
    TDirectoryManager*	    fDirMgr;	// !The actual directory manager
    string		    fFile;	// !Name of database file
    Bool_t		    fKanga;	// !File type
    Bool_t		    fActiveCollection,  // !Flag for collection mode
			    fChange;		// !Flags need to open the next file
    TFile		    *fCollectionFile;   // !File the Collection is stored				      
    VAbsCollection	    *fCollection;       // !Holds Object describing collection

    PAFProxy*		    fDictionary;    //! ProxyDict for option 7;

    TString      fTagExpr;      // Dynamic tag formula				     
    TString      fBitExpr;      // Dynamic tag formula				     

    Int_t   fCounter,fTotal;	// !Event counters

    ClassDef(TEventManager,1)	// Event manager
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TEventManager&);

#endif




