//==============================================================================
// GambitInterface.h
// created January 3, 2014
//==============================================================================

#ifndef GAMBITINTERFACE
#define GAMBITINTERFACE

#include "Chess.h"


//------------------------------------------------------------------------------
class GambitInterface {
private:
   Game game;
   Generator generator;

public:
   void loop ();
};


#endif

