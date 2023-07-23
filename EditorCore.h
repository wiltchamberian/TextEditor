#ifndef __EDITOR_CORE_H
#define __EDITOR_CORE_H

#include "Define.h"
#include "TextBuffer.h"
#include "GapBuffer.h"
#include "CmdBuffer.h"
#include "CmdExecutor.h"

class Position {
public:
  Index line;
  Index column;
  //Index* head =  nullptr;
  Index headIndex; 
  Index pieceIndex;
  //accurate index in textBuf
  Index textPosition = 0;
};

class EditorCore{
public:
  EditorCore();
  ~EditorCore();
  Length getLineLength(LineNo line) const; 
  void getLineText(LineNo line, Character* output, Length& length) const;
  String getLineText(LineNo line) const;
  Length getLineCount() const;
  String getContent() const;
  //support undo-redo
  inline void appendTextU(const String& text){
    executor.beginCmd();
    _appendText(text);
    executor.endCmd();
  }
  
  inline void insertTextU(LineNo line, Index column, const String& text){
    executor.beginCmd();
    _insertText(line,column, text);
    executor.endCmd();
  }
  
  inline void removeTextU(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd){
    executor.beginCmd();
    _removeText(lineStart,columnStart,lineLast,columnEnd);
    executor.endCmd();
  }
  
  void undo();
  void redo();
private:
  void _appendText(const String& text);
  void _insertText(LineNo line, Index column, const String& text);
  void _removeText(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd);

  Position getPiecePosition(LineNo id, Index column);
  //insert a linebreak
  void insertLineBreak(LineNo line, Index column);
  void removeLineBreak(LineNo id);

  //insert text (with linebreaks in it or not)
  void insertText(LineNo line, Index column, const Character* chs, Length length);
  void insertInLine(LineNo line, Index column, const Character* chs, Length length);
  void insertInLine(const Position& position, const Character* chs, Length length);
  void removeInLine(LineNo line, Index columnStart, Index columnEnd);
  GapBuffer<Index> gapBuffer;
  PieceTable* pieceTable = nullptr;
  CommandExecutor executor;
  TextBuffer textBuffer;

  /*****state variables*****/
  //the index of lastEdited piece
  Index lastEdit = -1;
};


#endif