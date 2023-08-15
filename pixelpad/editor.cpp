#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <algorithm>
#include <tuple>
#include <windows.h>

void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines);
void PrintOutDebug(TextFile& textfile);
void HandleLeftRightKeys(sf::Event& e, TextFile& textfile, bool& selection_mode, std::tuple<int, int>& selection_start,
	 std::tuple<int, int>& selection_end, bool& shift_held_down, std::vector<std::tuple<int, int>>& highlight_indexes);
void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text, std::vector<sf::Text>& text_lines, sf::Vector2f offset,
	 std::vector<sf::Text>& line_counter, sf::Font& text_font, sf::Vector2f& line_counter_offset);
void TryLoadFont(sf::Font& font, std::string path);
void ResizeView(const sf::RenderWindow& window, sf::View& view);
void ResizeTextRelativeToScreen(sf::Text& text, sf::RenderWindow& window);
void HandleEnter(sf::Event& e, std::vector<sf::Text>& text_lines,
	 TextFile& textfile, sf::Font& text_font, sf::Vector2f offset, std::vector<sf::Text>& line_counter, sf::Vector2f& line_counter_offset);
void HandleUpDownKeys(sf::Event& e, TextFile& textfile, bool& selection_mode);
void DrawAllTextLines(std::vector<sf::Text>& text_lines, sf::RenderWindow& window);
	 sf::Text CreateInitialTextLine(sf::Font& font, const sf::Vector2f& offset, int multiplier, std::string content);
void HighlightTypingPosition(sf::Text& current_text_line, sf::Font& font, int current_line_index, sf::RenderWindow& window);
void PrintSelectedChars(std::tuple<int, int> selection_start, std::tuple<int, int> selection_end, TextFile& textfile, std::vector<std::tuple<int, int>> highlight_indexes);
void SetSelectionIndexes(TextFile& textfile, std::tuple<int, int> selection_start, std::tuple<int, int> selection_end, std::vector<std::tuple<int, int>>& highlight_indexes);
void DeleteSelection(std::vector<std::tuple<int, int>> highlight_indexes, TextFile& textfile, std::vector<sf::Text>& text_lines, std::tuple<int, int> selection_start, std::tuple<int, int> selection_end);
void DrawLineCountBar(std::vector<sf::Text>& line_counter, sf::RenderWindow& window, TextFile& textfile);
void PopulateCountBar(std::vector<sf::Text>& line_counter, TextFile& textfile, sf::Font& text_font, sf::Vector2f line_count_offset);
void TextInputHelper(sf::Event& e, TextFile& textfile);


