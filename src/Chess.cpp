//==============================================================================
// Chess.cpp
// created November 17, 2012
//==============================================================================

#include "Chess.h"


//==============================================================================
// BitBoard
//==============================================================================

//------------------------------------------------------------------------------
Piece BitBoard::get (Square s) const {
   unsigned index = s     <<  2;
   unsigned block = index >>  5;
   unsigned pos   = index &  31;
   return ( word[block] >> pos ) & 0xfu;
}

//------------------------------------------------------------------------------
void BitBoard::set (Square s, Piece p) {
   unsigned index = s     <<  2;
   unsigned block = index >>  5;
   unsigned pos   = index &  31;
   word[block] &= ~(0xfu << pos);
   word[block] |= p << pos;
}

//------------------------------------------------------------------------------
void BitBoard::lightCastleShort () {
   set(4, PC::c0);
   set(5, PC::r0);
   set(6, PC::k0);
   set(7, PC::c0);
}

//------------------------------------------------------------------------------
void BitBoard::lightCastleLong () {
   set(0, PC::c0);
   set(2, PC::k0);
   set(3, PC::r0);
   set(4, PC::c0);
}

//------------------------------------------------------------------------------
void BitBoard::darkCastleShort () {
   set(60, PC::c0);
   set(61, PC::r0);
   set(62, PC::k0);
   set(63, PC::c0);
}

//------------------------------------------------------------------------------
void BitBoard::darkCastleLong () {
   set(56, PC::c0);
   set(58, PC::k0);
   set(59, PC::r0);
   set(60, PC::c0);
}

//------------------------------------------------------------------------------
void BitBoard::lightEnPassantLeft (unsigned file) {
   set(31 + file, PC::c0);
   set(32 + file, PC::c0);
   set(40 + file, PC::p0);
}

//------------------------------------------------------------------------------
void BitBoard::lightEnPassantRight (unsigned file) {
   set(32 + file, PC::c0);
   set(33 + file, PC::c0);
   set(40 + file, PC::p0);
}

//------------------------------------------------------------------------------
void BitBoard::darkEnPassantLeft (unsigned file) {
   set(23 + file, PC::c0);
   set(24 + file, PC::c0);
   set(16 + file, PC::p1);
}

//------------------------------------------------------------------------------
void BitBoard::darkEnPassantRight (unsigned file) {
   set(24 + file, PC::c0);
   set(25 + file, PC::c0);
   set(16 + file, PC::p1);
}


//==============================================================================
// MovementGenerator Methods
//==============================================================================

//------------------------------------------------------------------------------
// 0: advance 1, 1: advance 2, 2: capture left, 3: capture right
void MovementGenerator::advance_light_pawn () {
   switch (_state) {
   case 0:
      // technically this test is unnecessary as pawn promotion is mandatory
      // (so a light pawn can never be found on rank 7)
      if (rank(_src) < 7) {
         _dst = _src + 8;
         return;
      }
      // if the pawn is on rank 7 it cannot move anywhere
      _state = _end_state;
      return;
   case 1:
      if (rank(_src) == 1) {
         _dst = _src + 16;
         return;
      }
      ++_state;
   case 2:
      if (file(_src) > 0) {
         _dst = _src + 7;
         return;
      }
      ++_state;
   case 3:
      if (file(_src) < 7) {
         _dst = _src + 9;
         return;
      }
   default:
      _state = _end_state;
   }
}

//------------------------------------------------------------------------------
// 0: advance 1, 1: advance 2, 2: capture left, 3: capture right
void MovementGenerator::advance_dark_pawn () {
   switch (_state) {
   case 0:
      // technically this test is unnecessary as pawn promotion is mandatory
      // (so a dark pawn can never be found on rank 0)
      if (rank(_src) > 0) {
         _dst = _src - 8;
         return;
      }
      // if the pawn is on rank 7 it cannot move anywhere
      _state = _end_state;
      return;
   case 1:
      if (rank(_src) == 6) {
         _dst = _src - 16;
         return;
      }
      ++_state;
   case 2:
      if (file(_src) > 0) {
         _dst = _src - 9;
         return;
      }
      ++_state;
   case 3:
      if (file(_src) < 7) {
         _dst = _src - 7;
         return;
      }
   default:
      _state = _end_state;
   }
}

