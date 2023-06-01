#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>

void PrintOutCharactersFromVector(std::vector<char> characters);


int Editor::StartEditorWithFile(std::string filename, std::string filepath)
{

	// Use one method for handling the opening of files? Can add an explicit check if there are more than 1 files opened, if yes -> don't open a new window
	// 1) First open the file at the filepath and read its contents
	// 1.2) Create a text file object and add it to the array of other text objects
	// 2) Initialize a gap buffer and add the content into a char array
	TextFile textfile(filename, filepath);

	sf::RenderWindow window(sf::VideoMode(1650, 900), "pixelpad");
	sf::Event e;

	std::cout << textfile.GetGapBuffer().GetContent().size() << " is the size of content" << std::endl;



	while (window.isOpen()) {
		while (window.pollEvent(e)) {

			if (e.text.unicode < 128) {
				textfile.GetGapBuffer().InsertCharacter((char)e.text.unicode);
				PrintOutCharactersFromVector(textfile.GetGapBuffer().GetContent());
			}

			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
	}

	return 0;
}
	

void PrintOutCharactersFromVector(std::vector<char> characters) {
	// <--------------- Printing out contents of the gap buffer for testing ---------------------------->
	for (int i = 0; i < characters.size(); i++) {
		std::cout << characters[i];
	}
}




