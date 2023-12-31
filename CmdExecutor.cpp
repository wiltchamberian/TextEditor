#include "CmdExecutor.h"

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
  Index ind = pieceTable->addAndClearText();
  (*gapBuffer)[index] = ind;

  RemoveEmptyLineCmd cmd;
  cmd.lineNo = lineNo;
  cmd.pieceIndex = ind;
  addCmd(&cmd);
  return index;
}

void CommandExecutor::removeLineBreak(Index lineNo){


}

void CommandExecutor::insertInLineInPieceRedo(const InsertInLineInPieceRedoCmd& cmd){
  InsertInLineInPieceUndoCmd* newCmd = (InsertInLineInPieceUndoCmd*)undoBuffer.addCmd(CmdType::InsertInLineInPieceUndo, sizeof(InsertInLineInPieceUndoCmd));
  newCmd->ori = cmd.ori;
  newCmd->oriTextStart = GET(cmd.ori).getTextStart();
  newCmd->oriTextEnd = GET(cmd.ori).getTextEnd();
  
  Index right = pieceTable->add();
  Index left = pieceTable->add();

  pieceTable->setText(left, cmd.leftTextStart,cmd.leftTextEnd);
  pieceTable->setText(right, cmd.rightTextStart, cmd.rightTextEnd);
  
  pieceTable->insertBefore(left, cmd.ori);
  pieceTable->insertAfter(right, cmd.ori);

  pieceTable->setText(cmd.ori,cmd.oriTextStart, cmd.oriTextEnd);

}

void CommandExecutor::insertInLineInPieceUndo(const InsertInLineInPieceUndoCmd& cmd){
  InsertInLineInPieceRedoCmd* newCmd = (InsertInLineInPieceRedoCmd*)redoBuffer.addCmd(CmdType::InsertInLineInPieceRedo, sizeof(InsertInLineInPieceRedoCmd));
  newCmd->ori = cmd.ori;
  newCmd->oriTextStart = GET(cmd.ori).getTextStart();
  newCmd->oriTextEnd = GET(cmd.ori).getTextEnd();
  newCmd->leftTextStart = GET(GET(cmd.ori).getPrev()).getTextStart();
  newCmd->leftTextEnd = GET(GET(cmd.ori).getPrev()).getTextEnd();
  newCmd->rightTextStart = GET(GET(cmd.ori).getNext()).getTextStart();
  newCmd->rightTextEnd = GET(GET(cmd.ori).getNext()).getTextEnd();
  
  pieceTable->erase(GET(cmd.ori).getPrev());
  pieceTable->erase(GET(cmd.ori).getNext());

  GET(cmd.ori).setTextStart(cmd.oriTextStart);
  GET(cmd.ori).setTextEnd(cmd.oriTextEnd);
}

void CommandExecutor::insertInLineBetweenPieceRedo(const InsertInLineBetweenPieceRedoCmd& cmd){
  InsertInLineBetweenPieceUndoCmd* newCmd = (InsertInLineBetweenPieceUndoCmd*)undoBuffer.addCmd(CmdType::InsertInLineBetweenPieceUndo,sizeof(InsertInLineBetweenPieceUndoCmd));

  Index newPieceIndex = pieceTable->add();
  newCmd->pieceIndex = newPieceIndex;

  pieceTable->setText(newPieceIndex, cmd.textStart, cmd.textEnd);
  pieceTable->insertAfter(newPieceIndex, cmd.pieceBefore);
}

void CommandExecutor::insertInLineBetweenPieceUndo(const InsertInLineBetweenPieceUndoCmd& cmd){
  InsertInLineBetweenPieceRedoCmd* newCmd = (InsertInLineBetweenPieceRedoCmd*)redoBuffer.addCmd(CmdType::InsertInLineBetweenPieceRedo,sizeof(InsertInLineBetweenPieceRedoCmd));
  newCmd->pieceBefore = GET(cmd.pieceIndex).getPrev();
  newCmd->textStart = GET(cmd.pieceIndex).getTextStart();
  newCmd->textEnd = GET(cmd.pieceIndex).getTextEnd();

  pieceTable->unLink(cmd.pieceIndex);
  pieceTable->undoAdd(cmd.pieceIndex);
}

