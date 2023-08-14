#include "gapbuffer.h"
#include <iostream>

void PrintGapDebug(std::vector<int>& gap_starts, std::vector<int>& gap_ends, std::vector<int>& gap_sizes, int& current_line);
static const int GAP_SIZE = 3;

GapBuffer::GapBuffer(const std::vector<char>& content) {
	FillBuffersFromContent(content);
	current_line = 0;
}

GapBuffer::GapBuffer() {

}

std::vector<std::vector<char>> GapBuffer::GetLines() {
	return this->lines;
}

void GapBuffer::SetGapStart(int new_position) {
	gap_starts[current_line] = new_position;
}

void GapBuffer::SetGapEnd(int new_position) {
	gap_ends[current_line] = new_position;
}

void GapBuffer::SetGapSize(int new_gap_size) {

}

void GapBuffer::MoveGapLeft() {
	if (gap_starts[current_line] != 0) {
		// The character behind the gap start goes at the end of the gap
		lines.at(current_line).at(gap_ends[current_line]) = lines.at(current_line).at(gap_starts[current_line] - 1);
		lines.at(current_line).at(gap_starts[current_line] - 1) = '\0'; // after copying the char, we set it to '\0', virtually "moving" the gap
		gap_starts[current_line]--;
		gap_ends[current_line]--;
		return;
	}

	// if the gap is located at the very end, [_,_,_,a,b,c]
	//										   ^--GAP_START
	// moving left would require to go up a line and shift
	// its gap to the end
	if (gap_starts[current_line] == 0 && current_line != 0) {
		current_line -= 1;

		// Remove the gap sequence from its current position (for the line above)
		lines[current_line].erase(lines[current_line].begin() + gap_starts[current_line], lines[current_line].begin() + gap_ends[current_line] + 1);

		// Insert the gap sequence at the end of the vector
		for (int i = 0; i < gap_sizes[current_line]; ++i) {
			lines[current_line].push_back('\0');
		}

		// Set the new gap start & end
		gap_starts[current_line] = lines[current_line].size() - gap_sizes[current_line];
		gap_ends[current_line] = gap_starts[current_line] + (gap_sizes[current_line] - 1);
	}

}

void GapBuffer::MoveGapRight() {
	if (gap_ends[current_line] != lines.at(current_line).size() - 1) { // Move the gap normally to the right, gap is not at the end
		lines[current_line].at(gap_starts[current_line]) = lines[current_line].at(gap_ends[current_line] + 1);
		lines[current_line].at(gap_ends[current_line] + 1) = '\0'; // make the last character part of the gap since it is shifted
		gap_ends[current_line]++;
		gap_starts[current_line]++;
		return;
	}
	// if the gap is located at the end and the current line selected is not the last line available
	if (gap_ends[current_line] == lines.at(current_line).size() - 1 && current_line != lines.size() - 1) {
		current_line += 1; // go up a line

		// Remove the gap sequence from its position (for the line below)
		lines[current_line].erase(lines[current_line].begin() + gap_starts[current_line], lines[current_line].begin() + gap_ends[current_line] + 1);

		std::vector<char> temp_line;
		// push the gap at the start of the temp line
		for (int i = 0; i < gap_sizes[current_line]; ++i) {
			temp_line.push_back('\0');
		}

		// push the rest of the characters into the temp line
		for (int i = 0; i < lines[current_line].size(); i++) {
			temp_line.push_back(lines[current_line].at(i));
		}

		lines[current_line] = temp_line; // Finally, assign the temp line (TODO: Make this without a temporary vector)

		gap_starts[current_line] = 0; // begining of the line
		gap_ends[current_line] = gap_starts[current_line] + (gap_sizes[current_line] - 1);
	}
}

void GapBuffer::InsertCharacter(char character) {
	if (gap_sizes[current_line] == 1)
	{
		if (gap_starts[current_line] == lines[current_line].size() - 1) {
			lines[current_line].at(gap_starts[current_line]) = character;
			gap_sizes[current_line] = GAP_SIZE;
			gap_starts[current_line] += 1;
			gap_ends[current_line] += GAP_SIZE;
			ResizeGapMemoryFromBack();
			return;
		}
		else {
			lines[current_line].at(gap_starts[current_line]) = character; // Set the character at the gap_start
			gap_sizes[current_line] = GAP_SIZE;
			gap_starts[current_line] += 1; // Resize the gap start by 1
			gap_ends[current_line] += GAP_SIZE;; // Set the gap end to the correct value
			ResizeGapMemory();
			return;
		}
	}

	try
	{
		// Insert character at start
		this->lines[current_line].at(gap_starts[current_line]) = character;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what(); // information from error printed
	}

	gap_starts[current_line]++;
	gap_sizes[current_line]--;


	//std::cout << gap_sizes[current_line];
}

