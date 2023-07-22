#include "Commands.h"
#include "CmdBuffer.h"

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