void CommandExecutor::insertBreakInPieceRedo(const InsertBreakInPieceRedoCmd& cmd){
  InsertBreakInPieceUndoCmd* newCmd = (InsertBreakInPieceUndoCmd*)undoBuffer.addCmd(CmdType::InsertBreakInPieceUndo, sizeof(InsertBreakInPieceUndoCmd));
  newCmd->line = cmd.lineNo + 1;

  Index left =  pieceTable->add();
  pieceTable->setText(left, GET(cmd.pieceIndex).getTextStart(), cmd.textSplitPoint);
  Index right = pieceTable->add();
  pieceTable->setText(right, cmd.textSplitPoint, GET(cmd.pieceIndex).getTextEnd());

  Index* ind = gapBuffer->get(cmd.lineNo);
  Index head = *ind;
  Index tail = GET(head).getPrev();
  gapBuffer->insert(cmd.lineNo + 1);
  *(gapBuffer->get(cmd.lineNo+1)) = cmd.pieceIndex;

  pieceTable->insertBefore(left, cmd.pieceIndex);
  pieceTable->insertAfter(right, cmd.pieceIndex);
  pieceTable->linkTogether(tail, cmd.pieceIndex);
  pieceTable->linkTogether(left, head);

  pieceTable->setText(cmd.pieceIndex, cmd.textStart, cmd.textEnd);
}

void CommandExecutor::insertBreakInPieceUndo(const InsertBreakInPieceUndoCmd& cmd){
  InsertBreakInPieceRedoCmd* newCmd = (InsertBreakInPieceRedoCmd*)redoBuffer.addCmd(CmdType::InsertBreakInPieceRedo, sizeof(InsertBreakInPieceRedoCmd));
  
  Index* ind = gapBuffer->get(cmd.line);
  Index headup = *gapBuffer->get(cmd.line -1);
  Index head = *ind;

  newCmd->lineNo = cmd.line - 1;
  newCmd->textStart = GET(head).getTextStart();
  newCmd->textEnd = GET(head).getTextEnd();
  newCmd->textSplitPoint = GET(GET(headup).getPrev()).getTextEnd();
  newCmd->pieceIndex = head;

  gapBuffer->remove(cmd.line);

  pieceTable->setText(head, GET(GET(headup).getPrev()).getTextStart(),
                      GET(GET(head).getNext()).getTextEnd());
  pieceTable->linkTogether(GET(headup).getPrev(), head);
  pieceTable->erase(GET(head).getNext());
  pieceTable->erase(GET(head).getPrev());
}

void CommandExecutor::insertBreakBetweenPieceRedo(const InsertBreakBetweenPieceRedoCmd& cmd){
  InsertBreakBetweenPieceUndoCmd* newCmd = (InsertBreakBetweenPieceUndoCmd*)undoBuffer.addCmd(CmdType::InsertBreakBetweenPieceUndo, sizeof(InsertBreakBetweenPieceUndoCmd));
  newCmd->line = cmd.line + 1;

  Index oldHead = *(gapBuffer->get(cmd.line));
  Index newLine = gapBuffer->insert(cmd.line + 1);
  Index* ptr = gapBuffer->get(newLine);
  Index newHead = pieceTable->add();

  pieceTable->setText(newHead, cmd.textStart, cmd.textEnd);

  *ptr = newHead;
  Index n = GET(cmd.pieceIndex).getNext();
  Index tail = GET(oldHead).getPrev();

  pieceTable->linkTogether(cmd.pieceIndex, oldHead);
  pieceTable->linkTogether(newHead, n);
  pieceTable->linkTogether(tail, newHead);
}

