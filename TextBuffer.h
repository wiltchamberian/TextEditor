/*********************************************************************************
 * Author: ACE
 * Description: TextBuffer, used to save text, used with piecetable
 * Copyright (c) 2023 [ACE]. All rights reserved.
 *
 * This source code is licensed under the [GPL3.0] license, 
 * details of which can be found in the license file.
 *
 *********************************************************************************/

#ifndef __TEXT_BUFFER_H
#define __TEXT_BUFFER_H

#include "Define.h"

class Character{
public:
  Character(){}
  Character(char c):ch(c){}
  char ch;
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

#endif