#include "TextBuffer.h"

void TextBuffer::addCharacter(const Character& ch){
  vec.push_back(ch);
}

void TextBuffer::addCharacters(const Character* chs, Length length){
  vec.insert(vec.end(), chs, chs + length);
}

const Character* TextBuffer::data(Index index) const{
  return vec.data() + index;
}

Index TextBuffer::getEnd() const{
  return vec.size();
}