// <----------------- Graphical settings & configuration ----------------------------------->
static const float DEFAULT_SCREEN_WIDTH = 1024.f;
static const float DEFAULT_SCREEN_HEIGHT = 900.f;
static const sf::Color transparent_hightlight(255, 255, 255, 30);			// RGBA(255, 255, 255, 30)
static const sf::Color full_highlight(255, 255, 255, 150);					// RGBA(255, 255, 255, 150)
static const sf::Color line_count_color(0x73, 0x93, 0xB3);
static int previous_line_selected = 0;


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
	sf::Vector2f line_counter_offset(20.f, 20.f);

	TryLoadFont(text_font, "./8bitfont.ttf"); // Loads the font into the system -> TODO: Move this to happen only once in the main file

	std::vector<sf::Text> text_lines;
	std::vector<sf::Text> line_counter;

	std::string temp_content_string;
	std::vector<std::vector<char>> temp_lines = textfile.gap_buffer.GetLines();

	// <------------ SELECTION MODE SECTION ----------->
	bool selection_mode = false;
	bool shift_held_down = false;
	std::tuple<int, int> selection_start;				   // if the line of selection_start > selection_end, we have to go up lines & copy characters from end to start
	std::tuple<int, int> selection_end;					   // else if the line of selection_end < selection_start, we have to go from start to end
	std::vector<std::tuple<int, int>> highlight_indexes;   // Arrays of tuples used to represent positions in the text array
	std::vector<std::tuple<int, int>> already_highlighted;

	// Initialize the text objects for each line in the opened file
	for (int i = 0; i < temp_lines.size(); ++i) {

		// initially loop through all of the content to display it and fill the text files
		for (int k = 0; k < temp_lines.at(i).size(); ++k) {

			if (temp_lines.at(i).at(k) != '\0') {
				temp_content_string.push_back(temp_lines.at(i).at(k));
			}
		}

		text_lines.push_back(CreateInitialTextLine(text_font, text_offset, i, temp_content_string));

		temp_content_string = ""; // reset the temp string
	}

	PopulateCountBar(line_counter, textfile, text_font, line_counter_offset);
	
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			// <------------- handle input ---------------------->
			if (e.type == sf::Event::TextEntered)
			{
				if (e.key.code == 'q') {
					PrintSelectedChars(selection_start, selection_end, textfile, highlight_indexes);
					continue;
				}

				HandleUserInput(e, textfile, text_lines);
				HandleDelete(e, textfile, text, text_lines, text_offset,line_counter, text_font, line_counter_offset);

			}
			// <------------- handle cursor movement ---------------------->
			if (e.type == sf::Event::KeyPressed)
			{

				if (e.key.code == sf::Keyboard::LShift) {
					shift_held_down = true;
				}


				HandleLeftRightKeys(e, textfile, selection_mode, selection_start, selection_end, shift_held_down, highlight_indexes);
				HandleUpDownKeys(e, textfile, selection_mode);
				HandleEnter(e, text_lines, textfile, text_font, text_offset, line_counter, line_counter_offset);
			}

			if (e.type == sf::Event::KeyReleased) {

				if (e.key.code == sf::Keyboard::LShift) {
					shift_held_down = false;
				}
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

		// After all of the events, update the editor state
		// --------- clear the screen ----------
		window.clear(sf::Color::Black);

		// Set the view here
		window.setView(view);

		// --------- draw on the screen ---------
		HighlightTypingPosition(text_lines[textfile.gap_buffer.GetCurrentLine()], text_font, textfile.gap_buffer.GetGapStart(), window);
		DrawAllTextLines(text_lines, window);
		DrawLineCountBar(line_counter, window, textfile);

		// --------- display on the screen --------
		window.display();

	}
	return 0;
}

void TextInputHelper(sf::Event& e, TextFile& textfile) {

	// If the user presses { and it isn't inside a "" or ''
	if (e.text.unicode == '{' || e.text.unicode == '[') {

		std::vector<char> current_line = textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()];

		int gap_end = textfile.gap_buffer.GetGapEnd();
		int gap_start = textfile.gap_buffer.GetGapStart();

		// If the gap end is not located at the end and the gap start is not at the 0th index,
		// we can check if the user is placing { within '' or ""
		if (gap_end != textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()].size() - 1
			&& gap_start != 0) {
			
			if (current_line[gap_start - 1] == '\'' && current_line[gap_start + 1] == '\'')
			{
				// The user is trying to place it within '' => place only one of the characters
				textfile.gap_buffer.InsertCharacter(e.text.unicode);
				return;
			}
			if (current_line[gap_start - 1] == '"' && current_line[gap_start + 1] == '"')
			{
				// The user is trying to place it within "" => place only one of the characters
				textfile.gap_buffer.InsertCharacter(e.text.unicode);
				return;
			}
		}

		// Otherwise, we put two characters and shift one index back to put the gap in-between
		if (e.text.unicode == '{') {
			textfile.gap_buffer.InsertCharacter('{');
			textfile.gap_buffer.InsertCharacter('}');
		}

		if (e.text.unicode == '[') {
			textfile.gap_buffer.InsertCharacter('[');
			textfile.gap_buffer.InsertCharacter(']');
		}

		// now, the gap buffer would look something like this [a,b,c,{,},_,_,_]
		// We need to shift one index back
		textfile.gap_buffer.MoveGapLeft();

		return;

	}

	// If none of the above special cases apply, enter the unicode as normally.
	textfile.gap_buffer.InsertCharacter(e.text.unicode);
}

