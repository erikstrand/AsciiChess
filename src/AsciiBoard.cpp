//==============================================================================
// AsciiBoard.cpp
// created November 20, 2012
//==============================================================================

#include "AsciiBoard.h"


//------------------------------------------------------------------------------
AsciiBoard::AsciiBoard (BitBoard const& b) {
   negativeSpaceIsDark = true;

   // Draw board itself
   std::ostringstream out;
   bool light = true;
   Square s = 56;
   out << "|----------+----------+----------+----------+----------+----------+----------+----------|   \n";
   for (int r = 7; r >= 0; --r) {
      s = 8 * r;
      // each rank needs many lines
      for (int i=0; i<4; ++i) {
         s = (r << 3) + i;
         for (int file = 0; file < 8; ++file) {
            out << '|';
            if (light == negativeSpaceIsDark) {
               out << "..........";
            } else {
               out << "          ";
            }
            ++s;
            light = !light;
         }
         out << '|';

         if (i == 2) {
            out << "  " << r + 1;
         } else {
            out << "   ";
         }

         out << '\n';
         ++s;
      }

      // draw bottom ruler
      out << "|----------+----------+----------+----------+----------+----------+----------+----------|   \n";

      // next row starts with other color
      light = !light;
   }
   out << "     A          B          C          D          E          F          G          H\n";

   // Superimpose Pieces
   str = out.str();
   for (PieceItr itr(b, PieceItr::all_pieces); itr.valid(); ++itr) {
      int x = file(itr.square()) * 11 + 1;
      int y = (7 - rank(itr.square())) * 5 + 1;
      int i = y*rowL + x;
      switch (itr.piece()) {
      case PC::p0:
         negativeSpaceIsDark ? drawFilledPawn(i) : drawEmptyPawn(i); break;
      case PC::n0:
         negativeSpaceIsDark ? drawFilledKnight(i) : drawEmptyKnight(i); break;
      case PC::b0:
         negativeSpaceIsDark ? drawFilledBishop(i) : drawEmptyBishop(i); break;
      case PC::r0:
         negativeSpaceIsDark ? drawFilledRook(i) : drawEmptyRook(i); break;
      case PC::q0:
         negativeSpaceIsDark ? drawFilledQueen(i) : drawEmptyQueen(i); break;
      case PC::k0:
         negativeSpaceIsDark ? drawFilledKing(i) : drawEmptyKing(i); break;
      case PC::p1:
         negativeSpaceIsDark ? drawEmptyPawn(i) : drawFilledPawn(i); break;
      case PC::n1:
         negativeSpaceIsDark ? drawEmptyKnight(i) : drawFilledKnight(i); break;
      case PC::b1:
         negativeSpaceIsDark ? drawEmptyBishop(i) : drawFilledBishop(i); break;
      case PC::r1:
         negativeSpaceIsDark ? drawEmptyRook(i) : drawFilledRook(i); break;
      case PC::q1:
         negativeSpaceIsDark ? drawEmptyQueen(i) : drawFilledQueen(i); break;
      case PC::k1:
         negativeSpaceIsDark ? drawEmptyKing(i) : drawFilledKing(i); break;
      }
   }
}

//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, AsciiBoard const& ab) {
   // Output to cout
   return os << ab.str;
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyPawn (int i) {
   str[i] = 'P';
   str[i+4] = '_';
   str[i+5] = '_';

   i += rowL;
   str[i+3] = '(';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = ')';

   i += rowL;
   str[i+4] = '|';
   str[i+5] = '|';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyKnight (int i) {
   str[i] = 'N';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '-';

   i += rowL;
   str[i+2] = '(';
   str[i+3] = '_';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = '}';

   i += rowL;
   str[i+4] = ')';
   str[i+5] = ' ';
   str[i+6] = '}';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '|';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyBishop (int i) {
   str[i] = 'B';
   str[i+4] = ',';
   str[i+5] = '_';

   i += rowL;
   str[i+2] = '(';
   str[i+3] = '\\';
   str[i+4] = '|';
   str[i+5] = ' ';
   str[i+6] = ')';

   i += rowL;
   str[i+3] = '|';
   str[i+4] = ' ';
   str[i+5] = '|';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyRook (int i) {
   str[i] = 'R';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '_';

   i += rowL;
   str[i+2] = '[';
   str[i+3] = ' ';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = ' ';
   str[i+7] = ']';

   i += rowL;
   str[i+3] = '|';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = '|';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '_';
   str[i+7] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyQueen (int i) {
   str[i] = 'Q';
   str[i+2] = '\\';
   str[i+3] = '^';
   str[i+4] = '^';
   str[i+5] = '^';
   str[i+6] = '^';
   str[i+7] = '/';

   i += rowL;
   str[i+2] = '<';
   str[i+3] = ' ';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = ' ';
   str[i+7] = '>';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = '\\';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '_';
   str[i+7] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawEmptyKing (int i) {
   str[i] = 'K';
   str[i+3] = '_';
   str[i+4] = '+';
   str[i+5] = '+';
   str[i+6] = '_';

   i += rowL;
   str[i+2] = '\\';
   str[i+3] = ' ';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = ' ';
   str[i+7] = '/';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = ' ';
   str[i+5] = ' ';
   str[i+6] = '\\';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '_';
   str[i+7] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledPawn (int i) {
   str[i] = 'P';
   str[i+4] = '_';
   str[i+5] = '_';

   i += rowL;
   str[i+3] = '(';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = ')';

   i += rowL;
   str[i+4] = '$';
   str[i+5] = '$';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledKnight (int i) {
   str[i] = 'N';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '-';

   i += rowL;
   str[i+2] = '(';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '}';

   i += rowL;
   str[i+4] = ')';
   str[i+5] = '*';
   str[i+6] = '}';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '|';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledBishop (int i) {
   str[i] = 'B';
   str[i+4] = ',';
   str[i+5] = '_';

   i += rowL;
   str[i+2] = '(';
   str[i+3] = '\\';
   str[i+4] = '|';
   str[i+5] = '*';
   str[i+6] = ')';

   i += rowL;
   str[i+3] = '|';
   str[i+4] = '*';
   str[i+5] = '|';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledRook (int i) {
   str[i] = 'R';
   str[i+3] = '_';
   str[i+4] = '_';
   str[i+5] = '_';
   str[i+6] = '_';

   i += rowL;
   str[i+2] = '[';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = ']';

   i += rowL;
   str[i+3] = '|';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '|';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledQueen (int i) {
   str[i] = 'Q';
   str[i+2] = '\\';
   str[i+3] = '^';
   str[i+4] = '^';
   str[i+5] = '^';
   str[i+6] = '^';
   str[i+7] = '/';

   i += rowL;
   str[i+2] = '<';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = '>';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '\\';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = '\\';
}

//------------------------------------------------------------------------------
void AsciiBoard::drawFilledKing (int i) {
   str[i] = 'K';
   str[i+3] = '_';
   str[i+4] = '+';
   str[i+5] = '+';
   str[i+6] = '_';

   i += rowL;
   str[i+2] = '\\';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = '/';

   i += rowL;
   str[i+3] = '/';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '\\';

   i += rowL;
   str[i+2] = '/';
   str[i+3] = '*';
   str[i+4] = '*';
   str[i+5] = '*';
   str[i+6] = '*';
   str[i+7] = '\\';
}

