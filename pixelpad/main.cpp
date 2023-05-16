#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::string filepath = argv[1];
    std::string extension = "";
    bool collectExtension = false;

    for (int i = 0; i < filepath.length(); i++) {
        if (collectExtension) {
            extension += filepath[i];
        }
        if (filepath[i] == '.') {
            collectExtension = true;
        }
    }

    if (extension != "txt") {
        return 1;
    }
}