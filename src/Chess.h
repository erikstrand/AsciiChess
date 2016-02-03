//==============================================================================
// Chess.h
// created November 18, 2012
//==============================================================================

#ifndef CHESS
#define CHESS

#include <vector>
#include "HashFunctions.h"


//==============================================================================
// Piece Codes
//==============================================================================

//------------------------------------------------------------------------------
// These codes are used to represent states of a chess game.
/*
 * Internally, each configuration of pieces on the board is represented
 * as an array of these codes (one for each Square). Each piece code
 * represents the presence of the associated piece (on the associated square),
 * while each data code represents an empty square, and possibly
 * additional information (such as whose turn it is, or who can castle, etc.).
 */
typedef unsigned Piece;
namespace PC {
   // Data Codes
   static const unsigned c0 = 0;     // code0: internal use only
   static const unsigned c1 = 1;     // code1: internal use only
   static const unsigned c2 = 2;     // code2: out of bounds
   static const unsigned c3 = 3;     // code3: empty square

   // Light Piece Codes
   static const unsigned p0 = 4;     // light pawn
   static const unsigned n0 = 5;     // light knight
   static const unsigned b0 = 6;     // light bishop
   static const unsigned r0 = 7;     // light rook
   static const unsigned q0 = 8;     // light queen
   static const unsigned k0 = 9;     // light king

   // Dark Piece Codes
   static const unsigned p1 = 10;    // dark pawn
   static const unsigned n1 = 11;    // dark knight
   static const unsigned b1 = 12;    // dark bishop
   static const unsigned r1 = 13;    // dark rook
   static const unsigned q1 = 14;    // dark queen
   static const unsigned k1 = 15;    // dark king

   // Out of Bounds Code
   //static const unsigned out_of_bounds = 16;
}

//------------------------------------------------------------------------------
inline bool isLightPiece (Piece p) {
   return (4 <= p && p < 10);
}
inline bool isDarkPiece (Piece p) {
   return (10 <= p && p < 16);
}
inline bool isPiece (Piece p) {
   return (4 <= p && p < 16);
}
inline bool isEmpty (Piece p) {
   return (/*0 <= p &&*/ p < 4);
}
inline bool isLightOrEmpty (Piece p) {
   return (/*0 <= p &&*/ p < 10);
}
inline bool isDarkOrEmpty (Piece p) {
   return (isEmpty(p) || isDarkPiece(p));
}
/*
inline bool isOutOfBounds (Piece p) {
   return (16 == p);
}
*/
inline bool isPawn (Piece p) {
   return p == PC::p0 || p == PC::p1;
}
inline bool isKnight (Piece p) {
   return p == PC::n0 || p == PC::n1;
}
inline bool isKing (Piece p) {
   return p == PC::k0 || p == PC::k1;
}
inline bool isLongRange (Piece p) {
   return (6 <= p && p < 9) || (12 <= p && p < 15);
}
 

//==============================================================================
// Square Codes
//==============================================================================

//------------------------------------------------------------------------------
// Squares on the chess board are numbered so a1=0, b1=1, ..., g8=62, h8=63.
typedef unsigned Square;

//------------------------------------------------------------------------------
inline unsigned rank (Square s) { return s >> 3; }
inline unsigned file (Square s) { return s & 0x7; }


//==============================================================================
// BitBoard
//==============================================================================

//------------------------------------------------------------------------------
// The most basic board class. It only deals with getting and setting pieces.
class BitBoard {
private:
   // assumes sizeof(unsigned) == 4
   unsigned word[8];

public:
   BitBoard () {}
   BitBoard (BitBoard const& b) { memcpy(word, b.word, sizeof(unsigned) * 8); }
   void set (Square i, Piece p);
   Piece get (Square i) const;
   void clear () { memset(word, (PC::c0 | (PC::c0 << 4)), 32); }
   unsigned hash () const { return murmurhash(word, 8, 0xdefceedu); }

