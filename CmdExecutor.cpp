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

  pieceTable->erase(cmd.pieceIndex);
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
  pieceTable->erase(GET(head).getPrev());
  pieceTable->erase(GET(head).getNext());
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
  pieceTable->remove(newHead);
  gapBuffer->remove(newHead);
}

void CommandExecutor::removePieceSequenceRedo(const RemovePieceSequenceRedoCmd& cmd){
  RemovePieceSequenceUndoCmd* newCmd = (RemovePieceSequenceUndoCmd*)undoBuffer.addCmd(CmdType::RemovePieceSequenceUndo,sizeof(RemovePieceSequenceUndoCmd));
  newCmd->pieceStart = cmd.pieceStart;
  newCmd->pieceLast = cmd.pieceLast;
  newCmd->piecePreStartTextEnd = GET(GET(cmd.pieceStart).getPrev()).getTextEnd();
  newCmd->pieceSucLastTextStart = GET(GET(cmd.pieceLast).getNext()).getTextStart();
  
  pieceTable->setTextEnd(GET(cmd.pieceStart).getPrev(),cmd.piecePreStartTextEnd);
  pieceTable->setTextStart(GET(cmd.pieceLast).getNext(), cmd.pieceSucLastTextStart);
  pieceTable->unLink(cmd.pieceStart, cmd.pieceLast);
}

void CommandExecutor::removePieceSequenceUndo(const RemovePieceSequenceUndoCmd& cmd){
  RemovePieceSequenceRedoCmd* newCmd = (RemovePieceSequenceRedoCmd*)redoBuffer.addCmd(CmdType::RemovePieceSequenceRedo,sizeof(RemovePieceSequenceRedoCmd));
  newCmd->pieceStart = cmd.pieceStart;
  newCmd->pieceLast = cmd.pieceLast;
  newCmd->piecePreStartTextEnd = GET(GET(cmd.pieceStart).getPrev()).getTextEnd();
  newCmd->pieceSucLastTextStart = GET(GET(cmd.pieceLast).getNext()).getTextStart();

  pieceTable->setTextEnd(GET(cmd.pieceStart).getPrev(),cmd.piecePreStartTextEnd);
  pieceTable->setTextStart(GET(cmd.pieceLast).getNext(), cmd.pieceSucLastTextStart);
  pieceTable->linkTogether(GET(cmd.pieceStart).getPrev(), cmd.pieceStart);
  pieceTable->linkTogether(cmd.pieceLast, GET(cmd.pieceLast).getNext());
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
