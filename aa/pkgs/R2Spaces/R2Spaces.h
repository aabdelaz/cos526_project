/* Include file for R2 spatial subdivision module */

#ifndef __R2__SPACES__H__
#define __R2__SPACES__H__



/* Dependency include files */

#include "R2Shapes/R2Shapes.h"



/* Adt stuff */

#include "R2Spaces/RNAdt.h"
#include "R2Spaces/RNList.h"
#include "R2Spaces/RNTree.h"
#include "R2Spaces/RNWing.h"



/* Basic stuff */

#include "R2Spaces/R2Space.h"



/* Adt interface classes */

#include "R2Spaces/R2Spct.h"



/* BSP stuff */

#include "R2Spaces/R2Bspt.h"



/* Winged-edge stuff */

#include "R2Spaces/R2Wing.h"



/* Beam classes */

#include "R2Spaces/R2Beam.h"
#include "R2Spaces/R2Pbeam.h"
#include "R2Spaces/R2Sbeam.h"



/* Public functions */

int R2InitSpaces();
void R2StopSpaces();
 


#endif