   void move (Square src, Square dst) { set(dst, get(src)); set(src, PC::c0); }
   void lightCastleShort ();
   void lightCastleLong  ();
   void darkCastleShort  ();
   void darkCastleLong   ();
   void lightEnPassantLeft  (unsigned file);
   void lightEnPassantRight (unsigned file);
   void darkEnPassantLeft   (unsigned file);
   void darkEnPassantRight  (unsigned file);
};


//==============================================================================
// PieceItr
//==============================================================================

//------------------------------------------------------------------------------
// Iterates over all pieces on a board.
class PieceItr {
private:
   BitBoard const* b;
   Square n;
   unsigned c;

public:
   static const unsigned all_pieces   = 0;
   static const unsigned light_pieces = 1;
   static const unsigned dark_pieces  = 2;

public:
   PieceItr (BitBoard const& b_, unsigned code): b(&b_), n(-1), c(code) {
      operator++();
   }
   inline void operator++ ();
   Square square () const { return n; }
   Piece piece () const { return b->get(n); }
   bool valid () const { return n < 64; }

private:
   inline void advance_all   ();
   inline void advance_light ();
   inline void advance_dark  ();
};

//------------------------------------------------------------------------------
void PieceItr::operator++ () {
   switch (c) {
   case all_pieces:
      advance_all(); break;
   case light_pieces:
      advance_light(); break;
   case dark_pieces:
      advance_dark(); break;
   }
}

//------------------------------------------------------------------------------
void PieceItr::advance_all () {
   do { ++n; }
   while (n < 64 && !isPiece(b->get(n)));
}

//------------------------------------------------------------------------------
void PieceItr::advance_light () {
   do { ++n; }
   while (n < 64 && !isLightPiece(b->get(n)));
}

//------------------------------------------------------------------------------
void PieceItr::advance_dark () {
   do { ++n; }
   while (n < 64 && !isDarkPiece(b->get(n)));
}


//==============================================================================
// MovementGenerator
//==============================================================================

//------------------------------------------------------------------------------
// Given a square and a piece, generates all squares that the piece could move to
// through some legal move with some board configuration and some game state.
// Does not check anything dependent on other pieces
// (ex, captures being possible or not, pieces being in the way or not, etc.).
class MovementGenerator {
private:
   Square   _src;
   Square   _dst;
   Piece    _piece;
   unsigned _state;
   static const int _end_state = 56;

public:
   MovementGenerator () {}
   // replace with one setup function
   void setPiece  (Piece p) { _piece = p; }
   void setSource (Square src) { _src = src; initialize(); }
   bool valid () const { return _state < _end_state; }
   // advances the generator to the next possible motion
   void operator++ () { ++_state; dispatch(); }
   // Advances through multiple motions at once:
   // - pawns: skips from straight moves to diagnoal moves
   // - long range pieces: skips to the next direction of motion
   // - knights and kings: undefined behavior
   inline void nextMotion ();
   void finish () { _state = _end_state; }

   Square   src   () const { return _src; }
   Square   dst   () const { return _dst; }
   Piece    piece () const { return _piece; }
   unsigned state () const { return _state; }

   bool pawnMoveIsDiagonal () const {
      return _state == 2 || _state == 3;
   }
   bool pawnMoveIsAdvance2 () const {
      return _state == 1;
   }

private:
   void initialize () { _state = 0; dispatch(); }
   inline void dispatch ();
   void advance_light_pawn ();
   void advance_dark_pawn  ();
   void advance_knight     ();
   void advance_bishop     ();
   void advance_rook       ();
   void advance_queen      ();
   void advance_king       ();
};