void CommandExecutor::insertBreakBetweenPieceUndo(const InsertBreakBetweenPieceUndoCmd& cmd){
  InsertBreakBetweenPieceRedoCmd* newCmd = (InsertBreakBetweenPieceRedoCmd*)redoBuffer.addCmd(CmdType::InsertBreakBetweenPieceRedo, sizeof(InsertBreakBetweenPieceRedoCmd));
  newCmd->line  = cmd.line - 1;
  
  Index newHead =  *gapBuffer->get(cmd.line);
  Index oldHead =  *gapBuffer->get(cmd.line - 1);

  newCmd->pieceIndex = GET(oldHead).getPrev();
  newCmd->textStart = GET(newHead).getTextStart();
  newCmd->textEnd = GET(newHead).getTextEnd();

  pieceTable->linkTogether(GET(oldHead).getPrev(),GET(newHead).getNext());
  pieceTable->linkTogether(GET(newHead).getPrev(),oldHead);
  pieceTable->remove(newHead);
  
  gapBuffer->remove(newHead);
}

void CommandExecutor::removePieceSequenceRedo(const RemovePieceSequenceRedoCmd& cmd){
  RemovePieceSequenceUndoCmd* newCmd = (RemovePieceSequenceUndoCmd*)undoBuffer.addCmd(CmdType::RemovePieceSequenceUndo,sizeof(RemovePieceSequenceUndoCmd));
  newCmd->pieceStart = cmd.pieceStart;
  newCmd->pieceLast = cmd.pieceLast;
  newCmd->piecePreStart = GET(cmd.pieceStart).getPrev();
  newCmd->pieceSucLast = GET(cmd.pieceLast).getNext();
  newCmd->piecePreStartTextEnd = GET(newCmd->piecePreStart).getTextEnd();
  newCmd->pieceSucLastTextStart = GET(newCmd->pieceSucLast).getTextStart();
  
  pieceTable->setTextEnd(GET(cmd.pieceStart).getPrev(),cmd.piecePreStartTextEnd);
  pieceTable->setTextStart(GET(cmd.pieceLast).getNext(), cmd.pieceSucLastTextStart);
  
  pieceTable->erase(cmd.pieceStart, cmd.pieceLast);
}

void CommandExecutor::removePieceSequenceUndo(const RemovePieceSequenceUndoCmd& cmd){
  RemovePieceSequenceRedoCmd* newCmd = (RemovePieceSequenceRedoCmd*)redoBuffer.addCmd(CmdType::RemovePieceSequenceRedo,sizeof(RemovePieceSequenceRedoCmd));
  newCmd->pieceStart = cmd.pieceStart;
  newCmd->pieceLast = cmd.pieceLast;
  newCmd->piecePreStartTextEnd = GET(GET(cmd.pieceStart).getPrev()).getTextEnd();
  newCmd->pieceSucLastTextStart = GET(GET(cmd.pieceLast).getNext()).getTextStart();

  pieceTable->setTextEnd(GET(cmd.pieceStart).getPrev(),cmd.piecePreStartTextEnd);
  pieceTable->setTextStart(GET(cmd.pieceLast).getNext(), cmd.pieceSucLastTextStart);
  
  pieceTable->undoErase(cmd.piecePreStart, cmd.pieceSucLast, cmd.pieceLast);
}

void CommandExecutor::removeLineBreakRedo(const RemoveLineBreakRedoCmd& cmd){
  Length len = gapBuffer->size();
  if(cmd.line >= len-1){
    assert(false);
  }

  Index* ptr = gapBuffer->get(cmd.line);
  Index* ptr2 = gapBuffer->get(cmd.line+1);

  RemoveLineBreakUndoCmd* newCmd = (RemoveLineBreakUndoCmd*)undoBuffer.addCmd(CmdType::RemoveLineBreakUndo, sizeof(RemoveLineBreakUndoCmd));
  newCmd->line = cmd.line + 1;
  newCmd->pieceIndex = GET(*ptr2).getNext();

  pieceTable->linkTogether(GET(*ptr).getPrev(), GET(*ptr2).getNext());
  pieceTable->linkTogether(*ptr2,GET(*ptr).getNext());
  pieceTable->remove(*ptr);
  *ptr = *ptr2;
  gapBuffer->remove(cmd.line + 1);
  
  return;
}

