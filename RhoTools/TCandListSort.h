#ifndef TCANDLISTSORT_H
#define TCANDLISTSORT_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// File and Version Information:					//
//   TCandListSort.h							//
//                                                                      //
// Description:								//
//   Operates on a list of TCandidates to sort them according to	//
//   momentum or cluster energy, highest first.				//
//                                                                      //
// Usage:								//
//   TCandidList aList;							//
//   // ..and fill your list....					//
//   TCandListSort mylistsort; // to instantiate a TCandListSort object	//
//   mylistsort.p(aList); // to sort list according to momentum		//
//   mylistsort.Ecal(aList); // sort list according to cluster energy	//
//									//
//   Clearly, the momentum sort is very general. However, the Ecal sort	//
//   requires the presence of the CalQual info at Micro level.		//
//                                                                      //
// Original Author     Theresa Champion					//
//                                                                      //
// ROOT Version by Marcel Kunze, RUB					//
//////////////////////////////////////////////////////////////////////////

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "TObject.h"
class TCandList;

//              ---------------------
//              -- Class Interface --
//              ---------------------
 
class TCandListSort : public TObject {

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  TCandListSort();
  
  // Destructor
  virtual ~TCandListSort();
  
  // Member functions
  void P (TCandList& theList);
  void Ecal (TCandList& theList);

public:
    ClassDef(TCandListSort,1) // Sort TCandidates according to momentum or cluster energy
};

#endif