//------------------------------------------------------------------------------
void MovementGenerator::dispatch () {
   switch (_piece) {
   case PC::p0: advance_light_pawn(); break;
   case PC::n0: advance_knight();     break;
   case PC::b0: advance_bishop();     break;
   case PC::r0: advance_rook();       break;
   case PC::q0: advance_queen();      break;
   case PC::k0: advance_king();       break;
   case PC::p1: advance_dark_pawn();  break;
   case PC::n1: advance_knight();     break;
   case PC::b1: advance_bishop();     break;
   case PC::r1: advance_rook();       break;
   case PC::q1: advance_queen();      break;
   case PC::k1: advance_king();       break;
   }
}

//------------------------------------------------------------------------------
void MovementGenerator::nextMotion () {
   if (isPawn(_piece))
      _state = 2;
   else {
      _state = ((_state / 7) + 1) * 7;
   }
   dispatch();
}


//==============================================================================
// PathIndependentGen
//==============================================================================

//------------------------------------------------------------------------------
// Generates all path independent moves of any given piece on any given BitBoard.
/*
 * Note: I consider moves that put the king in check to be legal path independent
 * moves (provided they conform to the correct motions of the pieces), as these
 * moves are only ruled illegal by considering future possibilities.
 */
class PathIndependentGen {
private:
   MovementGenerator gen;
   // only imclude as function argument? Do after writing AI.
   BitBoard const* b;
   // should get rid of these; factor into logic
   bool (*isFriend) (Piece p);
   bool (*isEnemy) (Piece p);
   bool jump;

public:
   PathIndependentGen () {}
   // replace with one setup function
   void setBoard (BitBoard const& b_) { b = &b_; }
   inline void setColor  (bool light);
   inline void setSource (Square src);

   bool valid () const { return gen.valid(); }
   void operator++ () { ++gen; dispatch(); }
   void finish () { gen.finish(); }

   BitBoard const* board () const { return b; }
   Square   src       () const { return gen.src(); }
   Square   dst       () const { return gen.dst(); }
   Piece    srcPiece  () const { return gen.piece(); }
   Piece    dstPiece  () const { return b->get(dst()); }
   bool     isCapture () const { return isEnemy(dstPiece()); }

   bool pawnMoveIsAdvance2 () const { return gen.pawnMoveIsAdvance2(); }

private:
   void initialize () { jump = false; }
   inline void dispatch ();
   void advancePawn ();
   void advanceLongRange ();
   void advanceNormal ();    // advances knights and kings
};

//------------------------------------------------------------------------------
void PathIndependentGen::setColor (bool light) {
   isFriend = light ? &isLightPiece : &isDarkPiece;
   isEnemy  = light ? &isDarkPiece  : &isLightPiece;
}

//------------------------------------------------------------------------------
void PathIndependentGen::setSource (Square src) {
   // Set the piece
   gen.setPiece(b->get(src));

   // Set the location
   gen.setSource(src);

   // Set the color
   setColor(isLightPiece(srcPiece()));

   // Initialize
   initialize();
   dispatch();
}

//------------------------------------------------------------------------------
void PathIndependentGen::dispatch () {
   Piece p = srcPiece();
   if (isPawn(p))      { advancePawn(); return; }
   if (isLongRange(p)) { advanceLongRange(); return; }
   advanceNormal(); return;
}


//==============================================================================
// Path Independent Arbiter
//==============================================================================

//------------------------------------------------------------------------------
// Answers all questions that can be answered using only a PathIndependentGen
// Should be a namespace with functions, not a class.
class PathIndependentArbiter {
private:
   // include as argument, or create as necessary?
   // make static? treat class as singleton?
   PathIndependentGen gen;

public:
   // light signifies whose king we're investigating (not whose turn it is)
   bool isInCheck (BitBoard const& b, bool light);
   // if light == true, we check if dark can attack the given square
   bool isThreatened (BitBoard const& b, Square s, bool light);

   // en passant tests
   bool verifyLightEnPassantLeft  (BitBoard const& b, unsigned f);
   bool verifyLightEnPassantRight (BitBoard const& b, unsigned f);
   bool verifyDarkEnPassantLeft   (BitBoard const& b, unsigned f);
   bool verifyDarkEnPassantRight  (BitBoard const& b, unsigned f);

