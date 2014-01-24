// Macro to load the Rho shared libs
{
#include <iostream.h>

// This is the Windows section
#ifdef WIN32
    gSystem->Load("$RHO/lib/RhoMath.dll");
    gSystem->Load("$RHO/lib/RhoConditions.dll");
    gSystem->Load("$RHO/lib/RhoBase.dll");
    gSystem->Load("$RHO/lib/RhoNNO.dll");
    gSystem->Load("$RHO/lib/KangaSchema.dll");
    gSystem->Load("$RHO/lib/PAFSchema.dll");
    gSystem->Load("$RHO/lib/RhoManager.dll");
    gSystem->Load("$RHO/lib/RhoSelector.dll");
    gSystem->Load("$RHO/lib/RhoHistogram.dll");
    gSystem->Load("$RHO/lib/RhoTools.dll");
#ifdef DISPLAY
    gSystem->Load("$RHO/lib/PAFDisplay.dll");
#endif
#ifdef BETAPID
    gSystem->Load("$RHO/lib/PAFBetaPid.dll");
#endif

// This is the UNIX section
#else
    gSystem->Load("libEG.so");              // To use PDG database
    gSystem->Load("libPhysics.so");         // To use TVector
    gSystem->Load("libProof.so");
    gSystem->Load("libTreePlayer.so");      // To use TTreeFormula
    gSystem->Load("libRootAuth.so");

    gSystem->Load("$RHO/lib/libRhoNNO.so");
    gSystem->Load("$RHO/lib/libRhoMath.so");
    gSystem->Load("$RHO/lib/libRhoConditions.so");
    gSystem->Load("$RHO/lib/libRhoBase.so");
    gSystem->Load("$RHO/lib/libRhoTools.so");
    gSystem->Load("$RHO/lib/libRhoHistogram.so");
    gSystem->Load("$RHO/lib/libPAFSchema.so");
    gSystem->Load("$RHO/lib/libKangaSchema.so");
   // gSystem->Load("$RHO/lib/libPAFSchema.so");
    gSystem->Load("$RHO/lib/libRhoManager.so");
    gSystem->Load("$RHO/lib/libRhoSelector.so");
    //gSystem->Load("$RHO/lib/libPAFFitter.so");
    //gSystem->Load("$RHO/lib/libPAFAdapters.so");
    //gSystem->Load("$RHO/lib/libPAFFramework.so");

#ifdef DISPLAY
    gSystem->Load("$RHO/lib/libPAFDisplay.so");
#endif

#ifdef BETAPID
    gSystem->Load("$RHO/lib/libPAFBetaPid.so");
#endif

#endif
 
}
