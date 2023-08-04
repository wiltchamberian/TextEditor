/*********************************************************************************
 * Author: ACE
 * Description: All the undo,redo commands
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "Define.h"

class CmdHead{
public:
  CmdHead(){}
  CmdHead(CmdType t):type(t){
  }
  CmdType type;
  uint16_t length;
};

class PieceInfoChangeCmd{
public:
  PieceInfoChangeCmd(){
    head.type = CmdType::PieceInfoChange;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index textStart;
  Index textEnd;
  Index prev;
  Index next;
};

class SetPrevNextCmd{
public:
  SetPrevNextCmd(){
    head.type = CmdType::SetPrevNext;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index prev;
  Index next;
};

class TextStartChangeCmd{
public:
  TextStartChangeCmd(){
    head.type = CmdType::TextStartChange;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index textStartTarget;
};

class TextEndChangeCmd{
public:
  TextEndChangeCmd(){
    head.type = CmdType::TextEndChange;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index textEndTarget;
};

class TextStartEndChangeCmd{
public:
  TextStartEndChangeCmd(){
    head.type = CmdType::TextStartEndChange;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index textStart;
  Index textEnd;
};

class EndCmd{
public:
  EndCmd(){
    head.type = CmdType::End;
    head.length = sizeof(*this);
  }
  CmdHead head;
};

class LineCmd{
public:
  LineCmd(CmdType type){
    head.type = type;
    head.length = sizeof(*this);
  }
  LineCmd(CmdType type, LineNo l):head(type),line(l){
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;
  Index pieceIndex;
};

class RemoveLineRedoCmd{
public:
  RemoveLineRedoCmd(){
    head.type = CmdType::RemoveLineRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;
};

class RemoveLineUndoCmd{
public:
  RemoveLineUndoCmd(){
    head.type = CmdType::RemoveLineUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;
  Index pieceTail;
};

class InsertEmptyLineCmd{
public:
  InsertEmptyLineCmd(){
    head.type = CmdType::InsertEmptyLine;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index lineNo;
  Index pieceIndex;
};

class RemoveEmptyLineCmd{
public:
  RemoveEmptyLineCmd(){
    head.type = CmdType::RemoveEmptyLine;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index lineNo;
  Index pieceIndex;
};

//to replace inline cmd
class SetNextCmd{
public:
  SetNextCmd()
  { 
    head.type = CmdType::SetNextCmd; 
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index next;
};

class SetPrevCmd{
public:
  SetPrevCmd()
  { 
    head.type = CmdType::SetPrevCmd; 
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index prev;
};

class PieceCreateCmd{
public:
  PieceCreateCmd(){
    head.type = CmdType::PieceCreate;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index prev;
  Index next;
};

class PieceRemoveCmd{
public:
  PieceRemoveCmd(){
    head.type = CmdType::PieceRemove;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
};

class InsertInLineInPieceRedoCmd{
public:
  InsertInLineInPieceRedoCmd(){
    head.type = CmdType::InsertInLineInPieceRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index ori; //used as new middle
  Index oriTextStart;
  Index oriTextEnd;
  Index leftTextStart;
  Index leftTextEnd;
  Index rightTextStart;
  Index rightTextEnd;
};

class InsertInLineInPieceUndoCmd{
public:
  InsertInLineInPieceUndoCmd(){
    head.type = CmdType::InsertInLineInPieceUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index ori; //used as new middle
  Index oriTextStart;
  Index oriTextEnd;
};

class InsertInLineBetweenPieceRedoCmd{
public:
  InsertInLineBetweenPieceRedoCmd(){
    head.type = CmdType::InsertInLineBetweenPieceRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index textStart;
  Index textEnd;
  Index pieceBefore;
};

class InsertInLineBetweenPieceUndoCmd{
public:
  InsertInLineBetweenPieceUndoCmd(){
    head.type = CmdType::InsertInLineBetweenPieceUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
};

class InsertBreakInPieceRedoCmd{
public:
  InsertBreakInPieceRedoCmd(){
    head.type = CmdType::InsertBreakInPieceRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceIndex;
  Index textSplitPoint;
  Index lineNo;
  Index textStart;
  Index textEnd;
};

class InsertBreakInPieceUndoCmd{
public:
  InsertBreakInPieceUndoCmd(){
    head.type = CmdType::InsertBreakInPieceUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;
};

class InsertBreakBetweenPieceRedoCmd{
public:
  InsertBreakBetweenPieceRedoCmd(){
    head.type = CmdType::InsertBreakBetweenPieceRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index line;
  Index pieceIndex;
  Index textStart;
  Index textEnd;
};

class InsertBreakBetweenPieceUndoCmd{
public:
  InsertBreakBetweenPieceUndoCmd(){
    head.type = CmdType::InsertBreakBetweenPieceUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index line;
};

class RemovePieceSequenceUndoCmd{
public:
  RemovePieceSequenceUndoCmd(){
    head.type = CmdType::RemovePieceSequenceUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceStart;
  Index pieceLast;
  Index piecePreStart;
  Index pieceSucLast;
  Index piecePreStartTextEnd;
  Index pieceSucLastTextStart;
};

class RemovePieceSequenceRedoCmd{
public:
  RemovePieceSequenceRedoCmd(){
    head.type = CmdType::RemovePieceSequenceRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index pieceStart;
  Index pieceLast;
  Index piecePreStartTextEnd;
  Index pieceSucLastTextStart;
};

class RemoveLineBreakRedoCmd{
public:
  RemoveLineBreakRedoCmd(){
    head.type = CmdType::RemoveLineBreakRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;
};

class RemoveLineBreakUndoCmd{
public:
  RemoveLineBreakUndoCmd(){
    head.type = CmdType::RemoveLineBreakUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  LineNo line;//insert position
  Index pieceIndex; //the start of new line
};

class RemoveOneLineWithoutLineBreakRedoCmd{
public:
  RemoveOneLineWithoutLineBreakRedoCmd(){
    head.type = CmdType::RemoveOneLineWithoutLineBreakRedo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index tail;
  Index last;
};

class RemoveOneLineWithoutLineBreakUndoCmd{
public:
  RemoveOneLineWithoutLineBreakUndoCmd(){
    head.type = CmdType::RemoveOneLineWithoutLineBreakUndo;
    head.length = sizeof(*this);
  }
  CmdHead head;
  //first->...->last->tail->(first)
  Index tail;
  Index first;
  Index last;
};

///////////////////////////////

#endif