#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>

void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines);
void PrintOutDebug(TextFile& textfile);
void HandleLeftRightKeys(sf::Event& e, TextFile& textfile);
void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text, std::vector<sf::Text>& text_lines, sf::Vector2f offset);
void TryLoadFont(sf::Font& font, std::string path);
void ResizeView(const sf::RenderWindow& window, sf::View& view);
void ResizeTextRelativeToScreen(sf::Text& text, sf::RenderWindow& window);
void HandleEnter(sf::Event& e, std::vector<sf::Text>& text_lines, TextFile& textfile, sf::Font& text_font, sf::Vector2f offset);
void HandleUpDownKeys(sf::Event& e, TextFile& textfile);
void DrawAllTextLines(std::vector<sf::Text>& text_lines, sf::RenderWindow& window);
sf::Text CreateInitialTextLine(sf::Font& font, const sf::Vector2f& offset, int multiplier, std::string content);
void HighlightTypingPosition(sf::Text& current_text_line, sf::Font& font, int current_line_index, sf::RenderWindow& window);


static const float DEFAULT_SCREEN_WIDTH = 1024.f;
static const float DEFAULT_SCREEN_HEIGHT = 900.f;
static const sf::Color transparent_hightlight(255, 255, 255, 30); // RGBA(255, 255, 255, 30)
static const sf::Color full_highlight(255, 255, 255, 150); // RGBA(255, 255, 255, 150)


//<------------------ Variables that control the tick (cursor) on the screen --------------->
static const int TICK_COUNTDOWN = 5000;
static const int USER_TYPED_TICK_CD = 2000;
static int tick_counter = 0;
static bool tick_type = true;
static bool user_typed_tick = false;
static int user_typed_tick_counter = 0;


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
	for (int i = 0; i < temp_lines.size();++i) {

		// initially loop through all of the content to display it and fill the text files
		for (int k = 0; k < temp_lines.at(i).size(); ++k) {

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
				HandleUserInput(e, textfile, text_lines);
				HandleDelete(e, textfile, text, text_lines, text_offset);
				
			}
			// <------------- handle cursor movement ---------------------->
			if (e.type == sf::Event::KeyPressed)
			{
				HandleLeftRightKeys(e, textfile);
				HandleUpDownKeys(e, textfile);
				HandleEnter(e, text_lines, textfile,text_font,text_offset);
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
		HighlightTypingPosition(text_lines[textfile.gap_buffer.GetCurrentLine()], text_font, textfile.gap_buffer.GetGapStart(), window);
		DrawAllTextLines(text_lines, window);

		// --------- display on the screen --------
		window.display();

	}

	return 0;
}

void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines)
{
	// If the unicode is not backspace, enter, left or right arrow
	if (event.text.unicode != '\b' && event.text.unicode != 13 && event.text.unicode != sf::Keyboard::Left &&
		event.text.unicode != sf::Keyboard::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
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

		user_typed_tick = true;
		user_typed_tick_counter = USER_TYPED_TICK_CD;

		PrintOutDebug(textfile);
	}
}

void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text, std::vector<sf::Text>& text_lines, sf::Vector2f offset)
{
	if (e.text.unicode == '\b')
	{
		int line_before_delete = textfile.gap_buffer.GetCurrentLine();
		textfile.gap_buffer.DeleteCharacter(); 
		std::vector<char> changed_line = textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()];
		std::string temp_line;
		int line_after_delete = textfile.gap_buffer.GetCurrentLine();

		for (int i = 0; i < changed_line.size(); i++) {
			if (changed_line[i] != '\0') {
				temp_line.push_back(changed_line[i]);
			}
		}

		if (temp_line.size() == 0) {
			text_lines[textfile.gap_buffer.GetCurrentLine()].setString(" ");
		}
		else {
			text_lines[textfile.gap_buffer.GetCurrentLine()].setString(temp_line);
		}
		
		if (line_after_delete < line_before_delete) {
			text_lines.erase(text_lines.begin() + line_before_delete);

			int offset_x = static_cast<int>(offset.x);

			for (int i = 0; i < text_lines.size(); i++) {
				int offset_y = static_cast<int>(offset.y * (i + 1));

				text_lines[i].setPosition(sf::Vector2f(offset_x, offset_y));

			}
		}

		PrintOutDebug(textfile);
	}

}

