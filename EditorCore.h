#ifndef __EDITOR_CORE_H
#define __EDITOR_CORE_H

#include <vector>
#include <string>
#include <cassert>
#include <array>

using String = std::string;

#define GET(a) pieceTable->get(a)
using LineNo = size_t;
using Index = size_t;
using Length = size_t;

enum class CmdType : uint16_t{
  Append,
  InsertLine,
  RemoveLine,
  LineCmd,
  InLineRemove,
  InLineInsert,
  InsertLineBreak,
  RemoveLineBreak,
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
};

class Character{
public:
  Character(){}
  Character(char c):ch(c){}
  char ch;
};

class Piece{
public:
  friend class PieceTable;
  inline void setPrev(Index ind){pre = ind;}
  inline void setNext(Index ind){next = ind;}
  inline Index getPrev(){ return pre;}
  inline Index getNext(){ return next;}
  inline Index getTextStart() { return textStart;}
  inline Index getTextEnd() { return textEnd; }
  inline void setTextStart(Index ind){textStart = ind;}
  inline void setTextEnd(Index ind){textEnd = ind;}
private:
  Index pre = -1;
  Index next = -1;
  Index textStart = 0;
  Index textEnd = 0;
};

struct CmdHead{
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

class CmdBuffer{
public:
  ~CmdBuffer();
  CmdHead* addCmd(CmdType type, Length length);
  CmdHead* addCmd(const CmdHead* cmd);
  inline CmdHead* addCmd(const void* cmd);
  CmdHead* removeCmd();
  bool empty() const;
private:
  uint8_t* chunk = nullptr;
  Length chunk_size = 0;
  Length used_size = 0;
  Length cmd_count = 0;
};

template<class _T>
using SVector = std::vector<_T>;

using Addr = size_t;

template<typename _T>
class SparseSet {
public:
  Addr add(){
    Addr addr;
    if (!freeList.empty()) {
      addr = freeList.back();
      freeList.pop_back();
      return addr;
    }
    vec.emplace_back();
    return vec.size() - 1;
  }

  Addr add(const _T& a) {
    Addr addr;
    if (!freeList.empty()) {
      addr = freeList.back();
      vec[addr] = a;
      freeList.pop_back();
      return addr;
    }
    vec.push_back(a);
    return vec.size() - 1;
  }

  void remove(Addr addr) {
    if(freeList.empty() && addr == vec.size()-1){
      vec.pop_back();
    }else{
      freeList.push_back(addr);
    }
    return;
  }

  _T& get(Addr addr) {
    return vec[addr];
  }

