#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>

void HandleUserInput(sf::Event& event, TextFile& textfile);
void PrintOutDebug(TextFile& textfile);
void HandleLeftRightKeys(sf::Event& e, TextFile& textfile);


int Editor::StartEditorWithFile(std::string filename, std::string filepath)
{

	// Use one method for handling the opening of files? Can add an explicit check if there are more than 1 files opened, if yes -> don't open a new window
	// 1) First open the file at the filepath and read its contents
	// 1.2) Create a text file object and add it to the array of other text objects
	// 2) Initialize a gap buffer and add the content into a char array

	TextFile textfile(filename, filepath);
	sf::RenderWindow window(sf::VideoMode(1650, 900), "pixelpad");
	sf::Event e;

	while (window.isOpen()) {
		while (window.pollEvent(e)) {

			// <------------- handle input ---------------------->
			if (e.type == sf::Event::TextEntered) {
				if (e.text.unicode < 128) {
					HandleUserInput(e, textfile);
					PrintOutDebug(textfile);
				}

			}
			// <------------- handle cursor movement ---------------------->
			if (e.type == sf::Event::KeyPressed) {
				HandleLeftRightKeys(e, textfile);
			}

			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
	}

	return 0;
}


void HandleUserInput(sf::Event& event, TextFile& textfile) {
	// If the unicode is not backspace, enter, left or right arrow
	if (event.text.unicode != '\b' && event.text.unicode != 13 && event.text.unicode != '37' && event.text.unicode != '39' && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
	{
		textfile.gap_buffer.InsertCharacter(event.text.unicode);
	}
}

void PrintOutDebug(TextFile& textfile) {
	std::cout << textfile.gap_buffer.GetGapStart() << " is the gap start" << std::endl;
	std::cout << textfile.gap_buffer.GetGapEnd() << " is the gap end" << std::endl;
	std::cout << textfile.gap_buffer.GetGapSize() << " is the gap size" << std::endl;

	for (int i = 0; i < textfile.gap_buffer.GetContent().size(); i++) {
		if (i == textfile.gap_buffer.GetGapStart()) {
			for (int k = i; k < textfile.gap_buffer.GetGapEnd(); i++, k++) {
				std::cout << "_";
			}
		}
		else {
			std::cout << textfile.gap_buffer.GetContent().at(i);
		}
	}
}

void HandleLeftRightKeys(sf::Event& e, TextFile& textfile) {
	if (e.key.code == sf::Keyboard::Left)
		textfile.gap_buffer.MoveGapLeft();
		PrintOutDebug(textfile);
	if (e.key.code == sf::Keyboard::Right)
		textfile.gap_buffer.MoveGapRight();
}

