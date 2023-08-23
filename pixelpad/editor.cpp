#include "editor.h"
#include "textfile.h"
#include "SFML/Graphics.hpp"
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <algorithm>
#include <tuple>
#include <windows.h>
#include <typeinfo>

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
	 TextFile& textfile, sf::Font& text_font, sf::Vector2f offset, std::vector<sf::Text>& line_counter, 
	sf::Vector2f& line_counter_offset, sf::View& view);
void HandleUpDownKeys(sf::Event& e, TextFile& textfile, bool& selection_mode, bool& ctrl_pressed, sf::View& view, sf::RenderWindow& window);
void DrawAllTextLines(std::vector<sf::Text>& text_lines, sf::RenderWindow& window);
	 sf::Text CreateInitialTextLine(sf::Font& font, const sf::Vector2f& offset, int multiplier, std::string content);
void HighlightTypingPosition(sf::Text& current_text_line, sf::Font& font, int current_line_index, sf::RenderWindow& window, sf::View& view);
void PrintSelectedChars(std::tuple<int, int> selection_start, std::tuple<int, int> selection_end, TextFile& textfile, std::vector<std::tuple<int, int>> highlight_indexes);
void DrawLineCountBar(std::vector<sf::Text>& line_counter, sf::RenderWindow& window, TextFile& textfile);
void PopulateCountBar(std::vector<sf::Text>& line_counter, TextFile& textfile, sf::Font& text_font, sf::Vector2f line_count_offset);
void TextInputHelper(sf::Event& e, TextFile& textfile);
void HandleUpAndDownIndexing(TextFile& textfile, const int old_line);

// <----------------- Graphical settings & configuration ----------------------------------->
static const float DEFAULT_SCREEN_WIDTH = 1024.f;
static const float DEFAULT_SCREEN_HEIGHT = 900.f;
static const sf::Color transparent_hightlight(255, 255, 255, 30);			// RGBA(255, 255, 255, 30)
static const sf::Color full_highlight(255, 255, 255, 150);					// RGBA(255, 255, 255, 150)
static const sf::Color line_count_color(0x73, 0x93, 0xB3);
static const sf::Color default_bg_color(0x00, 0x01, 0x33);
static int previous_line_selected = 0;


//<------------------ Variables that control the tick (cursor) on the screen --------------->
static const int TICK_COUNTDOWN = 5000;
static const int USER_TYPED_TICK_CD = 2000;
static int tick_counter = 0;
static bool tick_type = true;
static bool user_typed_tick = false;
static int user_typed_tick_counter = 0;
static sf::Vector2f cursor_position;
static sf::Vector2f old_cursor_position;
static sf::Vector2f cursor_size;

//<---------------------------------- Command line ----------------------------------------->
static std::string current_command = "";
bool command_mode = false;

enum IndexMode {
	DefaultIndex,
	SaveIndex
};