  Addr getAddress(_T& d){
    return (&d) - vec.data();
  }
  size_t size() const{
    return vec.size()-freeList.size();
  }
protected:
  SVector<_T> vec;
  SVector<Addr> freeList;
};

//make it support undo redo, use atomic operation
class PieceTable:public SparseSet<Piece>
{
public:
  void setNext(Index piece, Index next){
    vec[piece].next = next;
  }
  void setNext(Piece& piece, Piece& next){
    piece.next = getAddress(next);
  }
  void setPrev(Index piece, Index prev){
    vec[piece].pre = prev;
  }
  void setPrev(Piece& piece, Piece& prev){
    piece.pre = getAddress(prev);
  } 
  Index prevIndex(Index ind) const{
    return vec[ind].pre;
  }
  Index nextIndex(Index ind) const{
    return vec[ind].next;
  }
  Piece& getPrev(Index ind){
    return vec[vec[ind].pre];
  }
  Piece& getNext(Index ind){
    return vec[vec[ind].next];
  }
  Index add(){
    Index ind = SparseSet<Piece>::add();
    vec[ind].setPrev(ind);
    vec[ind].setNext(ind);
    return ind;
  }
};

template<typename _T>
class GapBuffer{
public:
  ~GapBuffer(){
    if(chunk != nullptr){
      delete[] chunk;
    }
    return;
  }
  void clear(){
    leftSize = 0;
    rightSize = 0;
    siz = 0;
    delete[] chunk;
    chunk = nullptr;
  }
  bool empty() const{
    return (leftSize + rightSize) == siz;
  }
  Length capacity() const{
    return siz;
  }
  Length size() const{
    return leftSize + rightSize;
  }
  Index insert(Index id){
    Index res = 0;
    if( id > leftSize + rightSize){
      id = leftSize + rightSize;
    }else if(id < 0){
      id =  0;
    }
    if(leftSize + rightSize == siz){
      Index newSize = (siz == 0)? 1: (siz * 2);
      _T* tmp = chunk;
      chunk = new _T[newSize];
      if(id <= leftSize){
        std::memcpy(chunk, tmp, id * sizeof(_T));
        if(leftSize - id >0){
          std::memcpy(chunk + newSize - rightSize - (leftSize - id), tmp +id, (leftSize - id)*sizeof(_T));
        }
        std::memcpy(chunk + newSize - rightSize, tmp + siz - rightSize, rightSize * sizeof(_T));
      }else if(id > leftSize){
        std::memcpy(chunk, tmp, leftSize * sizeof(_T));
        std::memcpy(chunk + leftSize ,tmp+siz-rightSize, (id- leftSize)*sizeof(_T) );
        std::memcpy(chunk + newSize - rightSize + id - leftSize, tmp+ siz-rightSize + id- leftSize, (leftSize + rightSize -id)*sizeof(_T)); 
      }
      rightSize = leftSize + rightSize - id;
      leftSize = id; 
      res = leftSize;
      leftSize += 1;
      siz = newSize;
      delete[] tmp;
    }else if(leftSize + rightSize < siz){
      if(id <= leftSize){
        std::memmove(chunk +siz - rightSize - (leftSize - id), chunk + id, (leftSize - id)*sizeof(_T));
        rightSize = rightSize + leftSize - id;
        leftSize = id;
        res = leftSize;
        leftSize += 1;
        siz += 1;
      }else{
        std::memmove(chunk + leftSize,chunk + siz -rightSize, (id-leftSize)*sizeof(_T));
        rightSize = rightSize + leftSize - id;
        leftSize = id;
        res = leftSize;
        leftSize += 1;
        siz += 1;
      }
    }else{
      assert(false);
    }
    return res;
  }

  _T& operator[] (Index id){
    if(id < leftSize){
      return *(chunk+id);
    }
    return *(chunk + siz- rightSize + (id - leftSize));
  }
  _T* get(Index id) const{
    if(id >= leftSize + rightSize || id < 0){
      return nullptr;
    }
    if(id < leftSize){
      return chunk+id;
    }
    return chunk + siz- rightSize + (id - leftSize);
  }

  void remove(Index id){
    if(id <0 || id >= leftSize + rightSize){
      return;
    }
    if(id < leftSize){
      std::memmove(chunk + siz - rightSize -(leftSize - id - 1), chunk+id+1, (leftSize - id -1)*sizeof(_T));
      rightSize = leftSize + rightSize - id - 1;
      leftSize = id;
      siz -= 1;
    }else{
      std::memcpy(chunk + leftSize, chunk+ siz- rightSize, (id-leftSize)*sizeof(_T));
      rightSize = leftSize + rightSize - id - 1;
      leftSize = id;
      siz -= 1;
    }
  }
private:
  _T* chunk = nullptr;
  Length siz = 0;
  Length leftSize = 0;
  Length rightSize = 0;
};

class TextBuffer{
public:
  void addCharacter(const Character& ch);
  void addCharacters(const Character* chs, Length length);
  const Character* data(Index index) const;
  Index getEnd() const;
private:
  std::vector<Character> vec;
};

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
  inline void appendTextU(const String& text);
  
  inline void insertTextU(LineNo line, Index column, const String& text);
  
  inline void removeTextU(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd);
  
  void undo();
  void redo();
private:
  void appendText(const String& text);
  void insertText(LineNo line, Index column, const String& text);
  void removeText(LineNo lineStart,Index columnStart, LineNo lineLast, Index columnEnd);

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