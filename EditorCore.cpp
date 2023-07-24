#include "EditorCore.h"
#include <iostream>

EditorCore::EditorCore(){
  pieceTable = new PieceTable();

  executor.setGapBuffer(&(this->gapBuffer));
  executor.setPieceTable((this->pieceTable));
  executor.setTextBuffer(&(this->textBuffer));

  //insert empty line without commands
  Index ptr = gapBuffer.insert(0);
  Index ind = pieceTable->add();
  gapBuffer[ptr] = ind;

}

EditorCore::~EditorCore(){
  delete pieceTable;
}

void EditorCore::removeInLine(LineNo line, Index columnStart, Index columnEnd){
  Position pos1 = getPiecePosition(line, columnStart);
  Position pos2  = getPiecePosition(line, columnEnd);
  
  bool between1 = (pos1.textPosition == GET(pos1.pieceIndex).getTextEnd());
  bool between2 = (pos2.textPosition == GET(pos2.pieceIndex).getTextEnd());
  if(between1){
    pos1.pieceIndex = GET(pos1.pieceIndex).getNext();
    pos1.textPosition = GET(pos1.pieceIndex).getTextStart();
  }
  if(pos1.pieceIndex == pos2.pieceIndex){
    if(between1 && between2){
      RemovePieceSequenceRedoCmd cmd;
      cmd.pieceStart = pos1.pieceIndex;
      cmd.pieceLast = pos2.pieceIndex;
      cmd.piecePreStartTextEnd = GET(GET(pos1.pieceIndex).getPrev()).getTextEnd();
      cmd.pieceSucLastTextStart = GET(GET(pos2.pieceIndex).getNext()).getTextStart();
      executor.removePieceSequenceRedo(cmd);
    }else if(between1){
      executor.setTextStart(pos2.pieceIndex, pos2.textPosition);
    }else if(between2){
      executor.setTextEnd(pos1.pieceIndex, pos2.textPosition);
    }else{
      InsertInLineBetweenPieceRedoCmd cmd;
      cmd.pieceBefore = pos1.pieceIndex;
      cmd.textStart = pos2.textPosition;
      cmd.textEnd = GET(pos2.pieceIndex).getTextEnd();
      executor.insertInLineBetweenPieceRedo(cmd);
      executor.setTextEnd(pos1.pieceIndex, pos1.textPosition);
    }
  }else{
    Index pre = between1? GET(pos1.pieceIndex).getPrev():pos1.pieceIndex;
    Index next = between2? GET(pos2.pieceIndex).getNext():pos2.pieceIndex;
    if((!between1)&&(!between2)&& GET(pos1.pieceIndex).getNext()== pos2.pieceIndex){
      executor.setTextEnd(pos1.pieceIndex, pos1.textPosition);
      executor.setTextStart(pos2.pieceIndex, pos2.textPosition);
    }else{
      RemovePieceSequenceRedoCmd cmd;
      cmd.pieceStart = GET(pre).getNext();//pos1.pieceIndex;
      cmd.pieceLast = GET(next).getPrev();
      cmd.piecePreStartTextEnd = between1? GET(pre).getTextEnd():pos1.textPosition;
      cmd.pieceSucLastTextStart = between2? GET(next).getTextStart(): pos2.textPosition;
      executor.removePieceSequenceRedo(cmd);
    }

  }
}

void EditorCore::removeOneLineWithoutLineBreak(LineNo line){
  RemoveOneLineWithoutLineBreakRedoCmd cmd;
  cmd.tail = *gapBuffer.get(line);
  executor.removeOneLineWithoutLineBreakRedo(cmd);
}

void EditorCore::undo(){
  executor.undo();
}

void EditorCore::redo(){
  executor.redo();
}

void EditorCore::insertInLine(LineNo line, Index column, const Character* chs, Length length){
  Position position = getPiecePosition(line, column);
  insertInLine(position, chs, length);
}

