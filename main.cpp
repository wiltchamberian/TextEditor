#include <iostream>
#include "EditorCore.h"
#include <cassert>

void superTest(){
  std::string input = "wefwefwef\nwefwefwfoirjbrbr\nwefw12wefnbbr\nwefefefrbrb\nbrbrbrowenfef\n";

  std::string splitline = "\n***************\n";
  EditorCore core;
  core.appendText(input);
  std::cout << core.getContent() <<splitline;
  core.removeText(2,3,2,5);
  std::cout << core.getContent() <<splitline;
  core.undo();
  std::cout << core.getContent() <<splitline;
  core.insertText(0, 7,"xyxyxyxyxy");
  std::cout << core.getContent() <<splitline;

  for(int i=0; i< 100;++i){
    core.undo();
    core.redo();
    core.undo();
    core.undo();
    core.redo();
    core.redo();
  }

  std::cout << core.getContent() <<splitline;

  core.removeText(0,3,4,8);
  std::cout << core.getContent() <<splitline;
}

int main(int argc, char** argv){
  superTest();
  return 0;
}