//------------------------------------------------------------------------------
void MovementGenerator::advance_knight () {
   switch (_state) {
   // up 1, right 2
   case 0:
      if (file(_src) < 6 && rank(_src) < 7) {
         _dst = _src + 10;
         return;
      }
      ++_state;
   // up 2, right 1
   case 1:
      if (file(_src) < 7 && rank(_src) < 6) {
         _dst = _src + 17;
         return;
      }
      ++_state;
   // up 2, left 1
   case 2:
      if (file(_src) > 0 && rank(_src) < 6) {
         _dst = _src + 15;
         return;
      }
      ++_state;
   // up 1, left 2
   case 3:
      if (file(_src) > 1 && rank(_src) < 7) {
         _dst = _src + 6;
         return;
      }
      ++_state;
   // down 1, left 2
   case 4:
      if (file(_src) > 1 && rank(_src) > 0) {
         _dst = _src - 10;
         return;
      }
      ++_state;
   // down 2, left 1
   case 5:
      if (file(_src) > 0 && rank(_src) > 1) {
         _dst = _src - 17;
         return;
      }
      ++_state;
   // down 2, right 1
   case 6:
      if (file(_src) < 7 && rank(_src) > 1) {
         _dst = _src - 15;
         return;
      }
      ++_state;
   // down 1, right 2
   case 7:
      if (file(_src) < 6 && rank(_src) > 0) {
         _dst = _src - 6;
         return;
      }
   default:
      _state = _end_state;
   }
}

//------------------------------------------------------------------------------
// 0-6: up right, 7-13: up left, 14-20: down left, 21-27: down right
void MovementGenerator::advance_bishop () {
   if (_state < 28) {
      unsigned i;
      while (true) {
         // going up
         if (_state < 14) {
            // up and right
            if (_state < 7) {
               i = _state + 1;
               if (file(_src) < 8-i && rank(_src) < 8-i) {
                  _dst = _src + 9*i;
                  return;
               }
               _state = 7;
               continue;
            // up and left
            } else {
               i = _state - 6;
               if (file(_src) >= i && rank(_src) < 8-i) {
                  _dst = _src + 7*i;
                  return;
               }
               _state = 14;
               continue;
            }
         // going down
         } else {
            // down and left
            if (_state < 21) {
               i = _state - 13;
               if (file(_src) >= i && rank(_src) >= i) {
                  _dst = _src - 9*i;
                  return;
               }
               _state = 21;
               continue;
            // down and right
            } else {
               i = _state - 20;
               if (file(_src) < 8-i && rank(_src) >= i) {
                  _dst = _src - 7*i;
                  return;
               }
               _state = _end_state;
               return;
            }
         }
      }
   }
   _state = _end_state;
}

//------------------------------------------------------------------------------
// 0-6: right, 7-13: up, 14-20: left, 21-27: down
void MovementGenerator::advance_rook () {
   if (_state < 28) {
      unsigned i;
      while (true) {
         // going right or up
         if (_state < 14) {
            // right
            if (_state < 7) {
               i = _state + 1;
               if (file(_src) < 8-i) {
                  _dst = _src + i;
                  return;
               }
               _state = 7;
               continue;
            // up
            } else {
               i = _state - 6;
               if (rank(_src) < 8-i) {
                  _dst = _src + 8*i;
                  return;
               }
               _state = 14;
               continue;
            }
         // going left or down
         } else {
            // left
            if (_state < 21) {
               i = _state - 13;
               if (file(_src) >= i) {
                  _dst = _src - i;
                  return;
               }
               _state = 21;
               continue;
            // down
            } else {
               i = _state - 20;
               if (rank(_src) >= i) {
                  _dst = _src - 8*i;
                  return;
               }
               _state = _end_state;
               return;
            }
         }
      }
   }
   _state = _end_state;
}

//------------------------------------------------------------------------------
void MovementGenerator::advance_queen () {
   if (_state < 28) {
      // look for a bishop move
      advance_bishop();
      if (valid()) {
         return;
      }
      _state = 28;
   }

   // look for a rook move
   int state_copy = _state;
   _state -= 28;
   advance_rook();
   if (valid()) {
      _state += 28;
   }
}