void PrintOutDebug(TextFile& textfile)
{
	std::cout << std::endl;
	std::cout << " ------------------------ " << std::endl;
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

// TODO:
void SelectText() {

}

// TODO:
void SwapLines() {

}


void HandleUpDownKeys(sf::Event& e, TextFile& textfile) {
	if (e.key.code == sf::Keyboard::Down) {
		textfile.gap_buffer.MoveLineDown();
	}

	if (e.key.code == sf::Keyboard::Up) {
		textfile.gap_buffer.MoveLineUp();
	}

	user_typed_tick = true;
	user_typed_tick_counter = USER_TYPED_TICK_CD;
	//PrintOutDebug(textfile);
}

void HandleLeftRightKeys(sf::Event& e, TextFile& textfile)
{
	if (e.key.code == sf::Keyboard::Left) {
		textfile.gap_buffer.MoveGapLeft();
		//PrintOutDebug(textfile);
	}


	//PrintOutDebug(textfile);
	if (e.key.code == sf::Keyboard::Right) {
		textfile.gap_buffer.MoveGapRight();
		//PrintOutDebug(textfile);
	}


	//PrintOutDebug(textfile);
}

void HandleEnter(sf::Event& e, std::vector<sf::Text>& text_lines, TextFile& textfile, sf::Font& text_font, sf::Vector2f offset) {
	if (e.key.code == sf::Keyboard::Enter) {
		textfile.gap_buffer.InsertNewLine();

		// Do re-arranging of all lines here with the offset & add a new line to the vector

		sf::Text new_line_text;

		int rounded_offset_x = static_cast<int>(offset.x);
		
		new_line_text.setCharacterSize(20);
		new_line_text.setFont(text_font);
		new_line_text.setString(" ");
		new_line_text.setFillColor(sf::Color::White);
		
		// Insert the new line at the correct position
		int current_line = textfile.gap_buffer.GetCurrentLine();
		text_lines.insert(text_lines.begin() + current_line, new_line_text);

		// Finally, loop through all of the text lines after the new line and re-arrange their positions;
		for (int i = current_line; i < text_lines.size(); i++) 
		{
			int rounded_offset_y = static_cast<int>(offset.y * (i + 1)); // Has to be multiplied by the number of line
			text_lines[i].setPosition(sf::Vector2f(rounded_offset_x, rounded_offset_y));
		}

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

void HighlightTypingPosition(sf::Text& current_text_line,sf::Font& font, int current_line_index, sf::RenderWindow& window) {
	sf::RectangleShape rect;
	rect.setPosition(current_text_line.findCharacterPos(current_line_index).x +
		font.getGlyph('\0', 20, 0).bounds.left, current_text_line.getGlobalBounds().top +
		(current_text_line.getGlobalBounds().height + font.getGlyph('\0', 20, 0).bounds.top));
	rect.setSize(sf::Vector2f(font.getGlyph('\0', 20, 0).bounds.width, font.getGlyph('\0', 20, 0).bounds.height));

	//std::cout << " - " << (std::string)current_text_line.getString() << " - ";

	if (user_typed_tick) {
		
		if (user_typed_tick_counter == 0) {
			user_typed_tick = false;
		}
		else if (user_typed_tick_counter != 0) {

			rect.setFillColor(full_highlight);
			window.draw(rect);

			user_typed_tick_counter--;
			return;
		}
	}

	if (tick_counter == TICK_COUNTDOWN) {
		tick_type = !tick_type;
		tick_counter = 0;
	}

	if (tick_type) {
		rect.setFillColor(transparent_hightlight);
		window.draw(rect);
		tick_counter++;
		return;
	}
	else {
		rect.setFillColor(full_highlight);
		window.draw(rect);
		tick_counter++;
	}

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
