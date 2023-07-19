#include "EditorCore.h"

CmdBuffer::~CmdBuffer(){
  delete[] chunk;
}

CmdHead* CmdBuffer::addCmd(CmdType type, Length length){
  if(used_size + length > chunk_size){
    //re_allocate, by stragegy like vector
    uint8_t* tmp = chunk;
    Length newSize = std::max<size_t>(chunk_size * 2, used_size + length);
    chunk = new uint8_t[newSize];
    std::memcpy(chunk + newSize - used_size, tmp + chunk_size - used_size, used_size);
    delete[] tmp;
    chunk_size = newSize;
  }
  Length pos = chunk_size - used_size - length;
  ((CmdHead*)(chunk+pos))->type = type;
  ((CmdHead*)(chunk+pos))->length = length;
  used_size += length;
  cmd_count += 1;
  return (CmdHead*)(chunk+pos);  
}

CmdHead* CmdBuffer::addCmd(const CmdHead* cmd){
  CmdHead* res = addCmd(cmd->type, cmd->length);
  std::memcpy(res, cmd, cmd->length);
  return res;
}

CmdHead* CmdBuffer::addCmd(const void* cmd){
  return addCmd((const CmdHead*)cmd);
}

CmdHead* CmdBuffer::removeCmd(){
  CmdHead* head = (CmdHead*)(chunk + chunk_size - used_size);
  used_size -= head->length;
  cmd_count -= 1;
  return head;
}

bool CmdBuffer::empty() const{
  return cmd_count == 0;
}

CommandExecutor::CommandExecutor(){

}

CommandExecutor::CommandExecutor(PieceTable* table, GapBuffer<Index>* gap,TextBuffer* tb)
:pieceTable(table)
,gapBuffer(gap)
,textBuffer(tb)
{

}

void CommandExecutor::setPieceTable(PieceTable* pt){
  pieceTable = pt;
}

void CommandExecutor::setGapBuffer(GapBuffer<Index>* gap){
  gapBuffer = gap;
}

void CommandExecutor::setTextBuffer(TextBuffer* tb){
  textBuffer = tb;
}

void CommandExecutor::beginCmd(){
  EndCmd cmd;
  undoBuffer.addCmd(&cmd);
}

void CommandExecutor::endCmd(){
}

void CommandExecutor::executeTextStartChangeCommand(const TextStartChangeCmd& cmd){
  GET(cmd.pieceIndex).setTextStart(cmd.textStartTarget);
}

void CommandExecutor::executeTextEndChangeCommand(const TextEndChangeCmd& cmd){
  GET(cmd.pieceIndex).setTextEnd(cmd.textEndTarget);
}

CmdHead* CommandExecutor::addCmd(const CmdHead* cmd){
  return undoBuffer.addCmd(cmd);
}

CmdHead* CommandExecutor::addCmd(const void* cmd){
  return undoBuffer.addCmd((const CmdHead*)cmd);
}

void CommandExecutor::setPieceNext(Index pieceIndex, Index next){
  SetNextCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.next = pieceTable->get(pieceIndex).getNext();
  pieceTable->setNext(pieceIndex,next);
  addCmd(&cmd);
}

void CommandExecutor::setPieceNext(Piece& cur, Piece& next){
  SetNextCmd cmd;
  cmd.pieceIndex = pieceTable->getAddress(cur);
  cmd.next = cur.getNext();
  pieceTable->setNext(cur, next);
  addCmd(&cmd);
}

void CommandExecutor::setPiecePre(Index pieceIndex, Index pre){
  SetPrevCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.prev = pieceTable->get(pieceIndex).getPrev();
  pieceTable->setNext(pieceIndex,pre);
  addCmd(&cmd);
}

void CommandExecutor::setPiecePre(Piece& cur, Piece& prev){
  SetPrevCmd cmd;
  cmd.pieceIndex = pieceTable->getAddress(cur);
  cmd.prev = cur.getPrev();
  pieceTable->setNext(cur, prev);
  addCmd(&cmd);
}

