/*********************************************************************************
 * Author: ACE
 * Description: common defines and type of commands
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __DEFINE_H
#define __DEFINE_H

#include <string>
#include <cassert>
#include <array>
#include <vector>

using String = std::string;

#define GET(a) pieceTable->get(a)
using LineNo = uint32_t;
using Index = uint32_t;
using Length = uint32_t;

enum class CmdType : uint16_t{
  Append,
  InsertLine,
  RemoveLine,
  LineCmd,
  InLineRemove,
  InLineInsert,
  InsertLineBreak,
  RemoveLineBreak,
  LinkCmd,
  TextStartChange,
  TextEndChange,
  SetNextCmd,
  SetPrevCmd,
  PieceCreate,
  PieceRemove,
  TextStartEndChange,
  PieceInfoChange,
  SetPrevNext,
  // a specail command used to group multiple commands together.
  //and flag the consequent commands end
  End, 
  
  PieceBatchCreate,
  PieceBatchRemove,

  InsertInLineInPieceRedo,
  InsertInLineBetweenPieceRedo,
  InsertInLineInPieceUndo,
  InsertInLineBetweenPieceUndo,
  InsertBreakInPieceRedo,
  InsertBreakInPieceUndo,
  InsertBreakBetweenPieceRedo,
  InsertBreakBetweenPieceUndo,
  RemovePieceSequenceRedo,
  RemovePieceSequenceUndo,

  //after these are simple cmd, which may not perserve the underlying structure
  //but logically structure is peserved.
  SimpleInsert,
  SimpleRemove,
};


#endif