void HandleUserInput(sf::Event& event, TextFile& textfile, std::vector<sf::Text>& text_lines)
{
	if (event.text.unicode != '\b' && event.text.unicode != '\r' &&
		event.key.code != sf::Keyboard::Left && event.key.code != sf::Keyboard::Right)
	{
		
		TextInputHelper(event, textfile);

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

void HandleDelete(sf::Event& e, TextFile& textfile, sf::Text& text, std::vector<sf::Text>& text_lines, sf::Vector2f offset,
	std::vector<sf::Text>& line_counter, sf::Font& text_font, sf::Vector2f& line_counter_offset)
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

		// if the line has changed after the deletion of a character, this means we have to remove a line 
		// and re-position all of the text lines, as well as the line count
		if (line_after_delete < line_before_delete) {
			text_lines.erase(text_lines.begin() + line_before_delete);

			int offset_x = static_cast<int>(offset.x);

			for (int i = 0; i < text_lines.size(); i++) {
				int offset_y = static_cast<int>(offset.y * (i + 1));
				text_lines[i].setPosition(sf::Vector2f(offset_x, offset_y));
			}
			PopulateCountBar(line_counter, textfile, text_font, line_counter_offset);

			previous_line_selected = textfile.gap_buffer.GetCurrentLine();

		}
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
			}
		}
		std::cout << std::endl;
	}
}

void HandleUpDownKeys(sf::Event& e, TextFile& textfile, bool& selection_mode) {
	if (e.key.code == sf::Keyboard::Down) {
		textfile.gap_buffer.MoveLineDown();
	}

	if (e.key.code == sf::Keyboard::Up) {
		textfile.gap_buffer.MoveLineUp();
	}

	user_typed_tick = true;
	user_typed_tick_counter = USER_TYPED_TICK_CD;
}

void HandleLeftRightKeys(sf::Event& e, TextFile& textfile, bool& selection_mode,
	std::tuple<int, int>& selection_start, std::tuple<int, int>& selection_end,
	bool& shift_held_down, std::vector<std::tuple<int, int>>& highlight_indexes)
{
	// Record the selection_start only the first time that the user presses any of the keys, after which turn on selection mode
	if (e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::Right) {
		if (!selection_mode && shift_held_down) {
			selection_start = std::make_tuple(textfile.gap_buffer.GetCurrentLine(), textfile.gap_buffer.GetGapStart());
			selection_mode = true;
		}
	}

	if (e.key.code == sf::Keyboard::Left) {
		textfile.gap_buffer.MoveGapLeft();
	}

	//PrintOutDebug(textfile);					
	if (e.key.code == sf::Keyboard::Right) {
		textfile.gap_buffer.MoveGapRight();
		//PrintOutDebug(textfile);
	}

	if (selection_mode) {
		selection_end = std::make_tuple(textfile.gap_buffer.GetCurrentLine(), textfile.gap_buffer.GetGapStart());
		SetSelectionIndexes(textfile, selection_start, selection_end, highlight_indexes);
	}
}

void PrintSelectedChars(std::tuple<int, int> selection_start, std::tuple<int, int> selection_end, TextFile& textfile, std::vector<std::tuple<int,int>> highlight_indexes) {

	std::vector<std::vector<char>> lines = textfile.gap_buffer.GetLines();
	std::string selected_characters = "";

	int _selection_start = std::get<0>(selection_start);
	int _selection_end = std::get<0>(selection_end);

	int starting_character = std::get<1>(selection_start);
	int ending_character = std::get<1>(selection_end);

	if (_selection_end < _selection_start) {
		for (int i = _selection_end; i < _selection_start + 1; i++) {
			if (i == _selection_end) { // if we are at the final line, start the character collection from the ending character instead
				for (int k = ending_character; k < lines[i].size(); k++) {
					if (lines[i][k] != '\0') {
						selected_characters.push_back(lines[i][k]);
					}

				}
				selected_characters.push_back('\n'); // Finally, if we go to the end, we add a new line
				continue;
			}
			if (i == _selection_start) { // if we are at the starting line, begin from the starting character
				for (int k = starting_character; k < lines[i].size(); k++) {
					if (lines[i][k] != '\0') {
						selected_characters.push_back(lines[i][k]);
					}
				}
				continue;
			}

			// else, just add everything from start to end
			for (int k = 0; k < lines[i].size(); k++) {
				if (lines[i][k] != '\0') {
					selected_characters.push_back(lines[i][k]);
				}
			}
			selected_characters.push_back('\n');

		}
	}

	std::cout << selected_characters;

}


