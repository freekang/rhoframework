//--------------------------------------------------------------------------
// File and Version Information:
//      $Id $
//
// Description:
//      Class ComPackNonFlatFloat 
//      Class to pack doubles into 8 or 16 bits packing mantissa and fraction 
//      separately. This is an implementation of CommonUtils/ComPackFlatFloat
//      and it has the precision hardwired in order to make it more 
//      "resistent to changes"
//      This class is meant to be used to pack and unpack microDST. Changing 
//      its internal parameter most likely causes previously written MicroDST
//      to be misinterpreted.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      R.Faccini                   Originator. 
//
// Copyright Information:
//      Copyright (C) 1999          U.C.S.D.
//
//   
// Quote:
//      "It ain't the way that you do it, it's what you do"
//
//--------------------------------------------------------------------------

//
// C includes
//
#include <stddef.h> 
#include <stdlib.h>
#include <math.h>
#include <time.h>

//
// Bass class includes
//
#include "KangaSchema/ComPackFloat.h"
#include "KangaSchema/ComPackUFloat.h"
#include "TString.h"

//
// This class
//
#include "KangaSchema/ComPackNonFlatFloat.h"

#include <iostream>
using namespace std;

ComPackNonFlatFloat::~ComPackNonFlatFloat()
{
  delete _engine;
}

ComPackNonFlatFloat::ComPackNonFlatFloat(signType sigT)
{
  if(sigT==SIG){
    _engine=new ComPackFloat(FRACLEN_16-1,EXPLEN_16,BIAS_16);
  }else if(sigT==UNSIG){   
    _engine=new ComPackUFloat(FRACLEN_16,EXPLEN_16,BIAS_16);
  }else if(sigT==SIG_8){
    _engine=new ComPackFloat(FRACLEN_8-1,EXPLEN_8,BIAS_8);
  }else if(sigT==UNSIG_8){   
    _engine=new ComPackUFloat(FRACLEN_8,EXPLEN_8,BIAS_8);
  }
}

ComPackBase<double>::StatusCode
ComPackNonFlatFloat::unpack( double xmin,double xmax,d_UShort val, double & unpackedval ) const
{
  if( xmin>=xmax) return ComPackBase<double>::TAG_RANGE_ERROR ;
  if(val==0){
     unpackedval =0.;
     return ComPackBase<double>::TAG_OK;
  }
  double  x;
  unsigned long y(val);
  ComPackBase<double>::StatusCode status=_engine->unpack(y,x);
  if(status == ComPackBase<double>::TAG_OK) {
    if(xmin<0){
      unpackedval = (x>0) ? x*xmax : x*fabs(xmin);  
    }else{
      unpackedval = xmin + x*(xmax-xmin);
    }
  }
  return status;
}

  
ComPackBase<double>::StatusCode
ComPackNonFlatFloat::pack( double xmin, double xmax,double val, d_UShort & packedval ) const
{
  if( xmin>=xmax) return ComPackBase<double>::TAG_RANGE_ERROR ;
  if(fabs(val)<1e-30){
    // map 0 into 0
    packedval=0;
    return ComPackBase<double>::TAG_OK;
  }
  ComPackBase<double>::StatusCode status1(ComPackBase<double>::TAG_OK);
  if(xmin<0  &&  xmax>0){
    val = (val>0) ? val/xmax : val/fabs(xmin);  
  }else{
    val = (val-xmin)/(xmax-xmin);
  }
  if(fabs(val)>_engine->getMaxVal()) {
    status1 =  ComPackBase<double>::TAG_VAL_ROUND_UP;
    val = val>0?_engine->getMaxVal():-_engine->getMaxVal();
  }else if(fabs(val)<_engine->getMinVal()){
    status1 =  ComPackBase<double>::TAG_VAL_ROUND_DOWN;
    val = val>0 ? _engine->getMinVal():-_engine->getMinVal();
  }
  d_ULong x;
  ComPackBase<double>::StatusCode status=_engine->pack(val,x);
  packedval = x;
  return (status == ComPackBase<double>::TAG_OK) ? status1 : status;
}

ComPackBase<double>::StatusCode
ComPackNonFlatFloat::pack( double xmin, double xmax,double val, d_Octet & packedval ) const
{
  d_UShort x;
  ComPackBase<double>::StatusCode stat=pack(xmin,xmax,val,x);
  packedval = x & 0xff;
  return stat;
}

Bool_t
ComPackNonFlatFloat::packAndScream(double xmin, double xmax ,  double x, d_UShort&y, const char* name)
{
  if(x<xmin){
    cerr << "  Packing out of range " << name <<" " << x << endl;
    x = xmin;
  }else if(x>xmax){
    cerr << "  Packing out of range " << name <<" " << x << endl;
    x = xmax;
  }

  ComPackBase<double>::StatusCode status= pack (xmin,xmax,x,y);

  if(x!=0 &&status != ComPackBase<double>::TAG_OK){
    cerr << " Problems in Packing " << name <<" " << x << endl;
    cerr << status << endl;
    return false ;
  }

  return true ;
}

Bool_t
ComPackNonFlatFloat::packAndScream(double xmin, double xmax ,  double x, d_Octet&y, const char* name)
{
  if(x<xmin){
    cerr << "  Packing out of range " << name <<" " << x << endl;
    x = xmin;
  }else if(x>xmax){
    cerr << "  Packing out of range " << name <<" " << x << endl;
    x = xmax;
  }

  ComPackBase<double>::StatusCode status= pack (xmin,xmax,x,y);
  if(x!=0 &&status != ComPackBase<double>::TAG_OK){
    cerr << " Problems in Packing " << name <<" " << x << endl;
    cerr << status << endl;
    return false ;
  }

  return true ;
}

Bool_t
ComPackNonFlatFloat::unpackAndScream(double xmin, double xmax, const  d_UShort&x, double &y, const char* name) const 
{
  ComPackBase<double>::StatusCode status= unpack (xmin,xmax,x,y);
  if( status != ComPackBase<double>::TAG_OK) {
    cerr << "problems in unPacking " << name <<" " <<status << endl;
    return false ;
  }
  return true ;
}








