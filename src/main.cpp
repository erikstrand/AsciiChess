//==============================================================================
// main.cpp
// created November 18, 2012
//==============================================================================

#include <iostream>
#include "Chess.h"
#include "AsciiBoard.h"
#include "MoveParser.h"

using namespace std;


//------------------------------------------------------------------------------
int main (int argc, char** argv) {

   Board b;
   b.setupNewGame();
   //Board b2;


   b.clearHashState();
   b.b.set(5, PC::c0);
   b.b.set(6, PC::r0);
   b.b.set(7, PC::c0);

   b.b.set(49, PC::c0);
   b.b.set(25, PC::p1);

   b.testSpecialMoves();
   b.findCodes();
   //cout << AsciiBoard(b.bitboard());

   Board b2;
   Generator gen;
   gen.setBoard(b);

   gen.setSource(10);
   ++gen;
   b.realize(gen, b2);
   b = b2;
   cout << AsciiBoard(b.bitboard());
 
   gen.setBoard(b);
   gen.setSource(25);
   while (gen.valid()) {
      b.realize(gen, b2);
      cout << AsciiBoard(b2.bitboard());
      ++gen;
   }

   //Game game;
   //MoveParser parser;

   /*
   cout << AsciiBoard(b.bitboard());
   if (b.pd.lightMove()) {
      cout << "Light's move:\n";
   } else {
      cout << "Dark's move:\n";
   }
   Generator const& gen = parser.getMove(b);
   if (gen.valid()) {
      b.realize(gen, b2);
      cout << AsciiBoard(b2.bitboard());
   } else {
      cout << "Could not parse your move." << '\n';
   }
   */

}