void GapBuffer::DeleteCharacter()
{
	if (gap_starts[current_line] != 0) // Normal deletion of a character without overflow of lines
	{
		lines[current_line].at(gap_starts[current_line] - 1) = '\0';
		gap_starts[current_line]--;
		gap_sizes[current_line]++;
		return;
	}
	else if (gap_starts[current_line] == 0)
	{
		/* SCENARIO:
		*  line 0: a,b,c,d,_,_,'\n';
		*  line 1: _,_,x,y,z,'\n'; <--- CURRENT_LINE
		*  AFTER DELETE CHAR -> a,b,c,d,_,_,_,x,y,z,'\n';
		*/

		if (current_line != 0) {
			int old_line = current_line;
			current_line -= 1;

			// Remove the gap sequence from its current position (for the line above)
			lines[current_line].erase(lines[current_line].begin() + gap_starts[current_line], lines[current_line].begin() + gap_ends[current_line] + 1);

			// Insert the gap sequence at the end of the vector
			for (int i = 0; i < gap_sizes[current_line]; ++i) {
				lines[current_line].push_back('\0');
			}

			// Set the new gap start & end
			gap_starts[current_line] = lines[current_line].size() - gap_sizes[current_line];
			gap_ends[current_line] = gap_starts[current_line] + (gap_sizes[current_line] - 1);

			// delete the gap measurements for the old line
			gap_starts.erase(gap_starts.begin() + old_line);
			gap_ends.erase(gap_ends.begin() + old_line);
			gap_sizes.erase(gap_sizes.begin() + old_line);


			// Push all of the characters from the previous line (if any)
			for (int i = 0; i < lines[old_line].size(); i++) {

				if (lines[old_line][i] != '\0') {
					lines[current_line].push_back(lines[old_line].at(i));
				}
			}

			// Finally, delete the old line
			lines.erase(lines.begin() + old_line);

			return;
		}
	}

	//PrintGapDebug(gap_starts, gap_ends, gap_sizes, current_line);

}

void GapBuffer::FillBuffersFromContent(const std::vector<char>& content) {
	std::vector<std::vector<char>> temp_lines;
	for (int i = 0; i < content.size(); i++)
	{
		if (content.at(i) == '\n')
		{
			current_line++;

			if (current_line >= temp_lines.size()) {
				temp_lines.push_back(std::vector<char>()); // Add a new empty line if needed
			}

			continue;
		}

		if (current_line >= temp_lines.size()) {
			temp_lines.push_back(std::vector<char>()); // Add a new empty line if needed
		}
		temp_lines[current_line].push_back(content[i]);
	}

	// fill the gap_buffer_lines, gap_starts and gap_ends & newline_positions
	for (int i = 0; i < temp_lines.size(); i++)
	{
		lines.push_back(std::vector<char>());
		gap_starts.push_back(temp_lines[i].size()); // indicate the gap start
		gap_ends.push_back(temp_lines[i].size() + (GAP_SIZE - 1));
		gap_sizes.push_back(GAP_SIZE);

		// fill in the actual content
		for (int k = 0; k < temp_lines[i].size(); k++)
		{
			lines[i].push_back(temp_lines[i][k]);
		}

		// fill in the gap
		for (int j = 0; j < GAP_SIZE; j++) {
			lines[i].push_back('\0');
		}

		newline_positions.push_back(temp_lines[i].size()); // set the new_line to be at the end of the
	}

}

void GapBuffer::ResizeGapMemory() {
	/*
	1) Originally, the array of chars is: [a, b, c, d, _, e ,f]
	2) User inserts a char at index 4 (the gap) [a, b, c, d, a,e <- gap_start here  ,f]
	3) The gap is now empty and the user tries to insert another char (the gap is now located at position 5)
	4) We resize the gap [a, b, c, d, a, _ , _ , _ , _, e ,f]
	5) The gap start is now the fromIndex (the index where we extend the gap from
	6) The gap end is the fromIndex + gapSize (how big the gap is)
*/
	std::vector<char> temp;

	for (int i = 0; i < lines[current_line].size(); i++) {
		if (gap_starts[current_line] == i) { // if the index is where we need to expand, make the gap bigger
			for (int k = 0; k < GAP_SIZE; k++) {
				temp.push_back('\0');
			}
			temp.push_back(lines[current_line].at(i)); // finally, add the overwritten character from the gap
		}
		else {
			temp.push_back(lines[current_line].at(i)); // Otherwise, push the character itself
		}
	}

	this->lines[current_line] = temp;
	this->newline_positions[current_line] = this->lines[current_line].size(); // Set the new newline character to be always at the end

}

