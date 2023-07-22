#include "EditorCore.h"

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
  Index headIndex = gapBuffer[line];
  Index rover = headIndex;
  Length sum = 0;

  Index removeFirstIndex = 0;
  Index removeLastIndex = 0;
  size_t count_first = 0;
  size_t count_last = 0;
  size_t count = 0;
  size_t ok = 0;

  Index newStart = -1;
  Index newEnd = -1;
  TextEndChangeCmd endChange;
  TextStartChangeCmd startChange;
  do{
    Piece& piece =  pieceTable->get(rover);
    Length l = (rover == headIndex)? 0 :(piece.getTextEnd() - piece.getTextStart());
    sum += l;
    if(columnStart < sum){
      removeFirstIndex = piece.getNext();
      count_first = count + 1;
      newEnd = piece.getTextStart() + columnStart - sum + l;
      endChange.pieceIndex = rover;
      endChange.textEndTarget = piece.getTextEnd();
      ok += 1;
    }else if(columnStart == sum){
      removeFirstIndex = piece.getNext();
      count_first = count + 1;
      ok += 1;
    }
    if(columnEnd < sum){
      removeLastIndex = piece.getPrev();
      count_last = count - 1;
      newStart = piece.getTextStart() + columnEnd - sum + l;
      startChange.pieceIndex = rover;
      startChange.textStartTarget = piece.getTextStart();
      ok += 1;
    }else if(columnEnd == sum){
      removeLastIndex = rover;
      count_last = count;
      ok += 1;
    }
    if(ok == 2){
      break;
    }
    rover = piece.getNext();
    count += 1;
  }while(rover != headIndex);
  if(count_last >= count_first){
    executor.setPieceNext(pieceTable->prevIndex(removeFirstIndex),pieceTable->nextIndex(removeLastIndex));
    executor.setPiecePre(pieceTable->nextIndex(removeLastIndex), pieceTable->prevIndex(removeFirstIndex));
  }
  if(newEnd!= -1){
    executor.setTextEnd(pieceTable->prevIndex(removeFirstIndex), newEnd);
  }
  if(newStart!= -1){
    executor.setTextStart(pieceTable->nextIndex(removeLastIndex),newStart);
  }
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
  Index textStart = textBuffer.getEnd();
  textBuffer.addCharacters(chs, length);
  Index textEnd = textBuffer.getEnd();
  
  Index headIndex = gapBuffer[position.line];
  Index rover = headIndex;

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
    if(lastEdit == position.pieceIndex){
      //this is another method to decide,
      Index end = textBuffer.getEnd();
      assert(GET(position.pieceIndex).getTextEnd() == end);
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

  //Index ind = executor.insertEmptyLine(line + 1);

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
    for(int i = 0; i< len; ++i){
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
      for(int j = start; j < end;++j){
        text.push_back(textBuffer.data(j)->ch);
      }
      rover = GET(rover).getNext();
    }while(rover != head);
  }
  return text;
}

void EditorCore::appendText(const String& text){
  Index ind = gapBuffer.size();
  //last line is always an line without breakline
  Length len = getLineLength(ind - 1);
  insertText(ind - 1, len, text);
}

void EditorCore::insertText(LineNo line, Index column, const String& text){
  const Character* chs = (const Character*)(text.data());
  insertText(line,column, chs, text.size());
}

void EditorCore::insertText(LineNo line, Index column, const Character* chs, Length length){
  int prev = 0;
  for(int i= 0; i <= length; ++i){
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

void EditorCore::removeText(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd){
  Position pos = getPiecePosition(lineStart, columnEnd);
  Piece& piece = pieceTable->get(pos.pieceIndex);
  bool last = (piece.getNext()== pos.headIndex) && piece.getTextEnd() == pos.textPosition;
  
  if(lineStart == lineLast){
    if(columnStart == 0 && last){
      removeLineBreak(lineStart);
    }else{
      removeInLine(lineStart, columnStart, columnEnd);
    }
  }else{
    Length l = lineLast -lineStart - 1;
    if(columnStart == 0){
      removeLineBreak(lineStart);
    }else{
      Length firstLineEnd = getLineLength(lineStart);
      removeInLine(lineStart,columnStart, firstLineEnd);
      lineStart += 1;
    }
    for(int i=0; i<l;++i){
      removeLineBreak(lineStart);
    }
    if(last){
      removeLineBreak(lineStart);
    }else{
      removeInLine(lineStart, 0, columnEnd);
    } 
  }
}

void EditorCore::removeLineBreak(LineNo id){
  Index* ptr = gapBuffer.get(id);
  LineCmd cmd(CmdType::InsertLine, id);
  cmd.pieceIndex = *ptr;
  gapBuffer.remove(id);
  executor.addCmd(&cmd);
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

