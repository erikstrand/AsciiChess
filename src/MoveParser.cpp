//==============================================================================
// MoveParser.cpp
// created November 27, 2012
//==============================================================================

#include "MoveParser.h"
#include <iostream>
#include <sstream>


//------------------------------------------------------------------------------
Generator const& MoveParser::getMove (Board const& b) {
   setBoard(b);
   std::cin >> command;
   int parse_result = parse(command.c_str());

   while (parse_result == 0) {
      std::ostringstream errorStream;
      errorStream << "I do not understand the move \"" << command << "\".";
      error = errorStream.str();
      _gen.finish();
      return _gen;
   }

   if (!verify()) {
      std::ostringstream errorStream;
      errorStream << "The move \"" << command << "\" is not legal.";
      error = errorStream.str();
      _gen.finish();
      return _gen;
   }

   return _gen;
}

//------------------------------------------------------------------------------
// ToDo: need way to express en passant!
int MoveParser::parse (char const* command) {
   if (strcmp(command, "0-0") == 0) {
      if (_gen.board().specialMoves().canCastleShort()) {
         _gen.jumpToCastleShort();
         return 2;
      }
      return 0;
   }
   if (strcmp(command, "0-0-0") == 0) {
      if (_gen.board().specialMoves().canCastleLong()) {
         _gen.jumpToCastleLong();
         return 2;
      }
      return 0;
   }

   capture = false;
   foundPiece = false;
   foundFile1 = false;
   foundRank1 = false;
   foundFile2 = false;
   foundRank2 = false;

   char const* pieceLetters = "PNBRQK";
   char const* fileLetters = "abcdefgh";
   
   unsigned commandLength = strlen(command);
   if (commandLength < 2)
      return 0;

   unsigned i=0;
   for (unsigned j=0; j<strlen(pieceLetters); ++j) {
      if (command[i] == pieceLetters[j]) {
         piece = command[i];
         foundPiece = true;
         ++i;
      }
   }

   for (unsigned j=0; j<strlen(fileLetters); ++j) {
      if (command[i] == fileLetters[j]) {
         file1 = command[i];
         foundFile1 = true;
         ++i;
      }
   }

   // if we found two letters and that's all we have
   if (i == commandLength)
      return 0;

   if ('1' <= command[i] && command[i] <= '9') {
      rank1 = command[i];
      foundRank1 = true;
      ++i;
   }

   if (i == commandLength) {
      foundRank1 = false;
      rank2 = rank1;
      if (!foundFile1) {
         return 0;
      } else {
         file2 = file1;
         foundFile1 = false;
         return 1;
      }
   }

   if (command[i] == 'x') {
      capture = true;
      ++i;
   }

   if (i == commandLength) {
      return 0;
   }

   for (unsigned j=0; j<strlen(fileLetters); ++j) {
      if (command[i] == fileLetters[j]) {
         file2 = command[i];
         foundFile2 = true;
         ++i;
      }
   }

   if (i == commandLength) {
      return 0;
   }

   if ('1' <= command[i] && command[i] <= '9') {
      rank2 = command[i];
      foundRank2 = true;
      ++i;
   }

   // if we made it this far, either there was an 'x', or more than one rank or file
   if (1 == commandLength) {
      if (foundFile2) {
         return 1;
      }
   }
   return false;
}

//------------------------------------------------------------------------------
bool MoveParser::verify () {
   if (!foundPiece) {
      piece = 'P';
   }

   Piece p;

   switch (piece) {
   case 'P':
      p = _gen.lightMove() ? PC::p0 : PC::p1; break;
   case 'N':
      p = _gen.lightMove() ? PC::n0 : PC::n1; break;
   case 'B':
      p = _gen.lightMove() ? PC::b0 : PC::b1; break;
   case 'R':
      p = _gen.lightMove() ? PC::r0 : PC::r1; break;
   case 'Q':
      p = _gen.lightMove() ? PC::q0 : PC::q1; break;
   case 'K':
      p = _gen.lightMove() ? PC::k0 : PC::k1; break;
   }

   unsigned file1u = file1 - 'a';
   unsigned rank1u = rank1 - '1';
   unsigned file2u = file2 - 'a';
   unsigned rank2u = rank2 - '1';

   PieceItr itr(_gen.bitboard(), _gen.lightMove() ? PieceItr::light_pieces : PieceItr::dark_pieces);
   for ( ; itr.valid(); ++itr) {
      if (itr.piece() == p) {
         if (foundFile1 && file(itr.square()) != file1u)
            continue;
         if (foundRank1 && rank(itr.square()) != rank1u)
            continue;

         _gen.setSource(itr.square());
         for ( ; _gen.valid(); ++_gen) {
            if (rank(_gen.dst()) == rank2u && file(_gen.dst()) == file2u) {
               if (capture != _gen.isCapture())
                  return false;
               return true;
            }
         }
      }
   }

   return false;
}

