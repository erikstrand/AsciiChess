//==============================================================================
// MoveParser.h
// created November 27, 2012
//==============================================================================

#include "Chess.h"
#include <string>


//==============================================================================
// Move Parser
//==============================================================================

struct MoveParser {
   Generator _gen;
   std::string command;
   std::string error;
   char piece;
   char file1;
   char rank1;
   bool capture;
   char file2;
   char rank2;
   bool foundPiece;
   bool foundFile1;
   bool foundRank1;
   bool foundFile2;
   bool foundRank2;

   Generator const& getMove (Board const& b);

   void setBoard (Board const& b) { _gen.setBoard(b); }
   // Returns 0 if it doesn't understand the command,
   // 1 if the command makes sense but must be verified,
   // and 2 if the command makes sense and need not be verified.
   int parse (char const* command);
   bool verify ();
};

