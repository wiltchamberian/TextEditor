# **T**ext **E**ditor
A core for a text editor, may support advanced function and latex rendering in the future. 

Now focus on basic functions, initially finished, but may still have bugs and need more tests.

Support undo redo and some basic functions such as insert and remove

In theory, it can be embedded in any ui of text-editor with any rendering engine
or platform, also a good source for study.

It is still on developing but just passed the first round of test. 

⭐ Star us on GitHub — it helps!


# Compiler compatibility
> c++17, 20 (lower version may support but not test)

> Tested platforms:

> - Windows 10 : vscode

# Helper Document
main class is EditorCore and the interfaces are its public methods.
support undo redo

## Example
```c++

#include <iostream>
#include "EditorCore.h"
#include <cassert>

#define TEST_LOG std::cout << core.getContent() <<"\n***************\n";
void superTest(){
  std::string input = "wefwefwef\nwefwefwfoirjbrbr\nwefw12wefnbbr\nwefefefrbrb\nbrbrbrowenfef\n";

  EditorCore core;
  core.appendText(input);
  TEST_LOG
  core.removeText(2,3,2,5);
  TEST_LOG
  core.undo();
  TEST_LOG
  core.insertText(0, 7,"xyxyxyxyxy");
  TEST_LOG

  core.appendText("APPEND!");
  core.appendText("APPEND!");
  TEST_LOG

  for(int i=0; i< 100;++i){
    core.undo();
    core.redo();
    core.undo();
    core.undo();
    core.redo();
    core.redo();
  }
  TEST_LOG

  core.removeText(0,3,4,8);
  TEST_LOG

  core.appendText("abcde");
  uint32_t len = core.getLineCount();
  uint32_t length = core.getLineLength(len-2);
  TEST_LOG

  core.removeText(len-2, length-1, len-1, 0);
  TEST_LOG


  ///big data pressure test/////
  std::cout << "*******big data test********\n";
  for(int i = 0; i < 1000; ++i){
    core.appendText("this is your test"+std::to_string(i)+"\n");
    if(i==0 || i==1){
      TEST_LOG
    }
  }
  TEST_LOG

  for(int i=0; i< 1000;++i){
    core.undo();
  }
  TEST_LOG

  for(int i=0; i< 1000;++i){
    core.redo();
  }
  TEST_LOG
}

int main(int argc, char** argv){
  superTest();
  return 0;
}
```

