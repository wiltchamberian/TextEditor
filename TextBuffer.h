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