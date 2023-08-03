/*********************************************************************************
 * Author: ACE
 * Description: a kind of pieceTable, use double linked list and sparse set.
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __PIECE_TABLE_H
#define __PIECE_TABLE_H

#include "Define.h"

template<class _T>
using SVector = std::vector<_T>;

class Piece{
public:
  friend class PieceTable;
  inline void setPrev(Index ind){pre = ind;}
  inline void setNext(Index ind){next = ind;}
  inline Index getPrev() const { return pre;}
  inline Index getNext() const { return next;}
  inline Index getTextStart() const { return textStart;}
  inline Index getTextEnd() const { return textEnd; }
  inline void setTextStart(Index ind){textStart = ind;}
  inline void setTextEnd(Index ind){textEnd = ind;}
  inline void setText(Index start, Index end){textStart = start; textEnd = end;}
private:
  Index pre = -1;
  Index next = -1;
  Index textStart = 0;
  Index textEnd = 0;
};

//make it support undo redo, use atomic operation
class PieceTable
{
public:
  PieceTable(){
    vec.emplace_back();
    vec[0].setNext(0);
    vec[0].setPrev(0);
  }
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
  void setTextStart(Index node, Index textStart){
    vec[node].textStart = textStart;
  }

  void setTextEnd(Index node, Index textEnd){
    vec[node].textEnd = textEnd;
  }

  void setText(Index node, Index textStart, Index textEnd){
    vec[node].textStart = textStart;
    vec[node].textEnd = textEnd;
  }

  Index getTextStart(Index node) const{
    return vec[node].textStart;
  }

  Index getTextEnd(Index node) const{
    return vec[node].textEnd;
  }

  void insertAfter(Index newNode, Index rover){
    vec[newNode].next = vec[rover].next;
    vec[newNode].pre = rover;
    vec[vec[rover].next].pre = newNode;
    vec[rover].next = newNode;
  }

  void insertBefore(Index newNode, Index rover){
    vec[newNode].next = rover;
    vec[newNode].pre = vec[rover].pre;
    vec[vec[rover].pre].next = newNode;
    vec[rover].pre = newNode;
  }

  void splitBefore(Index node){
    vec[vec[node].pre].next = vec[node].pre;
    vec[node].pre = node;
  }

  void splitAfter(Index node){
    vec[vec[node].next].pre = vec[node].next;
    vec[node].next = node;
  }

  void linkTogether(Index left, Index right){
    vec[left].next = right;
    vec[right].pre = left;
  }

  //node's pointer is still efficient after unlink
  void unLink(Index node){
    vec[vec[node].pre].next = vec[node].next;
    vec[vec[node].next].pre = vec[node].pre;
  }

  //unlink a sequence in the list
  void unLink(Index nodeStart, Index nodeLast){
    vec[vec[nodeStart].pre].next = vec[nodeLast].next;
    vec[vec[nodeLast].next].pre  = vec[nodeStart].pre;
  }

  void erase(Index node){
    unLink(node);
    remove(node);
  }

  void erase(Index listHead, Index listTail){
    unLink(listHead,listTail);

    Index n = vec[0].getNext();
    vec[0].setNext(listHead);
    vec[listHead].setPrev(0);
    vec[listTail].setNext(n);
    vec[n].setPrev(listTail);

    return;
  }

  void undoErase(Index preHead, Index sucTail, Index tail){
    Index head = vec[0].getNext();
    Index right = vec[tail].getNext();
    vec[0].setNext(right);
    vec[right].setPrev(0);
    vec[preHead].setNext(head);
    vec[sucTail].setPrev(tail);
    vec[head].setPrev(preHead);
    vec[tail].setNext(sucTail);
    return;
  }

  void eraseCircularList(Index tail){
    Index head = vec[tail].getNext();
    erase(head, tail);
  }

  //undo erase a entire circular list
  void undoEraseCircularList(Index listTail){
    Index head = vec[0].getNext();
    Index right = vec[listTail].getNext();
    vec[0].setNext(right);
    vec[right].setPrev(0);
    vec[head].setPrev(listTail);
    vec[listTail].setNext(head);
    return;
  }

  Index add(const Piece& a) {
    Index addr;
    if(vec[0].getNext() != 0){
      addr = vec[0].getNext();
      vec[addr] = a;
      unLink(vec[0].getNext());
      return addr;
    }
    vec.push_back(a);
    return vec.size()-1;
  }
  
  Index add(){
    Index ind = 0;
    if(vec[0].getNext() != 0){
      ind = vec[0].getNext();
      unLink(ind);
      vec[ind].setPrev(ind);
      vec[ind].setNext(ind);
      vec[ind].setTextStart(0);
      vec[ind].setTextEnd(0);
      return ind;
    }
    vec.emplace_back();
    ind =  vec.size() - 1;

    vec[ind].setPrev(ind);
    vec[ind].setNext(ind);
    assert(vec[ind].getTextStart()==0 && vec[ind].getTextEnd() ==0);
    
    return ind;
  }

  void undoAdd(Index ind){
    remove(ind);
  }

  Piece& get(Index addr) {
    return vec[addr];
  }

  Index getAddress(Piece& d){
    return (&d) - vec.data();
  }
  
  //need to travel list, so complexity is O(n), if not necessary, don't use it
  size_t size() const{
    return vec.size() - getFreeListLength() - 1;
  }
  
 void remove(Index addr) {
    vec[vec[0].getNext()].setPrev(addr);
    vec[addr].setNext(vec[0].getNext());
    vec[addr].setPrev(0);
    vec[0].setNext(addr);
    return;
  }

  Index undoRemove(){
    return add();
  }
private:
  bool hasFreeNode() const{
    return vec[0].getNext() != 0;
  }
  bool noFreeNode() const{
    return vec[0].getNext() == 0;
  }
  Length getFreeListLength() const{
    Length l = 0;
    Index rover = vec[0].getNext();
    while(rover != 0){
      l += 1;
      rover = vec[rover].getNext();
    }
    return l;
  }
  //the first element is used as freeList token
  SVector<Piece> vec;
  
};


#endif