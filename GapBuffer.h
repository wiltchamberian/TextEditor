/*********************************************************************************
 * Author: ACE
 * Description: A template implementation of GapBuffer, used to save lines
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __GAP_BUFFER_H
#define __GAP_BUFFER_H

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
        if(id > 0){
          std::memcpy(chunk, tmp, id * sizeof(_T));
        }
        if(leftSize - id >0){
          std::memcpy(chunk + (newSize - rightSize - (leftSize - id)), tmp +id, (leftSize - id)*sizeof(_T));
        }
        if(rightSize >0){
          std::memcpy(chunk + newSize - rightSize, tmp + siz - rightSize, rightSize * sizeof(_T));
        }
        
      }else if(id > leftSize){
        if(leftSize > 0){
          std::memcpy(chunk, tmp, leftSize * sizeof(_T));
        }
        if(id > leftSize){
          std::memcpy(chunk + leftSize ,tmp+siz-rightSize, (id- leftSize)*sizeof(_T) );
        }
        if(leftSize + rightSize -id > 0){
          std::memcpy(chunk + newSize - rightSize + id - leftSize, tmp+ siz-rightSize + id- leftSize, (leftSize + rightSize -id)*sizeof(_T)); 
        }
      }
      rightSize = leftSize + rightSize - id;
      leftSize = id; 
      res = leftSize;
      leftSize += 1;
      siz = newSize;
      delete[] tmp;
    }else if(leftSize + rightSize < siz){
      if(id <= leftSize){
        if(leftSize - id >0){
          std::memmove(chunk +siz - rightSize - (leftSize - id), chunk + id, (leftSize - id)*sizeof(_T));
        }
        rightSize = rightSize + leftSize - id;
        leftSize = id;
        res = leftSize;
        leftSize += 1;
      }else{
        if(id - leftSize > 0){
          std::memmove(chunk + leftSize,chunk + siz -rightSize, (id-leftSize)*sizeof(_T));
        }
        rightSize = rightSize + leftSize - id;
        leftSize = id;
        res = leftSize;
        leftSize += 1;
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
      if(leftSize - id - 1 >0){
        std::memmove(chunk + siz - rightSize -(leftSize - id - 1), chunk+id+1, (leftSize - id -1)*sizeof(_T));
      }
      rightSize = leftSize + rightSize - id - 1;
      leftSize = id;
      siz -= 1;
    }else{
      if(id > leftSize){
        std::memcpy(chunk + leftSize, chunk+ siz- rightSize, (id-leftSize)*sizeof(_T));
      }
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


#endif