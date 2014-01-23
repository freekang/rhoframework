// TXMLP network trained with NNO NetworkTrainer at Thu Jan 23 22:33:38 2014
// Input parameters  mom:acos(theta):svt:emc:drc:dch:ifr:ifrExp:ifrAdd
// Output parameters abs(pid)==1
// Training files:
//$RHO/Data/PidTuple.root

#include "RhoNNO/TXMLP.h"

Double_t* Recall(Double_t *invec)
{
	static TXMLP net("TXMLP.net");
	Float_t x[7];
	x[0] 	= 1.02292	*	invec[0];	// mom
	x[1] 	= 2.87313	*	invec[1];	// acos(theta)
	x[2] 	= 0.267284	*	invec[2];	// svt
	x[3] 	= 0.763076	*	invec[3];	// emc
	x[4] 	= 1.75257	*	invec[4];	// drc
	x[5] 	= 0.00179238	*	invec[5];	// dch
	x[6] 	= 0.331141	*	invec[6];	// ifr
	return net.Recall(x);
}