//------------------------------------------------------------------------------
void MovementGenerator::advance_king () {
   switch (_state) {
   case 0:
      // right
      if (file(_src) < 7) {
         _dst = _src + 1;
         return;
      }
      _state = 2;
      goto up; // if it can't go right, it can't go right and up
   case 1:
      // right and up
      // this state is only reached if it can go right, so we only test that it can go up
      if (rank(_src) < 7) {
         _dst = _src + 9;
         return;
      }
      _state = 4;
      goto left;
   case 2:
      up:
      if (rank(_src) < 7) {
         _dst = _src + 8;
         return;
      }
      _state = 4;
      goto left;
   case 3:
      // left and up
      if (file(_src) > 0) {
         _dst = _src + 7;
         return;
      }
      _state = 6;
      goto down;
   case 4:
      left:
      if (file(_src) > 0) {
         _dst = _src - 1;
         return;
      }
      _state = 6;
      goto down;
   case 5:
      // left and down
      if (rank(_src) > 0) {
         _dst = _src - 9;
         return;
      }
      _state = _end_state;
      return;
   case 6:
      down:
      if (rank(_src) > 0) {
         _dst = _src - 8;
         return;
      }
      _state = _end_state;
      return;
   case 7:
      // right and down
      if (file(_src) < 7) {
         _dst = _src - 7;
         return;
      }
   default:
      _state = _end_state;
   }
}


//==============================================================================
// PathIndependentGen Methods
//==============================================================================

//------------------------------------------------------------------------------
void PathIndependentGen::advancePawn () {
   while (gen.valid()) {
      if (!gen.pawnMoveIsDiagonal()) {
         if (isEmpty(dstPiece()))
            return;
         gen.nextMotion();
         continue;
      } else {
         if (isEnemy(dstPiece()))
            return;
         ++gen;
         continue;
      }
   }
}

//------------------------------------------------------------------------------
void PathIndependentGen::advanceLongRange () {
   while (true) {
      if (jump) {
         gen.nextMotion();
         jump = false;
      }

      if (!gen.valid())
         return;

      if (isEmpty(dstPiece()))
         return;

      if (isEnemy(dstPiece())) {
         jump = true;
         return;
      }

      // if we've made it here, dstPiece() is a friend
      jump = true;
   }
}

//------------------------------------------------------------------------------
void PathIndependentGen::advanceNormal () {
   while (gen.valid()) {
      if (isFriend(dstPiece()))
         ++gen;
      else
         return;
   }
}


//==============================================================================
// Path Independent Arbiter
//==============================================================================