void EditorCore::insertInLine(const Position& position, const Character* chs, Length length){
  Index headIndex = gapBuffer[position.line];
  Index rover = headIndex;

  Index textStart = textBuffer.getEnd();
  textBuffer.addCharacters(chs, length);
  Index textEnd = textBuffer.getEnd();

  if(position.textPosition < GET(position.pieceIndex).getTextEnd()){
      InsertInLineInPieceRedoCmd cmd;
      cmd.leftTextStart = GET(position.pieceIndex).getTextStart();
      cmd.leftTextEnd = position.textPosition;
      cmd.rightTextStart = position.textPosition;
      cmd.rightTextEnd = GET(position.pieceIndex).getTextEnd();
      cmd.ori = position.pieceIndex;
      cmd.oriTextStart = textStart;
      cmd.oriTextEnd = textEnd;
      executor.insertInLineInPieceRedo(cmd);

      lastEdit = position.pieceIndex;
  }
  else if(position.textPosition == GET(position.pieceIndex).getTextEnd()){
    Index txtEnd = GET(position.pieceIndex).getTextEnd();
    Index txtStart = GET(position.pieceIndex).getTextStart();
    bool isTailPiece = (txtEnd==txtStart)? true : (textBuffer.data(txtStart)->ch=='\n');
    bool append = (txtEnd == textStart); //textStart is the original textEnd
    if((append&& (!isTailPiece)) /*lastEdit == position.pieceIndex*/){
      executor.setTextEnd(position.pieceIndex,GET(position.pieceIndex).getTextEnd()+ textEnd- textStart);
    }else{
      InsertInLineBetweenPieceRedoCmd cmd;
      cmd.pieceBefore = position.pieceIndex;
      cmd.textStart = textStart;
      cmd.textEnd = textEnd;
      executor.insertInLineBetweenPieceRedo(cmd);
    }
  }
  else{
    assert(false);
  }

}

void EditorCore::insertLineBreak(LineNo line, Index column){
  Position pos = getPiecePosition(line,column);

  Index textStart = textBuffer.getEnd();
  textBuffer.addCharacter(Character('\n'));
  Index textEnd = textBuffer.getEnd();

  //has linebreak line
  if(GET(pos.headIndex).getTextEnd()-GET(pos.headIndex).getTextStart() !=0){
    //executor.setTextStartEnd(gapBuffer[ind],textStart,textEnd);
  }else{
    //give old line a linebreak
    executor.setTextStartEnd(pos.headIndex, textStart, textEnd);
    textStart = 0;
    textEnd = 0;
  }
  if(pos.textPosition == GET(pos.pieceIndex).getTextEnd()){
    if(GET(pos.pieceIndex).getNext() != pos.headIndex){
      InsertBreakBetweenPieceRedoCmd cmd;
      cmd.pieceIndex = pos.pieceIndex;
      cmd.line  = pos.line;
      cmd.textStart = textStart;
      cmd.textEnd = textEnd;
      executor.insertBreakBetweenPieceRedo(cmd);
    }else{
      Index ind = executor.insertEmptyLine(line + 1);
    }
  }else if(pos.textPosition < GET(pos.pieceIndex).getTextEnd()){
    InsertBreakInPieceRedoCmd cmd;
    cmd.textStart = textStart;
    cmd.textEnd = textEnd;
    cmd.textSplitPoint = pos.textPosition;
    cmd.pieceIndex = pos.pieceIndex;
    cmd.lineNo = line;
    executor.insertBreakInPieceRedo(cmd);
  }
}

Length EditorCore::getLineLength(LineNo line) const{
  Length length = 0;
  getLineText(line, nullptr,length);
  return length;
}

void EditorCore::getLineText(LineNo line, Character* data, Length& length) const{
  Index* pt = gapBuffer.get(line);
  Index rover = *pt;
  length = 0;
  rover = pieceTable->nextIndex(rover);
  Index head = rover;
  do{
    Piece& piece = pieceTable->get(rover);
    Length len = piece.getTextEnd()-piece.getTextStart();
    if(data != nullptr){
      std::memcpy(data +length, textBuffer.data(piece.getTextStart()), len);
    }
    length += len;
    rover = pieceTable->nextIndex(rover);
  }while(rover != head);
  return;
}

String EditorCore::getLineText(LineNo line) const{
  String text;
  Index* pt = gapBuffer.get(line);
  Index rover = *pt;
  Length length = 0;
  rover = pieceTable->nextIndex(rover);
  Index head = rover;
  do{
    Piece& piece = pieceTable->get(rover);
    Index start = piece.getTextStart();
    Length len = piece.getTextEnd() - start;
    const Character* chs = textBuffer.data(start);
    for(Length i = 0; i< len; ++i){
      text.push_back(chs[i].ch);
    }
    length += len;
    rover = pieceTable->nextIndex(rover);
  }while(rover != head);
  return text;
}