void CommandExecutor::removeLineBreakUndo(const RemoveLineBreakUndoCmd& cmd){
  RemoveLineBreakRedoCmd* newCmd = (RemoveLineBreakRedoCmd*)redoBuffer.addCmd(CmdType::RemoveLineBreakRedo, sizeof(RemoveLineBreakRedoCmd));
  newCmd->line = cmd.line - 1;
  
  gapBuffer->insert(cmd.line);
  Index* ptr = gapBuffer->get(cmd.line);
  Index* ptr_pre = gapBuffer->get(cmd.line - 1);

  Index index = pieceTable->add();
  pieceTable->linkTogether(GET(cmd.pieceIndex).getPrev(),index);
  pieceTable->linkTogether(index, GET(*ptr_pre).getNext());
  pieceTable->linkTogether(*ptr_pre, cmd.pieceIndex);
  *ptr = *ptr_pre;
  *ptr_pre = index;

  return;
}

void CommandExecutor::removeOneLineWithoutLineBreakRedo(const RemoveOneLineWithoutLineBreakRedoCmd& cmd){
  RemoveOneLineWithoutLineBreakUndoCmd* newCmd = (RemoveOneLineWithoutLineBreakUndoCmd*)undoBuffer.addCmd(CmdType::RemoveOneLineWithoutLineBreakUndo, sizeof(RemoveOneLineWithoutLineBreakUndoCmd));
  newCmd->tail = cmd.tail;
  newCmd->last = GET(cmd.tail).getPrev();

  pieceTable->erase(GET(cmd.tail).getNext(),newCmd->last);
}

void CommandExecutor::removeOneLineWithoutLineBreakUndo(const RemoveOneLineWithoutLineBreakUndoCmd& cmd){
  RemoveOneLineWithoutLineBreakRedoCmd* newCmd = (RemoveOneLineWithoutLineBreakRedoCmd*)redoBuffer.addCmd(CmdType::RemoveOneLineWithoutLineBreakRedo, sizeof(RemoveOneLineWithoutLineBreakRedoCmd));
  newCmd->tail = cmd.tail;
  newCmd->last = cmd.last;

  pieceTable->undoErase(cmd.tail, cmd.tail, cmd.last);
}

void CommandExecutor::removeLineRedo(const RemoveLineRedoCmd& cmd){
  Index tail = *(gapBuffer->get(cmd.line));
  
  RemoveLineUndoCmd* newCmd = (RemoveLineUndoCmd*)undoBuffer.addCmd(CmdType::RemoveLineUndo,sizeof(RemoveLineUndoCmd));
  newCmd->line = cmd.line;
  newCmd->pieceTail = tail;

  gapBuffer->remove(cmd.line);
  pieceTable->eraseCircularList(tail);
}