void DeleteSelection(std::vector<std::tuple<int,int>> highlight_indexes, TextFile& textfile,
	std::vector<sf::Text>& text_lines, std::tuple<int, int> selection_start, std::tuple<int, int> selection_end) {


	std::vector<std::vector<char>> lines = textfile.gap_buffer.GetLines();
	std::string selected_characters = "";

	int _selection_start = std::get<0>(selection_start);
	int _selection_end = std::get<0>(selection_end);

	int starting_character = std::get<1>(selection_start);
	int ending_character = std::get<1>(selection_end);

	if (_selection_end < _selection_start) {
		for (int i = _selection_end; i < _selection_start + 1; i++) {
			if (i == _selection_end) { // if we are at the final line, start the character collection from the ending character instead
				for (int k = ending_character; k < lines[i].size(); k++) {
					if (lines[i][k] != '\0') {
						textfile.gap_buffer.GetLines()[i][k] = '\0';
					}

				}
				textfile.gap_buffer.GetLines().erase(textfile.gap_buffer.GetLines().begin() + i);
				continue;
			}
			if (i == _selection_start) { // if we are at the starting line, begin from the starting character
				for (int k = starting_character; k < lines[i].size(); k++) {
					if (lines[i][k] != '\0') {
						textfile.gap_buffer.GetLines()[i][k] = '\0';
					}
				}
				continue;
			}

			// else, just add everything from start to end
			for (int k = 0; k < lines[i].size(); k++) {
				if (lines[i][k] != '\0') {
					textfile.gap_buffer.GetLines()[i][k] = '\0';
				}
			}

			textfile.gap_buffer.GetLines().erase(textfile.gap_buffer.GetLines().begin() + i);

		}
	}

	std::cout << selected_characters;


}


void SetSelectionIndexes(TextFile& textfile, std::tuple<int, int> selection_start, std::tuple<int, int> selection_end, std::vector<std::tuple<int, int>>& highlight_indexes) {
	int _selection_start = std::get<0>(selection_start);
	int _selection_end = std::get<0>(selection_end);

	int starting_character = std::get<1>(selection_start);
	int ending_character = std::get<1>(selection_end);

	std::vector<std::vector<char>> lines = textfile.gap_buffer.GetLines();

	highlight_indexes.clear();


	if (_selection_end < _selection_start) { // if the start is in some line below and the end is somewhere up, aka user selected from right to left and went up a line
		for (int i = _selection_end; i < _selection_start + 1; i++) {
			if (i == _selection_end) { // if we are at the final line, start the character collection from the ending character instead
				for (int k = ending_character; k < lines[i].size(); k++) {

					if (lines[i][k] != '\0') {
						highlight_indexes.push_back(std::make_tuple(i, k));
					}

				}
				continue;
			}
			if (i == _selection_end) { // if we are at the starting line, begin from the starting character
				for (int k = starting_character; k < lines[i].size(); k++) {
					if (lines[i][k] != '\0') {
						highlight_indexes.push_back(std::make_tuple(i, k));
					}
				}
				continue;
			}

			// else, just add everything from start to end
			for (int k = 0; k < lines[i].size(); k++) {
				if (lines[i][k] != '\0') {
					highlight_indexes.push_back(std::make_tuple(i, k));
				}
			}
		}
		return; // finish after pushing the indexes into the vector
	}
}

void DrawLineCountBar(std::vector<sf::Text>& line_counter, sf::RenderWindow& window, TextFile& textfile) {
	for (int i = 0; i < line_counter.size(); i++) {

		if (i == textfile.gap_buffer.GetCurrentLine()) 
		{
			if (i != previous_line_selected) { // if the user has selected a different line, remove the highlight from the last line 
				line_counter[previous_line_selected].setFillColor(line_count_color);
				line_counter[i].setFillColor(sf::Color::White);
				previous_line_selected = i;

				continue;
			}
		}
		line_counter[i].setFillColor(line_count_color);
		window.draw(line_counter[i]);
	}
}