//------------------------------------------------------------------------------
bool PathIndependentArbiter::isInCheck (BitBoard const& b, bool light) {
   gen.setBoard(b);
   gen.setColor(light);

   Piece king = light ? PC::k0 : PC::k1;
   PieceItr itr(b, light ? PieceItr::dark_pieces : PieceItr::light_pieces);
   while (itr.valid()) {
      gen.setSource(itr.square());
      while (gen.valid()) {
         if (gen.dstPiece() == king) {
            return true;
         }
         ++gen;
      }
      ++itr;
   }
   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::isThreatened (BitBoard const& b, Square s, bool light) {
   gen.setBoard(b);
   gen.setColor(light);

   PieceItr itr(b, light ? PieceItr::dark_pieces : PieceItr::light_pieces);
   while (itr.valid()) {
      gen.setSource(itr.square());
      while (gen.valid()) {
         if (gen.dst() == s) {
            return true;
         }
         ++gen;
      }
      ++itr;
   }

   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyLightEnPassantLeft (BitBoard const& b, unsigned f) {
   BitBoard b2(b);
   Square dst = 40 + f;
   Square src = 31 + f;
   if (f > 0 && b.get(src) == PC::p0) {
      b2.move(src, dst);
      if (!isInCheck(b2, true))
         return true;
   }
   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyLightEnPassantRight (BitBoard const& b, unsigned f) {
   BitBoard b2(b);
   Square dst = 40 + f;
   Square src = 33 + f;
   if (f < 7 && b.get(src) == PC::p0) {
      b2.move(src, dst);
      if (!isInCheck(b2, true))
         return true;
   }
   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyDarkEnPassantLeft (BitBoard const& b, unsigned f) {
   BitBoard b2(b);
   Square dst = 16 + f;
   Square src = 23 + f;
   if (f > 0 && b.get(src) == PC::p1) {
      b2.move(src, dst);
      if (!isInCheck(b2, false))
         return true;
   }
   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyDarkEnPassantRight (BitBoard const& b, unsigned f) {
   BitBoard b2(b);
   Square dst = 16 + f;
   Square src = 25 + f;
   if (f < 7 && b.get(src) == PC::p1) {
      b2.move(src, dst);
      if (!isInCheck(b2, false))
         return true;
   }
   return false;
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyLightCastleShort (BitBoard const& b) {
   return ( isEmpty(b.get(5)) && isEmpty(b.get(6)) && !isThreatened(b, 4, true)
        && !isThreatened(b, 5, true) && !isThreatened(b, 6, true) );
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyDarkCastleShort (BitBoard const& b) {
   return ( isEmpty(b.get(61)) && isEmpty(b.get(62)) && !isThreatened(b, 60, false)
        && !isThreatened(b, 61, false) && !isThreatened(b, 62, false) );
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyLightCastleLong (BitBoard const& b) {
   return ( isEmpty(b.get(1)) && isEmpty(b.get(2)) && isEmpty(b.get(3)) && !isThreatened(b, 4, true)
        && !isThreatened(b, 1, true) && !isThreatened(b, 2, true) && !isThreatened(b, 3, true) );
}

//------------------------------------------------------------------------------
bool PathIndependentArbiter::verifyDarkCastleLong (BitBoard const& b) {
   return ( isEmpty(b.get(57)) && isEmpty(b.get(58)) && isEmpty(b.get(59)) && !isThreatened(b, 60, false)
        && !isThreatened(b, 57, false) && !isThreatened(b, 58, false) && !isThreatened(b, 59, false) );
}


//==============================================================================
// PathDependence Methods
//==============================================================================


//==============================================================================
// Board Methods
//==============================================================================

//------------------------------------------------------------------------------
void Board::testSpecialMoves () {
   PathIndependentArbiter arbiter;

   if (pd.lightMove()) {
      if (pd.pawnAdvanced2()) {
         if (arbiter.verifyLightEnPassantLeft(b, pd.pawnFile()))
            sm.setEnPassantLeft();
         if (arbiter.verifyLightEnPassantRight(b, pd.pawnFile()))
            sm.setEnPassantRight();
      }

      if (!pd.lightKing()) {
         if (!pd.lightRook7() && arbiter.verifyLightCastleShort(b))
            sm.setCastleShort();
         if (!pd.lightRook0() && arbiter.verifyLightCastleLong(b))
            sm.setCastleLong();
      }

   } else {
      if (pd.pawnAdvanced2()) {
         if (arbiter.verifyDarkEnPassantLeft(b, pd.pawnFile()))
            sm.setEnPassantLeft();
         if (arbiter.verifyDarkEnPassantRight(b, pd.pawnFile()))
            sm.setEnPassantRight();
      }

      if (!pd.darkKing()) {
         if (!pd.darkRook7() && arbiter.verifyDarkCastleShort(b))
            sm.setCastleShort();
         if (!pd.darkRook0() && arbiter.verifyDarkCastleLong(b))
            sm.setCastleLong();
      }
   }
}

//------------------------------------------------------------------------------
void Board::setupNewGame () {
   // start with everything as PC::c0 (i.e. 0)
   b.clear();
   pd.newGame();
   sm.clear();

   // set light pieces
   b.set(0, PC::r0);
   b.set(1, PC::n0);
   b.set(2, PC::b0);
   b.set(3, PC::q0);
   b.set(4, PC::k0);
   b.set(5, PC::b0);
   b.set(6, PC::n0);
   b.set(7, PC::r0);
   b.set(8, PC::p0);
   b.set(9, PC::p0);
   b.set(10, PC::p0);
   b.set(11, PC::p0);
   b.set(12, PC::p0);
   b.set(13, PC::p0);
   b.set(14, PC::p0);
   b.set(15, PC::p0);

   // set dark pieces
   b.set(56, PC::r1);
   b.set(57, PC::n1);
   b.set(58, PC::b1);
   b.set(59, PC::q1);
   b.set(60, PC::k1);
   b.set(61, PC::b1);
   b.set(62, PC::n1);
   b.set(63, PC::r1);
   b.set(48, PC::p1);
   b.set(49, PC::p1);
   b.set(50, PC::p1);
   b.set(51, PC::p1);
   b.set(52, PC::p1);
   b.set(53, PC::p1);
   b.set(54, PC::p1);
   b.set(55, PC::p1);

   // set history state
   testSpecialMoves();
   findCodes();
   writeHashState();
}

//------------------------------------------------------------------------------
void Board::realize (Generator const& generator, Board& child) const {
   child = *this;
   child.clearHashState();
   child.pd.swapTurn();
   child.pd.clearPawnState();

   if (generator.moveIsBasic()) {
      child.b.move(generator.src(), generator.dst());
      child.updatePDBasicMove(generator);
   } else {
      if (generator.specialMoveIsEPLeft()) {
         if (pd.lightMove())
            child.b.lightEnPassantLeft(generator.enPassantFile());
         else
            child.b.darkEnPassantLeft(generator.enPassantFile());
      }
      if (generator.specialMoveIsEPRight()) {
         if (pd.lightMove())
            child.b.lightEnPassantRight(generator.enPassantFile());
         else
            child.b.darkEnPassantRight(generator.enPassantFile());
      }
      if (generator.specialMoveIsCastleShort()) {
         if (pd.lightMove()) {
            child.b.lightCastleShort();
            child.pd.lightKingMoved();
            child.pd.lightRook7Moved();
         } else {
            child.b.darkCastleShort();
            child.pd.darkKingMoved();
            child.pd.darkRook7Moved();
         }
      }
      if (generator.specialMoveIsCastleLong()) {
         if (pd.lightMove()) {
            child.b.lightCastleLong();
            child.pd.lightKingMoved();
            child.pd.lightRook0Moved();
         } else {
            child.b.darkCastleLong();
            child.pd.darkKingMoved();
            child.pd.darkRook0Moved();
         }
      }
   }
   child.testSpecialMoves();
   child.findCodes();
   child.writeHashState();
}

//------------------------------------------------------------------------------
void Board::updatePDBasicMove (Generator const& gen) {
   // for convenience
   Piece p = gen.srcPiece();

   if (isLightPiece(p)) {
      // check if a light pawn double advanced
      if ( p == PC::p0 && gen.pawnMoveIsAdvance2() ) {
         pd.pawnAdvancedOnFile(file(gen.src()));
         return;
      }

      // check if the light king moved
      if (p == PC::k0) { pd.lightKingMoved(); return; }

      // check if a light rook moved
      if (p == PC::r0) {
         if (!pd.lightRook0() && gen.src() == 0) { pd.lightRook0Moved(); return; }
         if (!pd.lightRook7() && gen.src() == 7) { pd.lightRook7Moved(); return; }
      }
   } else {
      // check if a dark pawn double advanced
      if ( p == PC::p0 && gen.pawnMoveIsAdvance2() ) {
         pd.pawnAdvancedOnFile(file(gen.src()));
         return;
      }

      // check if the dark king moved
      if (p == PC::k1) { pd.darkKingMoved(); return; }

      // check if a dark rook moved
      if (p == PC::r1) {
         if (!pd.darkRook0() && gen.src() == 56) { pd.darkRook0Moved(); return; }
         if (!pd.darkRook7() && gen.src() == 63) { pd.darkRook7Moved(); return; }
      }
   }
}



//------------------------------------------------------------------------------
void Board::writeHashState () {
   if (pd.lightMove()) {
      if (sm.canEnPassantLeft()) {
         b.set(code[0], PC::c1);
         writeEnPassantFile();
      } else {
         b.set(code[0], PC::c0);
      }
   } else {
      if (sm.canEnPassantLeft()) {
         b.set(code[0], PC::c3);
         writeEnPassantFile();
      } else {
         b.set(code[0], PC::c2);
      }
   }

   if (sm.canCastleShort())
      b.set(code[1], sm.canCastleLong() ? PC::c3 : PC::c1);
   else
      b.set(code[1], sm.canCastleLong() ? PC::c2 : PC::c0);
}

//------------------------------------------------------------------------------
void Board::writeEnPassantFile () {
   if (pd.pawnFile() & 0x4u)
      b.set(code[2], PC::c1);
   else
      b.set(code[2], PC::c0);

   switch (pd.pawnFile() & 0x3u) {
   case 0:
      b.set(code[3], PC::c0); break;
   case 1:
      b.set(code[3], PC::c1); break;
   case 2:
      b.set(code[3], PC::c2); break;
   case 3:
      b.set(code[3], PC::c3); break;
   }
}


//==============================================================================
// Generator Methods
//==============================================================================

//------------------------------------------------------------------------------
void Generator::checkSpecialMoves () {
   switch (_state) {
   case 0:
      if (isPawn(srcPiece()) && _board->sm.canEnPassantLeft()
          && file(src()) == _board->pd.pawnFile() - 1
      )
         return;
      ++_state;
   case 1:
      if (isPawn(srcPiece()) && _board->sm.canEnPassantRight()
          && file(src()) == _board->pd.pawnFile() + 1
      )
         return;
      ++_state;
   case 2:
      if (isKing(srcPiece()) && _board->sm.canCastleShort()) return;
      ++_state;
   case 3:
      if (isKing(srcPiece()) && _board->sm.canCastleLong()) return;
      ++_state;
   }
}


//==============================================================================
// Arbiter Methods
//==============================================================================

bool Arbiter::putsOrLeavesInCheck (Board const& b, Generator const& gen) {
   Board b2;
   b.realize(gen, b2);
   return pia.isInCheck(b2.bitboard(), b2.pd.lightMove() ? false : true);
}

