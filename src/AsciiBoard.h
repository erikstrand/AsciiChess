//==============================================================================
// AsciiBoard.h
// created November 20, 2012
//==============================================================================

#ifndef ASCII_BOARD
#define ASCII_BOARD

#include "Chess.h"
#include <sstream>


//------------------------------------------------------------------------------
struct AsciiBoard {
   std::string str;
   bool negativeSpaceIsDark;

   AsciiBoard (BitBoard const& b);

   void drawEmptyPawn   (int i);
   void drawEmptyKnight (int i);
   void drawEmptyBishop (int i);
   void drawEmptyRook   (int i);
   void drawEmptyQueen  (int i);
   void drawEmptyKing   (int i);
   void drawFilledPawn   (int i);
   void drawFilledKnight (int i);
   void drawFilledBishop (int i);
   void drawFilledRook   (int i);
   void drawFilledQueen  (int i);
   void drawFilledKing   (int i);

   static const int rowL = 93;
};

std::ostream& operator<< (std::ostream& os, AsciiBoard const& ab);

#endif
