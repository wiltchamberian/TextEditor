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

using Addr = Length;

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
    /////TODO this may cause issue!
    //assert(vec[ind].textEnd == 0);
    //assert(vec[ind].textStart == 0);
    vec[ind].setTextStart(0);
    vec[ind].setTextEnd(0);
    
    return ind;
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
    SparseSet<Piece>::remove(node);
  }
};


#endif