Length EditorCore::getLineCount() const{
  return gapBuffer.size();
}

String EditorCore::getContent() const{
  Length l = gapBuffer.size();
  Length lineLength = 0;
  Length totalLength = 0;
  for(LineNo i =0; i<l;++i){
    lineLength = getLineLength(i);
    totalLength += lineLength;
  }
  String text;
  for(LineNo i = 0; i < l; ++i){
    Index* ptr = gapBuffer.get(i);
    Index rover = GET(*ptr).getNext();
    Index head = rover;
    
    do{
      auto start = GET(rover).getTextStart();
      auto end = GET(rover).getTextEnd();
      for(Index j = start; j < end;++j){
        text.push_back(textBuffer.data(j)->ch);
      }
      rover = GET(rover).getNext();
    }while(rover != head);
  }
  return text;
}

void EditorCore::_appendText(const String& text){
  Index ind = gapBuffer.size();
  //last line is always an line without breakline
  Length len = getLineLength(ind - 1);
  _insertText(ind - 1, len, text);
}

void EditorCore::_insertText(LineNo line, Index column, const String& text){
  const Character* chs = (const Character*)(text.data());
  insertText(line,column, chs, text.size());
}

bool EditorCore::hasBreakLine(LineNo id) const{
  auto& it = GET(*gapBuffer.get(id));
  return (it.getTextEnd() - it.getTextStart()) != 0;
}

void EditorCore::insertText(LineNo line, Index column, const Character* chs, Length length){
  Index prev = 0;
  for(Length i= 0; i <= length; ++i){
    if((i== length) ||(chs[i].ch == '\n')){
      if(i - prev >0){
        insertInLine(line, column, chs+prev, i - prev);
      }
      if(chs[i].ch == '\n'){
        insertLineBreak(line, column + i - prev);
      }
      line += 1;
      column = 0;
      prev = i+1;
    }
  }
}

void EditorCore::_removeText(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd){
  Position pos = getPiecePosition(lineLast, columnEnd);
  Piece& piece = pieceTable->get(pos.pieceIndex);
  bool last = (piece.getNext()== pos.headIndex) && piece.getTextEnd() == pos.textPosition;
  
  if(lineStart == lineLast){
    if(columnStart == 0 && last){
      removeOneLine(lineStart);
      LOG
    }else{
      removeInLine(lineStart, columnStart, columnEnd);
      LOG
    }
  }else{
    //we remove from last to first so that previous line id won't be affected
    if(last){
      removeOneLineWithoutLineBreak(lineLast);
      LOG
    }else{
      removeInLine(lineLast, 0, columnEnd);
      LOG
    } 

    for(LineNo i=lineLast-  1; i > lineStart;--i){
      removeOneLine(i);
      LOG
    }

    if(columnStart == 0){
      removeOneLine(lineStart);
      LOG
    }else{
      Length firstLineEnd = getLineLength(lineStart);
      if(hasBreakLine(lineStart)){
        firstLineEnd -= 1;
      }
      removeInLine(lineStart,columnStart, firstLineEnd);
      removeLineBreak(lineStart);
      LOG
    }
  }
}

void EditorCore::removeOneLine(LineNo id){
  Index* ptr = gapBuffer.get(id);
  LineCmd cmd(CmdType::InsertLine, id);
  cmd.pieceIndex = *ptr;
  gapBuffer.remove(id);
  executor.addCmd(&cmd);
}

void EditorCore::removeLineBreak(LineNo id){
  RemoveLineBreakRedoCmd cmd;
  cmd.line = id;
  executor.removeLineBreakRedo(cmd);
  executor.removeLineBreak(id);
}

Position EditorCore::getPiecePosition(LineNo id, Index column){
  Position pos;
  pos.line = id;
  pos.column = column;
  pos.headIndex = *gapBuffer.get(id);
  Piece* head = &(pieceTable->get(pos.headIndex));
  Piece* rover = head;
  Length l = 0;

  do{
    if(column <= l){
      pos.pieceIndex = pieceTable->getAddress(*rover);
      pos.textPosition = rover->getTextEnd() - (l-column);
      break;
    }
    rover = &(pieceTable->get(rover->getNext()));
    l += rover->getTextEnd() - rover->getTextStart();
  }while(head != rover);

  return pos;
}