void CommandExecutor::setTextStart(Index pieceIndex, Index start){
  TextStartChangeCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.textStartTarget = pieceTable->get(pieceIndex).getTextStart();
  pieceTable->get(pieceIndex).setTextStart(start);
  addCmd(&cmd);
}

void CommandExecutor::setTextEnd(Index pieceIndex, Index end){
  TextEndChangeCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.textEndTarget = pieceTable->get(pieceIndex).getTextEnd();
  pieceTable->get(pieceIndex).setTextEnd(end);
  addCmd(&cmd);
}

void CommandExecutor::setTextStartEnd(Index pieceIndex, Index start, Index end){
  TextStartEndChangeCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.textStart = GET(pieceIndex).getTextStart();
  cmd.textEnd = GET(pieceIndex).getTextEnd();
  GET(pieceIndex).setTextStart(start);
  GET(pieceIndex).setTextEnd(end);
  addCmd(&cmd);
}

void CommandExecutor::setPiecePrevNext(Index pieceIndex, Index prev, Index next){
  SetPrevNextCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.prev = GET(pieceIndex).getPrev();
  cmd.next = GET(pieceIndex).getNext();
  GET(pieceIndex).setPrev(prev);
  GET(pieceIndex).setNext(next);
  addCmd(&cmd);
}

void CommandExecutor::setPieceInfo(Index pieceIndex, Index prev, Index next, Index start, Index end){
  PieceInfoChangeCmd cmd;
  cmd.pieceIndex = pieceIndex;
  cmd.prev = GET(pieceIndex).getPrev();
  cmd.next = GET(pieceIndex).getNext();
  cmd.textStart = GET(pieceIndex).getTextStart();
  cmd.textEnd = GET(pieceIndex).getTextEnd();
  GET(pieceIndex).setPrev(prev);
  GET(pieceIndex).setNext(next);
  GET(pieceIndex).setTextStart(start);
  GET(pieceIndex).setTextEnd(end);
  addCmd(&cmd);
}

Index CommandExecutor::createPiece(){
  Index ind = pieceTable->add();
  PieceRemoveCmd cmd;
  cmd.pieceIndex = ind;
  addCmd(&cmd);
  return ind;
}

void CommandExecutor::removePiece(Index pieceIndex){
  PieceCreateCmd cmd;
  cmd.prev = pieceTable->get(pieceIndex).getPrev();
  cmd.next = pieceTable->get(pieceIndex).getNext();
  addCmd(&cmd);
  pieceTable->remove(pieceIndex);
}

Index CommandExecutor::insertEmptyLine(Index lineNo){
  Index index = gapBuffer->insert(lineNo);
  Index ind = pieceTable->add();
  (*gapBuffer)[index] = ind;

  RemoveLineBreakCmd cmd;
  cmd.lineNo = lineNo;
  cmd.pieceIndex = ind;
  addCmd(&cmd);
  return index;
}

void CommandExecutor::removeLineBreak(Index lineNo){


}