void GapBuffer::ResizeGapMemoryFromBack() {

	for (int i = gap_starts[current_line]; i < gap_ends[current_line] + 1; i++) {
		this->lines[current_line].push_back('\0');
	}
	this->newline_positions[current_line] = this->lines[current_line].size(); // Set the new newline character to be always at the end
}

void GapBuffer::InsertNewLine() {
	if (current_line == lines.size() - 1) { // if the current_line is located at the end of the lines
		current_line += 1;

		lines.push_back(std::vector<char>()); // Push a new vector for the new line
		gap_starts.push_back(0); // Push gap start as 0 of the sequence
		gap_ends.push_back(GAP_SIZE - 1); // Push new gap end
		gap_sizes.push_back(GAP_SIZE); // Gap size;
		newline_positions.push_back(GAP_SIZE); // Put a newline character at the end of the sequence

		for (int i = 0; i < gap_sizes[current_line]; i++) {
			lines[current_line].push_back('\0');
		}

		return;
	}
	if (current_line != lines.size() - 1) {

		int previous_line = current_line;
		bool content_after_gap = false;

		if (gap_ends[current_line] != lines[current_line].size() - 1) {
			content_after_gap = true;
		}

		current_line += 1;

		lines.insert(lines.begin() + current_line, std::vector<char>());
		gap_starts.insert(gap_starts.begin() + current_line, 0);
		gap_ends.insert(gap_ends.begin() + current_line, GAP_SIZE - 1);
		gap_sizes.insert(gap_sizes.begin() + current_line, GAP_SIZE);

		for (int i = 0; i < gap_sizes[current_line]; i++) {
			lines[current_line].push_back('\0');
		}
		
		if (content_after_gap) {

			int gap_increase = 0;

			for (int i = gap_ends[previous_line] + 1; i < lines[previous_line].size(); i++) {
				lines[current_line].push_back(lines[previous_line][i]);
				lines[previous_line][i] = '\0';
				gap_increase++;
			}

			gap_ends[previous_line] = lines[previous_line].size() - 1;
			gap_sizes[previous_line] = gap_sizes[previous_line] + gap_increase;
			newline_positions[previous_line] = gap_ends[previous_line] + 1;
		}

		//----

		newline_positions.insert(newline_positions.begin() + current_line, GAP_SIZE);

		std::cout << std::endl;
	}


}

void GapBuffer::MoveLineUp() {

	// if the line is located at the begining
	if (current_line == 0) {
		return;
	}

	current_line -= 1;
}

void GapBuffer::MoveLineDown() {
	// if the line is located at the end
	if (current_line == lines.size() - 1) {
		return;
	}

	current_line += 1;

	//PrintGapDebug(gap_starts, gap_ends, gap_sizes, current_line);
}


int GapBuffer::GetGapStart() {
	return gap_starts[current_line];
}

int GapBuffer::GetGapEnd() {
	return gap_ends[current_line];
}

int GapBuffer::GetGapSize() {
	return gap_sizes[current_line];
}

int GapBuffer::GetCurrentLine() {
	return current_line;
}

void GapBuffer::SetCurrentLine(int new_current_line) {
	this->current_line = new_current_line;
}


std::vector<int> GapBuffer::GetGapSizes() {
	return gap_sizes;
}

std::vector<int> GapBuffer::GetGapStarts() {
	return gap_starts;
}

std::vector<int> GapBuffer::GetGapEnds() {
	return gap_ends;
}


void PrintGapDebug(std::vector<int>& gap_starts, std::vector<int>& gap_ends, std::vector<int>& gap_sizes, int& current_line) {
	std::cout << std::endl;
	std::cout << gap_starts[current_line] << " - GAP START" << std::endl;
	std::cout << gap_ends[current_line] << " - GAP END" << std::endl;
	std::cout << gap_sizes[current_line] << " - GAP SIZE" << std::endl;
	std::cout << current_line << " - CURRENT LINE" << std::endl;
}