void PopulateCountBar(std::vector<sf::Text>& line_counter, TextFile& textfile, sf::Font& text_font, sf::Vector2f line_count_offset) {

	line_counter.clear();

	int line_count = textfile.gap_buffer.GetLines().size();
	


	sf::Text line_count_text;

	line_count_text.setCharacterSize(20);
	line_count_text.setFont(text_font);
	line_count_text.setFillColor(line_count_color);

	int rounded_offset_x = static_cast<int>(line_count_offset.x);

	for (int i = 0; i < line_count; i++) {
		int rounded_offset_y = static_cast<int>(line_count_offset.y * (i + 1));
		line_count_text.setString(std::to_string(i + 1));
		line_count_text.setPosition(sf::Vector2f(rounded_offset_x, rounded_offset_y));

		line_counter.push_back(line_count_text);
	}
}


void HandleEnter(sf::Event& e, std::vector<sf::Text>& text_lines,
	TextFile& textfile, sf::Font& text_font, sf::Vector2f offset,std::vector<sf::Text>& line_counter, sf::Vector2f& line_counter_offset) {
	if (e.key.code == sf::Keyboard::Enter) {

		// Capturing the count of the line before, as creating a new line might need re-arrangement
		int line_before = textfile.gap_buffer.GetCurrentLine(); 
		textfile.gap_buffer.InsertNewLine();

		sf::Text new_line_text;

		int rounded_offset_x = static_cast<int>(offset.x);
		new_line_text.setCharacterSize(20);
		new_line_text.setFont(text_font);
		new_line_text.setString(" ");
		new_line_text.setFillColor(sf::Color::White);

		if (line_before != textfile.gap_buffer.GetCurrentLine()) {
			std::vector<char> current_line_buffer = textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()];
			std::string current_line_text = "";

			// we count if there are any regular characters
			int regular_char_count = 0;
			for (int i = 0; i < current_line_buffer.size(); i++) {
				if (current_line_buffer[i] != '\0') {
					current_line_text.push_back(current_line_buffer[i]);
					regular_char_count++;
					continue;
				}
			}

			// if there are no regular characters on the new line, we can asume that no content was moved
			// Therefore, we set the string as " " (in order to be able to highlight it)
			if (regular_char_count == 0) {
				new_line_text.setString(" ");
			}
			else {
				new_line_text.setString(current_line_text);
			}
		}

		// Insert the new line at the correct position
		int current_line = textfile.gap_buffer.GetCurrentLine();
		text_lines.insert(text_lines.begin() + current_line, new_line_text);

		// We could have the scenario that the gap end is not 
		// located at the end of the gap itself
		// In this specific case, we have a scenario where existing 
		// text must be moved/changed both in the previous and current_line
		if (line_before != textfile.gap_buffer.GetCurrentLine()) {
			std::string previous_line_text = "";					// TO-DO: Remove this and create a more memory efficient solution						
			std::vector<char> previous_line_buffer = textfile.gap_buffer.GetLines()[line_before];
			for (int i = 0; i < previous_line_buffer.size(); i++) {
				if (previous_line_buffer[i] != '\0') {
					previous_line_text.push_back(previous_line_buffer[i]);
				}
			}
			text_lines[line_before].setString(previous_line_text);
		}

		// Finally, loop through all of the text lines after the new line and re-arrange their positions;
		for (int i = current_line; i < text_lines.size(); i++)
		{
			int rounded_offset_y = static_cast<int>(offset.y * (i + 1)); // Has to be multiplied by the number of line
			text_lines[i].setPosition(sf::Vector2f(rounded_offset_x, rounded_offset_y));
		}

		PopulateCountBar(line_counter, textfile, text_font, line_counter_offset);
		previous_line_selected = textfile.gap_buffer.GetCurrentLine();

		PrintOutDebug(textfile);

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


// Check whether the user has pressed a key with user_typed_tick
// Depending on the bool, the highlight either stays fully filled or blinks in a less transparent mode
void HighlightTypingPosition(sf::Text& current_text_line, sf::Font& font, int current_line_index, sf::RenderWindow& window) {
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