void CommandExecutor::act(CmdBuffer& current, CmdBuffer& target){
  EndCmd run;
  target.addCmd(&run);
  while(!current.empty()){
    CmdHead* head = current.removeCmd();
    if(head->type == CmdType::End){
      break;
    }
    switch(head->type){
      case CmdType::InsertLineBreak:{
        InsertLineBreakCmd* cmd = (InsertLineBreakCmd*)(head);
        Index ind = pieceTable->add();
        Index pt = gapBuffer->insert(cmd->lineNo);
        (*gapBuffer)[pt] = ind;
        RemoveLineBreakCmd newCmd;
        newCmd.lineNo = cmd->lineNo;
        newCmd.pieceIndex = ind;
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::RemoveLineBreak:{
        RemoveLineBreakCmd* cmd = (RemoveLineBreakCmd*)(head);
        pieceTable->remove(cmd->pieceIndex);
        gapBuffer->remove(cmd->lineNo);
        InsertLineBreakCmd newCmd;
        newCmd.lineNo = cmd->lineNo;
        newCmd.pieceIndex = cmd->pieceIndex;
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::PieceCreate:{
        PieceCreateCmd* cmd = (PieceCreateCmd*)head;
        Index ind = pieceTable->add();
        PieceRemoveCmd newCmd;
        newCmd.pieceIndex = ind;
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::PieceRemove:{
        PieceRemoveCmd* cmd = (PieceRemoveCmd*)head;
        PieceCreateCmd newCmd;
        newCmd.prev = pieceTable->get(cmd->pieceIndex).getPrev();
        newCmd.next = pieceTable->get(cmd->pieceIndex).getNext();
        target.addCmd(&newCmd);
        pieceTable->remove(cmd->pieceIndex);
      }
      break;
      case CmdType::PieceBatchCreate:{
        PieceBatchCreateCmd* cmd = (PieceBatchCreateCmd*)head;
        PieceBatchRemoveCmd* newCmd =(PieceBatchRemoveCmd*)target.addCmd(CmdType::PieceBatchRemove, cmd->head.length);
        size_t count = (cmd->head.length - sizeof(CmdHead))/sizeof(Index);
        for(int i=0; i<count;++i){
          newCmd->pieceIndex[i] = pieceTable->add();
        }
      }
      break;
      case CmdType::PieceBatchRemove:{
        PieceBatchRemoveCmd* cmd = (PieceBatchRemoveCmd*)head;
        PieceBatchCreateCmd* newCmd =(PieceBatchCreateCmd*)target.addCmd(CmdType::PieceBatchCreate, cmd->head.length);
        size_t count = (cmd->head.length - sizeof(CmdHead))/sizeof(Index);
        for(int i= 0; i< count;++i){
          newCmd->pieceIndex[i] = cmd->pieceIndex[i];
        }
        for(int i=0; i<count;++i){
          pieceTable->remove(cmd->pieceIndex[i]);
        }
      }
      break;
      case CmdType::SetPrevCmd:{
        SetPrevCmd* cmd = (SetPrevCmd*)head;
        Piece& piece = pieceTable->get(cmd->pieceIndex);
        SetPrevCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.prev = piece.getPrev();
        piece.setPrev(cmd->prev);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::SetNextCmd:{
        SetNextCmd* cmd = (SetNextCmd*)head;
        Piece& piece = pieceTable->get(cmd->pieceIndex);
        SetNextCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.next = piece.getNext();
        piece.setNext(cmd->next);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::TextEndChange:
      {
        TextEndChangeCmd* cmd = (TextEndChangeCmd*)head;

        TextEndChangeCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.textEndTarget = GET(cmd->pieceIndex).getTextEnd();
        executeTextEndChangeCommand(*cmd);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::TextStartChange:
      {
        TextStartChangeCmd* cmd = (TextStartChangeCmd*)head;
        TextStartChangeCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.textStartTarget = GET(cmd->pieceIndex).getTextStart();
        executeTextStartChangeCommand(*cmd);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::InsertLine:
      {
        LineCmd* cmd = (LineCmd*)head;
        Index look = gapBuffer->insert(cmd->line);
        (*gapBuffer)[look] = cmd->pieceIndex;

        LineCmd newCmd(CmdType::RemoveLine);
        newCmd.line = cmd->line;
        newCmd.pieceIndex = cmd->pieceIndex;
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::RemoveLine:{
        LineCmd* cmd = (LineCmd*)head;
        gapBuffer->remove(cmd->line);

        LineCmd newCmd(CmdType::InsertLine);
        newCmd.line = cmd->line;
        newCmd.pieceIndex = cmd->pieceIndex;
        target.addCmd(&newCmd);     
      }
      break;
      case CmdType::TextStartEndChange:{
        TextStartEndChangeCmd* cmd = (TextStartEndChangeCmd*)head;
        TextStartEndChangeCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.textStart = GET(cmd->pieceIndex).getTextStart();
        newCmd.textEnd =  GET(cmd->pieceIndex).getTextEnd(); 
        GET(cmd->pieceIndex).setTextStart(cmd->textStart);
        GET(cmd->pieceIndex).setTextEnd(cmd->textEnd);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::SetPrevNext:{
        SetPrevNextCmd* cmd = (SetPrevNextCmd*)head;
        SetPrevNextCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.prev = GET(cmd->pieceIndex).getPrev();
        newCmd.next =  GET(cmd->pieceIndex).getNext(); 
        GET(cmd->pieceIndex).setPrev(cmd->prev);
        GET(cmd->pieceIndex).setNext(cmd->next);
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::PieceInfoChange:{
        PieceInfoChangeCmd* cmd = (PieceInfoChangeCmd*)head;
        PieceInfoChangeCmd newCmd;
        newCmd.pieceIndex = cmd->pieceIndex;
        newCmd.prev = GET(cmd->pieceIndex).getPrev();
        newCmd.next =  GET(cmd->pieceIndex).getNext(); 
        newCmd.textStart = GET(cmd->pieceIndex).getTextStart();
        newCmd.textEnd = GET(cmd->pieceIndex).getTextEnd();
        GET(cmd->pieceIndex).setPrev(cmd->prev);
        GET(cmd->pieceIndex).setNext(cmd->next);
        GET(cmd->pieceIndex).setTextStart(cmd->textStart);
        GET(cmd->pieceIndex).setTextEnd(cmd->textEnd);
        target.addCmd(&newCmd);        
      }
      break;
      default:
      break;
    }
  }
  
}

void CommandExecutor::undo(){
  act(undoBuffer, redoBuffer);
}

void CommandExecutor::redo(){
  act(redoBuffer, undoBuffer);
}

void CommandExecutor::submit(){

}

void TextBuffer::addCharacter(const Character& ch){
  vec.push_back(ch);
}

void TextBuffer::addCharacters(const Character* chs, Length length){
  vec.insert(vec.end(), chs, chs + length);
}

const Character* TextBuffer::data(Index index) const{
  return vec.data() + index;
}

Index TextBuffer::getEnd() const{
  return vec.size();
}

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
  Length sum = 0;

  if(position.textPosition < GET(position.pieceIndex).getTextEnd()){
      Index newPieceIndex = executor.createPiece();
      executor.setTextStartEnd(newPieceIndex, textStart, textEnd);

      Index leftIndex = executor.createPiece();
      executor.setPieceInfo(leftIndex,GET(position.pieceIndex).getPrev(),
                            newPieceIndex,
                            GET(position.pieceIndex).getTextStart(),
                            position.textPosition);
      
      Index rightIndex = executor.createPiece();
      executor.setPieceInfo(rightIndex, newPieceIndex,GET(position.pieceIndex).getNext(),
                            position.textPosition,                         
                            GET(position.pieceIndex).getTextEnd());

      executor.setPiecePrevNext(newPieceIndex, leftIndex, rightIndex);
      executor.setPiecePre(GET(position.pieceIndex).getNext(), rightIndex);
      executor.setPieceNext(GET(position.pieceIndex).getPrev(), leftIndex); 

      executor.removePiece(position.pieceIndex);

      lastEdit = newPieceIndex;
  }
  else if(position.textPosition == GET(position.pieceIndex).getTextEnd()){
    if(lastEdit == position.pieceIndex){
      //this is another method to decide,
      Index end = textBuffer.getEnd();
      assert(GET(position.pieceIndex).getTextEnd() == end);
      executor.setTextEnd(position.pieceIndex,GET(position.pieceIndex).getTextEnd()+ textEnd- textStart);
    }else{
      Index newPieceIndex = executor.createPiece();
      executor.setPieceInfo(newPieceIndex,position.pieceIndex,GET(position.pieceIndex).getNext(),
                            textStart, textEnd);
      
      executor.setPiecePre(GET(position.pieceIndex).getNext(), newPieceIndex);
      executor.setPieceNext(position.pieceIndex, newPieceIndex);
    }
  }
  else{
    assert(false);
  }


}

void EditorCore::insertLineBreak(LineNo line, Index column){
  Position pos = getPiecePosition(line,column);

  Index ind = executor.insertEmptyLine(line + 1);

  Index textStart = textBuffer.getEnd();
  textBuffer.addCharacter(Character('\n'));
  Index textEnd = textBuffer.getEnd();

  //has linebreak line
  if(GET(pos.headIndex).getTextEnd()-GET(pos.headIndex).getTextStart() !=0){
    executor.setTextStartEnd(gapBuffer[ind],textStart,textEnd);
  }else{
    //give old line a linebreak
    executor.setTextStartEnd(pos.headIndex, textStart, textEnd);
  }
  if(pos.textPosition == GET(pos.pieceIndex).getTextEnd()){
    if(GET(pos.pieceIndex).getNext() != pos.headIndex){
      executor.setPiecePrevNext(gapBuffer[ind],pieceTable->get(pos.headIndex).getPrev(), GET(pos.pieceIndex).getNext());
      executor.setPiecePre(GET(pos.pieceIndex).getNext(), gapBuffer[ind]);
      executor.setPieceNext(pieceTable->prevIndex(pos.headIndex),gapBuffer[ind]);
      executor.setPiecePre(pos.headIndex, pos.pieceIndex);
      executor.setPieceNext(pos.pieceIndex, pos.headIndex);
    }
  }else if(pos.textPosition < GET(pos.pieceIndex).getTextEnd()){
    Index left = executor.createPiece();
    executor.setPieceInfo(left,pieceTable->prevIndex(pos.pieceIndex),
                          pos.headIndex,
                          GET(pos.pieceIndex).getTextStart(),
                          pos.textPosition);
    
    Index right = executor.createPiece();
    executor.setPieceInfo(right,gapBuffer[ind],GET(pos.pieceIndex).getNext(),
                          pos.textPosition,GET(pos.pieceIndex).getTextEnd());

    executor.setPieceNext(pieceTable->prevIndex(pos.pieceIndex),left);
    executor.setPiecePre(pos.headIndex, left);
    executor.setPiecePrevNext(gapBuffer[ind],pieceTable->prevIndex(pos.headIndex),right);
    executor.setPiecePre(GET(pos.pieceIndex).getNext(), right);
    executor.setPieceNext(pieceTable->prevIndex(pos.headIndex), gapBuffer[ind]);
    executor.removePiece(pos.pieceIndex);
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

void EditorCore::appendTextU(const String& text){
  executor.beginCmd();
  appendText(text);
  executor.endCmd();
}

void EditorCore::appendText(const String& text){
  Index ind = gapBuffer.size();
  //last line is always an line without breakline
  Length len = getLineLength(ind - 1);
  insertText(ind - 1, len, text);
}

void EditorCore::insertTextU(LineNo line, Index column, const String& text){
  executor.beginCmd();
  insertText(line,column, text);
  executor.endCmd();
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

void EditorCore::removeTextU(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd){
  executor.beginCmd();
  removeText(lineStart,columnStart,lineLast,columnEnd);
  executor.endCmd();
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

#include <iostream>
int main(int argc, int** argv){
  CmdType type = CmdType::InLineInsert;
  if(type > CmdType::End){
    printf("abc\n");
  }

  GapBuffer<Index> gapBuffer;
  Index ind0 = gapBuffer.insert(0);
  gapBuffer[ind0] = 11;
  Index ind1 = gapBuffer.insert(1);
  gapBuffer[ind1] = 21;
  Index res1 = *gapBuffer.get(0);
  Index res2 = *gapBuffer.get(1);

  CmdBuffer cmdBuffer;
  EndCmd cmd1;
  cmdBuffer.addCmd(&cmd1);
  RemoveLineBreakCmd cmd2;
  cmdBuffer.addCmd(&cmd2);
  TextStartChangeCmd cmd3;
  cmdBuffer.addCmd(&cmd3);
  CmdHead* c1 = cmdBuffer.removeCmd();
  CmdHead* c2 = cmdBuffer.removeCmd();
  CmdHead* c3 = cmdBuffer.removeCmd();

  EditorCore core;
  String text = "abcd\n123";
  core.appendTextU(text);
  Length lineCount = core.getLineCount();
  for(int i=0; i<lineCount;++i){
    std::cout << core.getLineText(i);
  }

  std::cout <<"\n\n";
  core.undo();
  lineCount = core.getLineCount();
  std::cout << core.getContent();

  std::cout <<"\n\n";
  core.redo();
  lineCount = core.getLineCount();
  std::cout<<core.getContent();
  
  return 0;
}