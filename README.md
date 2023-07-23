# **T**ext **E**ditor
A core for a text editor, may support advanced function and latex rendering in the future. 

Now focus on basic functions, initially finished, but may still have bugs and need more tests.

Support undo redo and some basic functions such as insert and remove

In theory, it can be embedded in any ui of text-editor with any rendering engine
or platform, also a good source for study.

It is still on developing and not tested enough.

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
}

int main(int argc, char** argv){
  superTest();
  return 0;
}
```