static IndexMode index_mode = DefaultIndex;

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

	sf::Vector2f text_offset(100.f, 20.f); // Offset for the text lines
	sf::Vector2f line_counter_offset(40.f, 20.f); // Offset for the left line count bar

	TryLoadFont(text_font, "./testfont.ttf"); // Loads the font into the system -> TODO: Move this to happen only once in the main file

	std::vector<sf::Text> text_lines;
	std::vector<sf::Text> line_counter;

	// Temporary string that we use to initialize the gap buffer -> TODO: Remove this and create a more memory efficient solution
	std::string temp_content_string;
	std::vector<std::vector<char>> temp_lines = textfile.gap_buffer.GetLines();

	// <------------ SELECTION MODE SECTION ----------->
	bool selection_mode = false;
	bool shift_held_down = false;
	std::tuple<int, int> selection_start;				   // if the line of selection_start > selection_end, we have to go up lines & copy characters from end to start
	std::tuple<int, int> selection_end;					   // else if the line of selection_end < selection_start, we have to go from start to end
	std::vector<std::tuple<int, int>> highlight_indexes;   // Arrays of tuples used to represent positions in the text array
	std::vector<std::tuple<int, int>> already_highlighted;

	// <------------- MISC ---------------------------->
	bool ctrl_held_down = false;
	// --

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

	// The countbar represents the line counter on the left side of the editor
	// After we initialize the gap buffer, as well as all of the text lines, we populate the line count vector
	PopulateCountBar(line_counter, textfile, text_font, line_counter_offset);
	
	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{

			if (e.key.code == 27) { // Unicode for escape
				if (command_mode) {
					command_mode = false;
					continue;
				}
			}

			if (e.text.unicode == 16) { // CTRL + Q for command mode
				command_mode = true;

				std::cout << std::endl;
				std::cout << "(" << std::get<0>(selection_start) << ", " << std::get<1>(selection_start) << ")";
				std::cout << std::endl;
				std::cout << "(" << std::get<0>(selection_end) << ", " << std::get<1>(selection_end) << ")";
				std::cout << std::endl;

				ctrl_held_down = false;
				selection_mode = false;

				//std::cout << "Command mode activated";
				continue;
			}
			// <------------- handle input ---------------------->
			if (e.type == sf::Event::TextEntered)
			{

				//std::cout << e.key.code;

				if (!command_mode) {
					HandleUserInput(e, textfile, text_lines);
					HandleDelete(e, textfile, text, text_lines, text_offset, line_counter, text_font, line_counter_offset);
				}
			}
			if (e.type == sf::Event::KeyPressed)
			{
				if (!command_mode) {
					if (e.key.code == sf::Keyboard::LShift) {

						if (ctrl_held_down) {
							ctrl_held_down = false;
						}
					
						shift_held_down = true;
					}
					if (e.key.code == sf::Keyboard::LControl) {
						if (shift_held_down) {
							shift_held_down = false;
						}
						ctrl_held_down = true;
						std::cout << "Ctrl held down";
					}

					HandleLeftRightKeys(e, textfile, selection_mode, selection_start, selection_end, shift_held_down, highlight_indexes);
					HandleUpDownKeys(e, textfile, selection_mode, ctrl_held_down, view, window);
					HandleEnter(e, text_lines, textfile, text_font, text_offset, line_counter, line_counter_offset, view);
				}
			}

			if (!command_mode) {
				if (e.type == sf::Event::KeyReleased) {

					if (e.key.code == sf::Keyboard::LShift) {
						shift_held_down = false;
					}

					if (e.key.code == sf::Keyboard::LControl) {
						ctrl_held_down = false;
						//std::cout << "ctrl released";
					}
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
		window.clear(default_bg_color);

		// Set the view here
		window.setView(view);

		// --------- draw on the screen ---------
		HighlightTypingPosition(text_lines[textfile.gap_buffer.GetCurrentLine()], text_font, textfile.gap_buffer.GetGapStart(), window, view);
		DrawAllTextLines(text_lines, window);
		DrawLineCountBar(line_counter, window, textfile);

		// --------- display on the screen --------
		window.display();

	}
	return 0;
}

void HandleCommandInput() {
	
}


void ExecuteCommand() {
	if (command_mode) {

		// If there is nothing to interpret, stop command
		if (current_command.size() == 0) {
			command_mode = false;
			current_command = "";
			return;
		}

		std::string command_type = "";

		command_type.push_back(current_command[0]); // GET THE FIRST TWO CHARS FOR THE CMD
		command_type.push_back(current_command[1]);
		
		

		if (command_type != "rp") { // TODO: Replace this with a list of commands when there's one
			return; 
		}

		int line_one = 0;
		int line_two = 0;

		bool fill_line_two = false;

		try {
			for (int i = 2; i < current_command.size(); i++) {
				if (current_command[i] == ':') {
					fill_line_two = true;
				}

				if (!fill_line_two) {
					line_one += std::stoi(&current_command[i]);
				}
				else {
					line_two += std::stoi(&current_command[i]);
				}

			}
			std::cout << command_type << " " << line_one << ":" << line_two;
		}
		catch (const std::exception & e) { // if it fails, then the command was wrong
			return;
		}
	}
}

void TextInputHelper(sf::Event& e, TextFile& textfile) {

	// If the user presses { and it isn't inside a "" or ''
	if (e.text.unicode == '{' || e.text.unicode == '[' || e.text.unicode == '(') {

		std::vector<char> current_line = textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()];

		int gap_end = textfile.gap_buffer.GetGapEnd();
		int gap_start = textfile.gap_buffer.GetGapStart();

		// If the gap end is not located at the end and the gap start is not at the 0th index,
		// we can check if the user is placing { within '' or ""
		if (gap_end != textfile.gap_buffer.GetLines()[textfile.gap_buffer.GetCurrentLine()].size() - 1
			&& gap_start != 0) {
			
			if (current_line[gap_start - 1] == '\'' && current_line[gap_end + 1] == '\'')
			{
				// The user is trying to place it within '' => place only one of the characters
				textfile.gap_buffer.InsertCharacter(e.text.unicode);
				return;
			}
			if (current_line[gap_start - 1] == '"' && current_line[gap_end + 1] == '"')
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

		if (e.text.unicode == '(') {
			textfile.gap_buffer.InsertCharacter('(');
			textfile.gap_buffer.InsertCharacter(')');
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
		event.key.code != sf::Keyboard::Left && event.key.code != sf::Keyboard::Right && event.text.unicode != 36 
		&& event.key.code != sf::Keyboard::Escape)
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


// Helper function to help with up and down indexing handling for the different types of index modes
void HandleUpAndDownIndexing(TextFile& textfile, const int old_line) {
	if (index_mode == SaveIndex) { // If in save index mode, don't do the next steps
		return;
	}

	// Get the gap starts of the two lines to check the difference
	int old_line_gap_start = textfile.gap_buffer.GetGapStarts()[old_line];
	int new_line_gap_start = textfile.gap_buffer.GetGapStart();

	int current_line = textfile.gap_buffer.GetCurrentLine();

	//if () do pre optimization and check if the gap start is already what we need it to be


	if (old_line_gap_start == new_line_gap_start) {
		std::cout << std::endl;
		std::cout << old_line_gap_start << std::endl;
		std::cout << new_line_gap_start << std::endl;
		return; // We don't need to do anything further
	}

	if (old_line_gap_start > new_line_gap_start) { // This means that the line we moved to has to have its index at the end

		// 1) Set the gap start and gap ends for the current line

		std::cout << std::endl;
		std::cout << old_line_gap_start << std::endl;
		std::cout << new_line_gap_start << std::endl;

		int gap_buffer_size = textfile.gap_buffer.GetLines()[current_line].size();
		int gap_start = textfile.gap_buffer.GetGapStart();
		int gap_end = textfile.gap_buffer.GetGapEnd();

		// shift the gap to the end, replacing all of the gap instances with characters & characters with \0 vice versa
		for (int i = gap_end + 1; i < gap_buffer_size; i++) {
			textfile.gap_buffer.SetCharacter(current_line, gap_start, textfile.gap_buffer.GetLines()[current_line][i]);
			textfile.gap_buffer.SetCharacter(current_line, i, '\0');
			gap_start++;
			gap_end++;
		}

		// 2) Override the gap start and gap end
		textfile.gap_buffer.SetGapStart(gap_start);
		textfile.gap_buffer.SetGapEnd(gap_end);
		return;
	}
	else {
		// Otherwise, we can set the gap start to be equal to the old line gap start
		std::vector<char> temp;

		for (int i = 0; i < textfile.gap_buffer.GetLines()[current_line].size(); i++) {
			if (old_line_gap_start == i) { // if the index is where we need to shift the gap to, push null character sequence
				for (int k = 0; k < textfile.gap_buffer.GetGapSize(); k++) {
					temp.push_back('\0');
				}
				if (textfile.gap_buffer.GetLines()[current_line][i] != '\0') { // Add any potential overwritten characters (if not null)
					temp.push_back(textfile.gap_buffer.GetLines()[current_line].at(i));
				}
			}
			else { // Otherwise, just add back the character in its correct space, keeping the structure
				if (textfile.gap_buffer.GetLines()[current_line][i] != '\0') {
					temp.push_back(textfile.gap_buffer.GetLines()[current_line].at(i));
				}
			}
		}

		// Finally, set the gap start and end
		textfile.gap_buffer.SetGapStart(old_line_gap_start);
		textfile.gap_buffer.SetGapEnd(old_line_gap_start + (textfile.gap_buffer.GetGapSize() - 1));

		// Override the line (TODO: Change this to a more memory efficient solution)
		textfile.gap_buffer.SetLine(current_line, temp);
		return;
	}
}

void HandleUpDownKeys(sf::Event& e, TextFile& textfile, bool& selection_mode, bool& ctrl_pressed, sf::View& view, sf::RenderWindow& window) {
	if (e.key.code == sf::Keyboard::Down) {

		if (!ctrl_pressed) {
			user_typed_tick = true;
			user_typed_tick_counter = USER_TYPED_TICK_CD;

			int old_line = textfile.gap_buffer.GetCurrentLine();
			textfile.gap_buffer.MoveLineDown();

			if (index_mode == SaveIndex) { // If in save index mode, don't do the next steps
				return;
			}

			HandleUpAndDownIndexing(textfile, old_line);
		}
		else {
			view.move(sf::Vector2f(0.f, 20.f));

			std::cout << std::endl;
			std::cout << view.getCenter().y;
			std::cout << std::endl;
		}
		
	}

	if (e.key.code == sf::Keyboard::Up) {

		if (!ctrl_pressed) {

			user_typed_tick = true;
			user_typed_tick_counter = USER_TYPED_TICK_CD;

			int old_line = textfile.gap_buffer.GetCurrentLine();
			textfile.gap_buffer.MoveLineUp();

			if (index_mode == SaveIndex) { // If in save index mode, don't do the next steps
				return;
			}

			HandleUpAndDownIndexing(textfile, old_line);

		}
		else {

			if (view.getCenter().y == DEFAULT_SCREEN_HEIGHT / 2.f) {
				return;
			}

			std::cout << view.getCenter().y;
			std::cout << std::endl;
			std::cout << DEFAULT_SCREEN_HEIGHT / 2.f;

			view.move(sf::Vector2f(0.f, -20.f));
		}
	}
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

	if (e.key.code == sf::Keyboard::Right) {
		textfile.gap_buffer.MoveGapRight();
	}

	if (selection_mode) {
		selection_end = std::make_tuple(textfile.gap_buffer.GetCurrentLine(), textfile.gap_buffer.GetGapStart());
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
	TextFile& textfile, sf::Font& text_font, sf::Vector2f offset,std::vector<sf::Text>& line_counter,
	sf::Vector2f& line_counter_offset, sf::View& view) {
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

		// if the line has changed after pressing enter (added due some special cases)
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

			int regular_char_count = 0;

			for (int i = 0; i < previous_line_buffer.size(); i++) {
				if (previous_line_buffer[i] != '\0') {
					previous_line_text.push_back(previous_line_buffer[i]);
					regular_char_count++;
				}
			} 

			// if there are no regular characters on the new line, we can asume that no content was moved
			// Therefore, we set the string as " " (in order to be able to highlight it)
			if (regular_char_count == 0) {
				text_lines[line_before].setString(" ");
			}
			else {
				text_lines[line_before].setString(previous_line_text);
			}
		}

		// Finally, loop through all of the text lines after the new line and re-arrange their positions;
		for (int i = current_line; i < text_lines.size(); i++)
		{
			int rounded_offset_y = static_cast<int>(offset.y * (i + 1)); // Has to be multiplied by the number of line
			text_lines[i].setPosition(sf::Vector2f(rounded_offset_x, rounded_offset_y));
		}

		PopulateCountBar(line_counter, textfile, text_font, line_counter_offset);
		previous_line_selected = textfile.gap_buffer.GetCurrentLine();
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
void HighlightTypingPosition(sf::Text& current_text_line, sf::Font& font, int current_line_index, sf::RenderWindow& window, sf::View& view) {
	sf::RectangleShape rect;
	rect.setPosition(current_text_line.findCharacterPos(current_line_index).x +
		font.getGlyph('\0', 20, 0).bounds.left, current_text_line.getGlobalBounds().top +
		(current_text_line.getGlobalBounds().height + font.getGlyph('\0', 20, 0).bounds.top));
	rect.setSize(sf::Vector2f(font.getGlyph('\0', 20, 0).bounds.width, font.getGlyph('\0', 20, 0).bounds.height));

	cursor_position = rect.getPosition();
	cursor_size = rect.getSize();

	if (cursor_position != old_cursor_position) { // we save the cursor position and check whether there's been a change with its position

		// Update rectangle's position and check if it's beyond the view's bottom, moving the screen accordingly
		sf::Vector2f rectBottom = cursor_position + sf::Vector2f(0.f, cursor_size.y);
		if (rectBottom.y > view.getCenter().y + view.getSize().y / 2.f) {
			view.move(sf::Vector2f(0.f, 20.f));
		}
		else if (cursor_position.y < view.getCenter().y - view.getSize().y / 2.f) {
			view.move(sf::Vector2f(0.f, -20.f));
		}


		old_cursor_position = cursor_position;
	}


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
