#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>

void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines);
void PrintOutDebug(TextFile& textfile);
void HandleLeftRightKeys(sf::Event& e, TextFile& textfile);
void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text);
void TryLoadFont(sf::Font& font, std::string path);
void ResizeView(const sf::RenderWindow& window, sf::View& view);
void ResizeTextRelativeToScreen(sf::Text& text, sf::RenderWindow& window);
void UpdateTextFromGapBuffer(sf::Text& text, TextFile& textfile);
void HandleEnter(sf::Event& e, sf::Text& text, TextFile& textfile);
void HandleUpDownKeys(sf::Event& e, TextFile& textfile);
void DrawAllTextLines(std::vector<sf::Text>& text_lines, sf::RenderWindow& window);
sf::Text CreateInitialTextLine(sf::Font& font, const sf::Vector2f& offset, int multiplier, std::string content);



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

	std::string temp_content_string;
	std::vector<std::vector<char>> temp_lines = textfile.gap_buffer.GetLines();

	// Initialize the text objects for each line in the opened file
	for (int i = 0; i < temp_lines.size();i++) {

		// initially loop through all of the content to display it and fill the text files
		for (int k = 0; k < temp_lines.at(i).size(); k++) {

			if (temp_lines.at(i).at(k) != '\0') {
				temp_content_string.push_back(temp_lines.at(i).at(k));
			}
		}

		text_lines.push_back(CreateInitialTextLine(text_font, text_offset, i, temp_content_string));

		temp_content_string = ""; // reset the temp string
	}

	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			// <------------- handle input ---------------------->
			if (e.type == sf::Event::TextEntered)
			{
				if (e.text.unicode < 128) {
					HandleUserInput(e, textfile, text_lines);
					HandleDelete(e, textfile, text);
					//PrintOutDebug(textfile);
				}
			}
			// <------------- handle cursor movement ---------------------->
			if (e.type == sf::Event::KeyPressed)
			{
				HandleLeftRightKeys(e, textfile);
				HandleUpDownKeys(e, textfile);
				HandleEnter(e, text, textfile);
				//PrintOutDebug(textfile);
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
		DrawAllTextLines(text_lines, window);

		// --------- display on the screen --------
		window.display();

	}

	return 0;
}


void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines)
{
	// If the unicode is not backspace, enter, left or right arrow
	if (event.text.unicode != '\b' && event.text.unicode != 13 && event.text.unicode != '37' && 
		event.text.unicode != '39' && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
		event.key.code != sf::Keyboard::Down && event.key.code != sf::Keyboard::Up)
	{
		textfile.gap_buffer.InsertCharacter(event.text.unicode);
		
		int current_line = textfile.gap_buffer.GetCurrentLine();

		std::vector<char> changed_line = textfile.gap_buffer.GetLines()[current_line];
		std::string temp_line;

		for (int i = 0; i < changed_line.size(); i++) {
			if (changed_line[i] != '\0') {
				temp_line.push_back(changed_line[i]);
			}
		}

		text_lines[current_line].setString(temp_line);

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

	//PrintOutDebug(textfile);
}

void HandleLeftRightKeys(sf::Event& e, TextFile& textfile)
{
	if (e.key.code == sf::Keyboard::Left)
		textfile.gap_buffer.MoveGapLeft();
	//PrintOutDebug(textfile);
	if (e.key.code == sf::Keyboard::Right)
		textfile.gap_buffer.MoveGapRight();
	//PrintOutDebug(textfile);
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

sf::Text CreateInitialTextLine(sf::Font& font, const sf::Vector2f& offset, int multiplier, std::string content) {

	// multiplier starts at 0, so increase by 1

	multiplier += 1;

	int offset_x = static_cast<int>(offset.x);
	int offset_y = static_cast<int>(offset.y * multiplier);

	sf::Text text;
	text.setCharacterSize(20);
	text.setFont(font);
	text.setString("");
	text.setFillColor(sf::Color::White);
	text.setPosition(sf::Vector2f(offset_x, offset_y));
	text.setString(content);

	return text;
}

void DrawAllTextLines(std::vector<sf::Text>& text_lines, sf::RenderWindow& window) {
	for (int i = 0; i < text_lines.size(); i++) 
	{
		window.draw(text_lines.at(i));
	}
}
