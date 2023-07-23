/*********************************************************************************
 * Author: ACE
 * Description: CommandBuffer, it is a stack, used to save undo,redo commands
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/
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