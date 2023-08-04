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

#define INVALID_HANDLE (uint32_t)(-1)

template<typename _T>
using Vec = std::vector<_T>;

using String = std::string;

#define GET(a) pieceTable->get(a)
using LineNo = uint32_t;
using Index = uint32_t;
using Length = uint32_t;
using Byte = uint8_t;
using Real = float;
using Num = uint32_t;

//if you want to debug uncomment this sentence and comment "#define LOG"
//#define LOG std::cout<< this->getContent()<<"\n\n*******************\n\n";
#define LOG

enum class CmdType : uint16_t{
  Append,
  InsertLine,
  RemoveLine,
  LineCmd,
  InLineRemove,
  InLineInsert,
  InsertEmptyLine,
  RemoveEmptyLine,
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
  RemoveLineBreakRedo,
  RemoveLineBreakUndo,
  RemoveOneLineWithoutLineBreakRedo,
  RemoveOneLineWithoutLineBreakUndo,
  RemoveLineRedo,
  RemoveLineUndo,
  
  //after these are simple cmd, which may not perserve the underlying structure
  //but logically structure is peserved.
  SimpleInsert,
  SimpleRemove,
};


#endif