   // castling tests
   bool verifyLightCastleShort (BitBoard const& b);
   bool verifyDarkCastleShort  (BitBoard const& b);
   bool verifyLightCastleLong  (BitBoard const& b);
   bool verifyDarkCastleLong   (BitBoard const& b);
};


//==============================================================================
// Path Dependence
//==============================================================================

//------------------------------------------------------------------------------
class PathDependence {
private:
   /*
    * Bits are:
    * 0x1:  light to move
    * 0x2:  some pawn just double advanced
    * 0x4:  white king has moved
    * 0x8:  white rook 0 has moved
    * 0x10: white rook 7 has moved
    * 0x20: dark king has moved
    * 0x40: dark rook 0 has moved
    * 0x80: dark rook 7 has moved
    */
   unsigned flags; 

   // If 0x2 is set, this specifies the file (pawn color is !flags[0x1]).
   // Otherwise it should be 0.
   unsigned _pawnFile;

public:
   PathDependence () {}
   void newGame () { flags = 0x1; _pawnFile = 0; }

   // getters
   bool lightMove     () const { return flags & 0x1; }
   bool pawnAdvanced2 () const { return flags & 0x2; }
   bool lightKing     () const { return flags & 0x4; }
   bool lightRook0    () const { return flags & 0x8; }
   bool lightRook7    () const { return flags & 0x10; }
   bool darkKing      () const { return flags & 0x20; }
   bool darkRook0     () const { return flags & 0x40; }
   bool darkRook7     () const { return flags & 0x80; }
   unsigned pawnFile  () const { return _pawnFile; }

   // setters
   void swapTurn        () { flags ^= 0x1; }
   void clearPawnState  () { flags &= ~0x2; _pawnFile = 0; }
   void pawnAdvancedOnFile (unsigned pawnFile) { flags |= 0x2; _pawnFile = pawnFile; }
   void lightKingMoved  () { flags |= 0x4; }
   void lightRook0Moved () { flags |= 0x8; }
   void lightRook7Moved () { flags |= 0x10; }
   void darkKingMoved   () { flags |= 0x20; }
   void darkRook0Moved  () { flags |= 0x40; }
   void darkRook7Moved  () { flags |= 0x80; }
};


//==============================================================================
// Special Moves
//==============================================================================

//------------------------------------------------------------------------------
// Stores flags indicating which special moves are available
struct SpecialMoves {
private:
   /*
    * Bits are:
    * 0x1:  can capture en passant with pawn in pawnFile-1 file
    * 0x2:  can capture en passant with pawn in pawnFile+1 file
    * 0x4:  can castle short
    * 0x8:  can castle long
    */
   unsigned flags;

public:
   SpecialMoves (): flags(0) {}
   void clear () { flags = 0; }
   bool canEnPassantLeft  () const { return flags & 0x1; }
   bool canEnPassantRight () const { return flags & 0x2; }
   bool canCastleShort    () const { return flags & 0x4; }
   bool canCastleLong     () const { return flags & 0x8; }

   void setEnPassantLeft  () { flags |= 0x1; }
   void setEnPassantRight () { flags |= 0x2; }
   void setCastleShort    () { flags |= 0x4; }
   void setCastleLong     () { flags |= 0x8; }
};


//==============================================================================
// Board
//==============================================================================

//------------------------------------------------------------------------------
// forward declaration of Generator
class Generator;

//------------------------------------------------------------------------------
class Board {
public:
   BitBoard b;
   Square code[4];
   PathDependence pd;
   SpecialMoves sm; 

public:
   Board () {}
   void operator= (Board const& b) { memcpy(this, &b, sizeof(Board)); }
   void testSpecialMoves ();
   void setupNewGame ();
   void realize (Generator const& generator, Board& child) const;

