#include <iostream>
#include "EditorCore.h"
#include <cassert>

//随机生成一些文本插入删除，撤销恢复混合操作。
//同时模拟结果
void handTest(){
  EditorCore core;
  String text = "abcd\n123";
  core.appendTextU(text);
  String text2 = "xyzd";
  String text3 = "oooooo\ndds";
  core.appendTextU(text2);
  core.insertTextU(1, 3, text3);
  String content = core.getContent();
  core.undo();
  core.redo();
  assert(content == core.getContent());
  Length n = core.getLineCount();
  for(int i = 0; i < n ;++i){
    std::cout << core.getLineText(i);
  }
}

void superTest(){
  std::string input = "wefwefwef\nwefwefwfoirjbrbr\nwefw12wefnbbr\nwefefefrbrb\nbrbrbrowenfef\n";

  std::string splitline = "\n***************\n";
  EditorCore core;
  core.appendTextU(input);
  std::cout << core.getContent() <<splitline;
  core.removeTextU(2,3,2,5);
  std::cout << core.getContent() <<splitline;
  core.undo();
  std::cout << core.getContent() <<splitline;
  core.insertTextU(0, 7,"xyxyxyxyxy");
  std::cout << core.getContent() <<splitline;
}

int main(int argc, char** argv){
  superTest();
  return 0;
}