#include <iostream>
#include "EditorCore.h"


int main(int argc, char** argv){
  EditorCore core;
  String text = "abcd\n123";
  core.appendTextU(text);
  Length lineCount = core.getLineCount();
  for(int i=0; i<lineCount;++i){
    std::cout << core.getLineText(i);
  }

  std::cout <<"\n\n";
  core.undo();
  lineCount = core.getLineCount();
  std::cout << core.getContent();

  std::cout <<"\n\n";
  core.redo();
  lineCount = core.getLineCount();
  std::cout<<core.getContent();
  
  return 0;
}