   BitBoard const& bitboard () const { return b; }
   PathDependence const& pathDependence () const { return pd; }
   SpecialMoves const& specialMoves () const { return sm; }

public:
   void updatePDBasicMove (Generator const& gen);
   inline void clearHashState ();
   void writeHashState ();
   void writeEnPassantFile ();
   inline void findCodes ();
};

//------------------------------------------------------------------------------
void Board::clearHashState () {
   b.set(code[0], PC::c0);
   b.set(code[1], PC::c0);
   b.set(code[2], PC::c0);
   b.set(code[3], PC::c0);
}

//------------------------------------------------------------------------------
void Board::findCodes () {
   Square i = 0;
   for (int j=0; j<4; ++j) {
      while (isPiece(b.get(i))) ++i;
      code[j] = i++;
   }
}


//==============================================================================
// Generator
//==============================================================================

//------------------------------------------------------------------------------
// Generates all legal moves of any given piece on any given BitBoard.
class Generator {
private:
   Board const* _board;
   PathIndependentGen _gen;
   PathIndependentArbiter _arbiter;
   // 0: en passant capture left, 1: en passant capture right, 2: O-O, 3: O-O-O, 4: done
   // will later add states for forced draws (threefold repetition, 50 move rule)
   int _state; 

public:
   Generator () {}
   // should have one setup function
   void setBoard (Board const& b) { _board = &b; _gen.setBoard(b.bitboard()); }
   inline void setSource (Square src);

   inline void operator++ ();
   bool valid () const { return _gen.valid() || _state < 4; }
   void finish () { _gen.finish(); _state = 4; }

   Board const& board () const { return *_board; }
   BitBoard const& bitboard () const { return _board->bitboard(); }
   bool lightMove () const { return _board->pathDependence().lightMove(); }

   bool moveIsBasic () const { return _gen.valid(); }
   Square src      () const { return _gen.src(); }
   Square dst      () const { return _gen.dst(); }
   Piece  srcPiece () const { return _gen.srcPiece(); }
   Piece  dstPiece () const { return _gen.dstPiece(); }
   bool pawnMoveIsAdvance2 () const { return _gen.pawnMoveIsAdvance2(); }
   bool isCapture  () const { return _gen.isCapture(); }

   bool specialMoveIsEPLeft      () const { return _state == 0; }
   bool specialMoveIsEPRight     () const { return _state == 1; }
   unsigned enPassantFile        () const { return _board->pathDependence().pawnFile(); }
   bool specialMoveIsCastleShort () const { return _state == 2; }
   bool specialMoveIsCastleLong  () const { return _state == 3; }

   void jumpToEPLeft      () { _gen.finish(); _state = 0; }
   void jumpToEPRight     () { _gen.finish(); _state = 1; }
   void jumpToCastleShort () { _gen.finish(); _state = 2; }
   void jumpToCastleLong  () { _gen.finish(); _state = 3; }

private:
   void checkSpecialMoves ();
};

//------------------------------------------------------------------------------
void Generator::setSource (Square src) {
   _gen.setSource(src);
   _state = 0;
   if (!_gen.valid())
      checkSpecialMoves();
}

//------------------------------------------------------------------------------
void Generator::operator++ () {
   if (_gen.valid()) {
      ++_gen;
      if (_gen.valid())
         return;
   } else {
      ++_state;
   }
   checkSpecialMoves();
}


//==============================================================================
// Arbiter
//==============================================================================

class Arbiter {
   PathIndependentArbiter pia;
   bool putsOrLeavesInCheck (Board const& b, Generator const& gen);
};


//==============================================================================
// Chess Game
//==============================================================================

class Game {
private:
   std::vector<Board> boards;

public:
   Game () {
      boards.push_back(Board());
      boards[0].setupNewGame();
   }
   Board const& currentBoard () const { return boards.back(); }
   void move (Generator const& gen) {
      Board const& current = boards.back();
      boards.push_back(Board());
      current.realize(gen, boards.back());
   }
};

#endif
