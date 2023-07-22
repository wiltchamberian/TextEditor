#ifndef __CMD_BUFFER_H
#define __CMD_BUFFER_H

#include "Define.h"

class CmdHead;

class CmdBuffer{
public:
  ~CmdBuffer();
  CmdHead* addCmd(CmdType type, Length length);
  CmdHead* addCmd(const CmdHead* cmd);
  CmdHead* addCmd(const void* cmd);
  CmdHead* removeCmd();
  bool empty() const;
private:
  uint8_t* chunk = nullptr;
  Length chunk_size = 0;
  Length used_size = 0;
  Length cmd_count = 0;
};


#endif