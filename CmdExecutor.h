/*********************************************************************************
 * Author: ACE
 * Description: CommandExecutor is responsible for process undo, redo operations
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __CMD_EXECUTOR_H
#define __CMD_EXECUTOR_H

#include "CmdBuffer.h"
#include "PieceTable.h"
#include "TextBuffer.h"
#include "GapBuffer.h"
#include "Commands.h"

//responsible for undo, redo
class CommandExecutor{
public:
  CommandExecutor();
  CommandExecutor(PieceTable* pieceTable, GapBuffer<Index>* gap,TextBuffer* textBuffer);
  void setPieceTable(PieceTable* pt);
  void setGapBuffer(GapBuffer<Index>* gap);
  void setTextBuffer(TextBuffer* tb);
  void beginCmd();
  void endCmd();

  //add cmd to the undo buffer
  CmdHead* addCmd(const CmdHead* cmd);
  CmdHead* addCmd(const void* cmd);
  
  void undo();
  void redo();
  void submit();

  //command
  void setPieceNext(Index pieceIndex, Index next);
  void setPieceNext(Piece& cur, Piece& next);
  void setPiecePre(Index pieceIndex, Index pre);
  void setPiecePre(Piece& cur, Piece& next);
  void setTextStart(Index pieceIndex, Index start);
  void setTextEnd(Index pieceIndex, Index end);
  void setTextStartEnd(Index pieceIndex, Index start, Index end);
  void setPiecePrevNext(Index pieceIndex, Index prev, Index next);
  void setPieceInfo(Index pieceIndex, Index prev, Index next, Index start, Index end);
  Index createPiece();
  template<size_t _Len>
  std::array<Index, _Len> createMultiplePieces(){
    static_assert(_Len > 0);
    std::array<Index, _Len> arrays;
    PieceBatchRemoveCmd* ptr = addCmd(CmdType::PieceBatchRemove,sizeof(CmdHead) + sizeof(Index)*_Len);
    for(int i= 0; i< _Len;++i){
      arrays[i] = pieceTable->add();
      ptr->index[i] = arrays[i];
    }
    return arrays;
  }
  void removePiece(Index pieceIndex);
  //without add charactor
  Index insertEmptyLine(Index lineNo);
  void removeLineBreak(Index lineNo);

  void insertInLineInPieceRedo(const InsertInLineInPieceRedoCmd& cmd);
  void insertInLineInPieceUndo(const InsertInLineInPieceUndoCmd& cmd);
  void insertInLineBetweenPieceRedo(const InsertInLineBetweenPieceRedoCmd& cmd);
  void insertInLineBetweenPieceUndo(const InsertInLineBetweenPieceUndoCmd& cmd);
  void insertBreakInPieceRedo(const InsertBreakInPieceRedoCmd& cmd);
  void insertBreakInPieceUndo(const InsertBreakInPieceUndoCmd& cmd);
  void insertBreakBetweenPieceRedo(const InsertBreakBetweenPieceRedoCmd& cmd);
  void insertBreakBetweenPieceUndo(const InsertBreakBetweenPieceUndoCmd& cmd);
  void removePieceSequenceRedo(const RemovePieceSequenceRedoCmd& cmd);
  void removePieceSequenceUndo(const RemovePieceSequenceUndoCmd& cmd);
  void removeLineBreakRedo(const RemoveLineBreakRedoCmd& cmd);
  void removeLineBreakUndo(const RemoveLineBreakUndoCmd& cmd);
  void removeOneLineWithoutLineBreakRedo(const RemoveOneLineWithoutLineBreakRedoCmd& cmd);
  void removeOneLineWithoutLineBreakUndo(const RemoveOneLineWithoutLineBreakUndoCmd& cmd);
  void removeLineRedo(const RemoveLineRedoCmd& cmd);
  void removeLineUndo(const RemoveLineUndoCmd& cmd);
private:
  void act(CmdBuffer& current, CmdBuffer& target);
  void executeTextStartChangeCommand(const TextStartChangeCmd& cmd);
  void executeTextEndChangeCommand(const TextEndChangeCmd& cmd);
  CmdBuffer undoBuffer;
  CmdBuffer redoBuffer;
  PieceTable* pieceTable = nullptr;
  GapBuffer<Index>* gapBuffer = nullptr;
  TextBuffer* textBuffer = nullptr;
};


#endif