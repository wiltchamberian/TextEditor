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

class InsertLineBreakCmd{
public:
  InsertLineBreakCmd(){
    head.type = CmdType::InsertLineBreak;
    head.length = sizeof(*this);
  }
  CmdHead head;
  Index lineNo;
  Index pieceIndex;
};

class RemoveLineBreakCmd{
public:
  RemoveLineBreakCmd(){
    head.type = CmdType::RemoveLineBreak;
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

class PieceBatchCreateCmd{
public:
  PieceBatchCreateCmd(){
    head.type = CmdType::PieceBatchCreate;
  }
  CmdHead head;
  Index pieceIndex[1];
};

class PieceBatchRemoveCmd{
public:
  PieceBatchRemoveCmd(){
    head.type = CmdType::PieceBatchRemove;
  }
  CmdHead head;
  Index pieceIndex[1];
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

///////////////////////////////

#endif