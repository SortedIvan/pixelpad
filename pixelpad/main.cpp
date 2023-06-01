#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include "textfile.h"
#include "editor.h"

int main(int argc, char* argv[])
{

    if (argv[1] == NULL) {
        // run without file
    }

    std::string filepath = argv[1];
    std::string extension = "";
    bool collectExtension = false;
    
    Editor editor; // Create the editor object that will be used throughout the application cycle

    for (int i = 0; i < filepath.length(); i++) {
        if (collectExtension) {
            extension += filepath[i];
        }
        if (filepath[i] == '.') {
            collectExtension = true;
        }
    }

    if (extension != "txt") { // Check for extension type, only accept .txt files
        return 1;
    }
    
    std::string base_filename = filepath.substr(filepath.find_last_of("/\\") + 1);

    editor.StartEditorWithFile(base_filename, filepath);


}