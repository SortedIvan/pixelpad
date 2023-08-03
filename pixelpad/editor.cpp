#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>

void HandleUserInput(sf::Event& event, TextFile& textfile, sf::Text& text);
void PrintOutDebug(TextFile& textfile);
void HandleLeftRightKeys(sf::Event& e, TextFile& textfile);
void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text);
void TryLoadFont(sf::Font& font, std::string path);
void ResizeView(const sf::RenderWindow& window, sf::View& view);
void ResizeTextRelativeToScreen(sf::Text& text, sf::RenderWindow& window);
void UpdateTextFromGapBuffer(sf::Text& text, TextFile& textfile);

static const float DEFAULT_SCREEN_WIDTH = 1024.f;
static const float DEFAULT_SCREEN_HEIGHT = 900.f;

int Editor::StartEditorWithFile(std::string filename, std::string filepath)
{

	// Use one method for handling the opening of files? Can add an explicit check if there are more than 1 files opened, if yes -> don't open a new window
	// 1) First open the file at the filepath and read its contents
	// 1.2) Create a text file object and add it to the array of other text objects
	// 2) Initialize a gap buffer and add the content into a char array

	TextFile textfile(filename, filepath);
	sf::RenderWindow window(sf::VideoMode(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT), "pixelpad");
	sf::Event e;

	sf::Font text_font;
	sf::Text text;

	// Set the view as the center of the screen and make it full size
	sf::View view(sf::Vector2f(DEFAULT_SCREEN_WIDTH / 2.f, DEFAULT_SCREEN_HEIGHT / 2.f),
		sf::Vector2f(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT));

	TryLoadFont(text_font, "./8bitfont.ttf"); // Loads the font into the system -> TODO: Move this to happen only once in the main file

	//------- Text customization ---------
	text.setCharacterSize(20);
	text.setFont(text_font);
	text.setString("Test");
	text.setFillColor(sf::Color::White);
	ResizeTextRelativeToScreen(text, window);


	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			// <------------- handle input ---------------------->
			if (e.type == sf::Event::TextEntered)
			{
				if (e.text.unicode < 128) {
					HandleUserInput(e, textfile, text);
					HandleDelete(e, textfile, text);
					PrintOutDebug(textfile);
				}
			}
			// <------------- handle cursor movement ---------------------->
			if (e.type == sf::Event::KeyPressed)
			{
				HandleLeftRightKeys(e, textfile);
			}

			if (e.type == sf::Event::Resized)
			{

				// When the window is resized, update the view to match the new size
				view.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
				ResizeTextRelativeToScreen(text, window);
			}

			if (e.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// After all of the events, update the game state
		// --------- clear the screen ----------
		window.clear(sf::Color::Black);

		// Set the view here
		window.setView(view);

		// --------- draw on the screen ---------
		window.draw(text);

		// --------- display on the screen --------
		window.display();

	}

	return 0;
}


void HandleUserInput(sf::Event& event, TextFile& textfile, sf::Text& text)
{
	// If the unicode is not backspace, enter, left or right arrow
	if (event.text.unicode != '\b' && event.text.unicode != 13 && event.text.unicode != '37' && event.text.unicode != '39' && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
	{
		textfile.gap_buffer.InsertCharacter(event.text.unicode);
		UpdateTextFromGapBuffer(text, textfile);
	}
}

void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text)
{
	if (e.text.unicode == '\b')
	{
		textfile.gap_buffer.DeleteCharacter();
		UpdateTextFromGapBuffer(text, textfile);
	}
}

void PrintOutDebug(TextFile& textfile)
{
	std::cout << std::endl;
	std::cout << textfile.gap_buffer.GetGapStart() << " is the gap start" << std::endl;
	std::cout << textfile.gap_buffer.GetGapEnd() << " is the gap end" << std::endl;
	std::cout << textfile.gap_buffer.GetGapSize() << " is the gap size" << std::endl;
	std::cout << textfile.gap_buffer.GetContent().size() << " is the content size" << std::endl;

	for (int i = 0; i < textfile.gap_buffer.GetContent().size(); i++)
	{
		int size = textfile.gap_buffer.GetGapSize() + i;
		if (i == textfile.gap_buffer.GetGapStart())
		{
			for (int k = i; k < size; i++, k++)
			{
				std::cout << "_";
			}
		}
		else
		{
			std::cout << textfile.gap_buffer.GetContent().at(i);
		}
	}
}

void HandleLeftRightKeys(sf::Event& e, TextFile& textfile)
{
	if (e.key.code == sf::Keyboard::Left)
		textfile.gap_buffer.MoveGapLeft();
	PrintOutDebug(textfile);
	if (e.key.code == sf::Keyboard::Right)
		textfile.gap_buffer.MoveGapRight();
	PrintOutDebug(textfile);
}

void TryLoadFont(sf::Font& font, std::string path)
{
	if (!font.loadFromFile(path))
	{
		std::cout << "Error loading the font file" << std::endl;
		system("pause");
	}
}

void ResizeView(const sf::RenderWindow& window, sf::View& view)
{
	float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(DEFAULT_SCREEN_HEIGHT * aspectRatio, DEFAULT_SCREEN_HEIGHT);
}

void ResizeTextRelativeToScreen(sf::Text& text, sf::RenderWindow& window)
{
	sf::Vector2f textOffset(50.f, 20.f);

	// Round the offset values to the nearest integer to avoid slight shifting
	int roundedOffsetX = static_cast<int>(textOffset.x);
	int roundedOffsetY = static_cast<int>(textOffset.y);

	text.setPosition(roundedOffsetX, roundedOffsetY);
}

// Function to update the sf::Text object based on the gap_buffer content
void UpdateTextFromGapBuffer(sf::Text& text, TextFile& textfile)
{

}