void CommandExecutor::removeLineUndo(const RemoveLineUndoCmd& cmd){
  RemoveLineRedoCmd* newCmd = (RemoveLineRedoCmd*)redoBuffer.addCmd(CmdType::RemoveLineRedo, sizeof(RemoveLineRedoCmd));
  newCmd->line = cmd.line;

  gapBuffer->insert(cmd.line);
  pieceTable->undoEraseCircularList(cmd.pieceTail);
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
      case CmdType::InsertEmptyLine:{
        InsertEmptyLineCmd* cmd = (InsertEmptyLineCmd*)(head);
        Index ind = pieceTable->addAndClearText();
        Index pt = gapBuffer->insert(cmd->lineNo);
        (*gapBuffer)[pt] = ind;
        RemoveEmptyLineCmd newCmd;
        newCmd.lineNo = cmd->lineNo;
        newCmd.pieceIndex = ind;
        target.addCmd(&newCmd);
      }
      break;
      case CmdType::RemoveEmptyLine:{
        RemoveEmptyLineCmd* cmd = (RemoveEmptyLineCmd*)(head);
        pieceTable->remove(cmd->pieceIndex);
        gapBuffer->remove(cmd->lineNo);
        InsertEmptyLineCmd newCmd;
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
      case CmdType::RemoveLineRedo:{
        RemoveLineRedoCmd* cmd = (RemoveLineRedoCmd*)head;
        removeLineRedo(*cmd);
      }
      break;
      case CmdType::RemoveLineUndo:{
        RemoveLineUndoCmd* cmd = (RemoveLineUndoCmd*)head;
        removeLineUndo(*cmd);
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
      case CmdType::InsertInLineInPieceRedo:{
        InsertInLineInPieceRedoCmd* cmd= (InsertInLineInPieceRedoCmd*)head;
        insertInLineInPieceRedo(*cmd);
      }
      break;
      case CmdType::InsertInLineInPieceUndo:{
        InsertInLineInPieceUndoCmd* cmd = (InsertInLineInPieceUndoCmd*)head;
        insertInLineInPieceUndo(*cmd);
      }
      break;
      case CmdType::InsertInLineBetweenPieceRedo:{
        const InsertInLineBetweenPieceRedoCmd* cmd = (InsertInLineBetweenPieceRedoCmd*)head;
        insertInLineBetweenPieceRedo(*cmd);
      }
      break;
      case CmdType::InsertInLineBetweenPieceUndo:{
        const InsertInLineBetweenPieceUndoCmd* cmd = (InsertInLineBetweenPieceUndoCmd*)head;
        insertInLineBetweenPieceUndo(*cmd);
      }
      break;
      case CmdType::InsertBreakInPieceRedo:{
        const InsertBreakInPieceRedoCmd* cmd = (InsertBreakInPieceRedoCmd*)head;
        insertBreakInPieceRedo(*cmd);
      }
      break;
      case CmdType::InsertBreakInPieceUndo:{
        const InsertBreakInPieceUndoCmd* cmd = (InsertBreakInPieceUndoCmd*)head;
        insertBreakInPieceUndo(*cmd);
      }
      break;
      case CmdType::InsertBreakBetweenPieceRedo:{
        const InsertBreakBetweenPieceRedoCmd* cmd = (InsertBreakBetweenPieceRedoCmd*)head;
        insertBreakBetweenPieceRedo(*cmd);
      }
      break;
      case CmdType::InsertBreakBetweenPieceUndo:{
        const InsertBreakBetweenPieceUndoCmd* cmd = (InsertBreakBetweenPieceUndoCmd*)head;
        insertBreakBetweenPieceUndo(*cmd);
      }
      break;
      case CmdType::RemovePieceSequenceRedo:{
        const RemovePieceSequenceRedoCmd* cmd = (RemovePieceSequenceRedoCmd*)head;
        removePieceSequenceRedo(*cmd);
      }
      break;
      case CmdType::RemovePieceSequenceUndo:{
        const RemovePieceSequenceUndoCmd* cmd = (RemovePieceSequenceUndoCmd*)head;
        removePieceSequenceUndo(*cmd);
      }
      break;
      case CmdType::RemoveLineBreakRedo:{
        const RemoveLineBreakRedoCmd* cmd = (RemoveLineBreakRedoCmd*)head;
        removeLineBreakRedo(*cmd);
      }
      break;
      case CmdType::RemoveLineBreakUndo:{
        const RemoveLineBreakUndoCmd* cmd = (RemoveLineBreakUndoCmd*)head;
        removeLineBreakUndo(*cmd);
      }
      break;
      case CmdType::RemoveOneLineWithoutLineBreakRedo:{
        const RemoveOneLineWithoutLineBreakRedoCmd* cmd = (RemoveOneLineWithoutLineBreakRedoCmd*)head;
        removeOneLineWithoutLineBreakRedo(*cmd);
      }
      break;
      case CmdType::RemoveOneLineWithoutLineBreakUndo:{
        const RemoveOneLineWithoutLineBreakUndoCmd* cmd = (RemoveOneLineWithoutLineBreakUndoCmd*)head;
        removeOneLineWithoutLineBreakUndo(*cmd);
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
