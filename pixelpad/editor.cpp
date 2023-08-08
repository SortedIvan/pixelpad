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
void HandleEnter(sf::Event& e, sf::Text& text, TextFile& textfile);
void HandleUpDownKeys(sf::Event& e, TextFile& textfile);
sf::Text CreateTextNewLine(sf::Font& font);

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

	sf::Vector2f text_offset(50.f, 20.f);

	TryLoadFont(text_font, "./8bitfont.ttf"); // Loads the font into the system -> TODO: Move this to happen only once in the main file

	std::vector<sf::Text> text_lines;

	// Initialize the text objects for each line in the opened file
	for (int i = 0; i < textfile.gap_buffer.GetLines().size();i++) {
		text_lines.push_back(CreateTextNewLine(text_font));
	}




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
				HandleUpDownKeys(e, textfile);
				HandleEnter(e, text, textfile);
				PrintOutDebug(textfile);
			}

			if (e.type == sf::Event::Resized)
			{

				// When the window is resized, update the view to match the new size
				view.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
				
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
	if (event.text.unicode != '\b' && event.text.unicode != 13 && event.text.unicode != '37' && 
		event.text.unicode != '39' && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
		event.key.code != sf::Keyboard::Down && event.key.code != sf::Keyboard::Up)
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
	for (int i = 0; i < textfile.GetGapBuffer().GetLines().size(); i++) {
		for (int k = 0; k < textfile.GetGapBuffer().GetLines()[i].size(); k++) {

			if (textfile.GetGapBuffer().GetLines()[i][k] == '\0') {
				std::cout << "_";
			}
			else {
				std::cout << textfile.GetGapBuffer().GetLines()[i][k];
			}		}
		std::cout << std::endl;
	}
}

void HandleUpDownKeys(sf::Event& e, TextFile& textfile) {
	if (e.key.code == sf::Keyboard::Down) {
		textfile.gap_buffer.MoveLineDown();
	}

	if (e.key.code == sf::Keyboard::Up) {
		textfile.gap_buffer.MoveLineUp();
	}

	PrintOutDebug(textfile);
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

void HandleEnter(sf::Event& e, sf::Text& text, TextFile& textfile) {
	if (e.key.code == sf::Keyboard::Enter) {
		textfile.gap_buffer.InsertNewLine();
	}
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
	float aspect_ratio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(DEFAULT_SCREEN_HEIGHT * aspect_ratio, DEFAULT_SCREEN_HEIGHT);
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

sf::Text CreateTextNewLine(sf::Font& font, const sf::Vector2f& offset, int multiplier) {

	// multiplier starts at 0, so increase by 1

	sf::Text text;
	text.setCharacterSize(20);
	text.setFont(font);
	text.setString("");
	text.setFillColor(sf::Color::White);

	return text;
}


