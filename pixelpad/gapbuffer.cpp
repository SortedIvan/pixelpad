#include "gapbuffer.h"
#include <iostream>

const int GAP_RESIZE_BY = 3;

GapBuffer::GapBuffer(std::vector<char> content, int gap_start, int gap_end, int gap_size) {
	this->content = content;
	this->gap_start = gap_start;
	this->gap_end = gap_end;
	this->gap_size = gap_size;
	this->curr_index = 0;

}

void GapBuffer::SetGapEnd(int new_position) {
	this->gap_end = new_position;
}

void GapBuffer::SetGapStart(int new_position) {
	this->gap_start = new_position;
}

void GapBuffer::SetGapSize(int new_size) {
	this->gap_size = new_size;
}

std::vector<char> GapBuffer::GetContent() {
	return content;
}

void GapBuffer::SetContent(std::vector<char> content) {
	this->content = content;
}

void GapBuffer::MoveGapLeft() { 
	// In order to move the gap to the left, you first have to see if there are any elements that need to be copied to the right
	// Say you have the array: [a, b, c, d, _, _, _, _] where the gap is represented by _
	// if you move it one to the left, the resulting array would look like this: [a, b, c, _, _, _, _, d]

	if (gap_end == gap_start && gap_size == 1) { // if there's only 1 space left
		if (gap_start != 0) {
			content.at(gap_start) = content.at(gap_start - 1);
			gap_start--;
			gap_end--;
			return;
		}
	}
	
	if (gap_start != 0) {
		content.at(gap_end) = content.at(gap_start - 1);
		gap_start--;
		gap_end--;
	}
}

// TODO: Fix this, it does not work by 1 index (have to tap twice)
void GapBuffer::MoveGapRight() {
	if (gap_end != content.size() - 1) // the gap end is not located at the end 
	{
		content.at(gap_start) = content.at(gap_end + 1);
		gap_end++;
		gap_start++;
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
	
	for (int i = 0; i < content.size(); i++) {
		if (gap_start == i) { // if the index is where we need to expand, make the gap bigger
			for (int k = 0; k < GAP_RESIZE_BY; k++) {
				temp.push_back('\0');
			}
			temp.push_back(content.at(i)); // finally, add the overwritten character from the gap
		}
		else {
			temp.push_back(content.at(i)); // Otherwise, push the character itself
		}
	}

	this->content = temp;
}

void GapBuffer::ResizeGapMemoryFromBack() {
	for (int i = gap_start; i < gap_end + 1; i++) {
		this->content.push_back('\0');
	}
}

void GapBuffer::InsertCharacter(char character) {
	if (gap_size == 1) {
		if (gap_start == content.size() - 1) { // When the gap_start is located at the end and the size == 1, we resize from back
			content.at(gap_start) = character; // Set the character at the gap_start
			gap_size--; // Make the gap size equal to 0
			SetGapSize(GAP_RESIZE_BY); // Resize the gap size by the necessary amount (TODO: Make dynamic value)
			gap_start = gap_start + 1; // Resize the gap start by 1
			gap_end = gap_end + gap_size; // Set the gap end to the correct value
			ResizeGapMemoryFromBack(); // Finally, resize from the back of the buffer
			return;
		}
		else {
			content.at(gap_start) = character; // Set the character at the gap_start
			gap_size--; // Make the gap size equal to 0
			SetGapSize(GAP_RESIZE_BY); // Resize the gap size by the necessary amount (TODO: Make dynamic value)
			gap_start = gap_start + 1; // Resize the gap start by 1
			gap_end = gap_end + gap_size; // Set the gap end to the correct value
			ResizeGapMemory();
			return;
		}
	}

	try {
		// Insert character at start
		this->content.at(this->gap_start) = character;
	}
	catch (const std::exception& e) {
		std::cout << e.what(); // information from error printed
	}

	this->gap_start++;
	this->gap_size--;
}

void GapBuffer::DeleteCharacter() {
	if (gap_start != 0) {
		content.at(gap_start - 1) = '\0';
		gap_start--;
		gap_size++;
	}
}


void GapBuffer::MoveIndexLeft() {
	if (curr_index == 0) {
		return;
	}

	curr_index--;
}

void GapBuffer::MoveIndexRight() {
	if (curr_index == content.size() - 1) {
		return;
	}

	curr_index++;
}

void GapBuffer::TurnOffInsertMode() {
	this->insert_mode = false;
}

void GapBuffer::TurnOnInsertMode() {
	this->insert_mode = true;
}

int GapBuffer::GetGapStart() {
	return this->gap_start;
}

int GapBuffer::GetGapEnd() {
	return this->gap_end;
}

int GapBuffer::GetGapSize() {
	return this->gap_size;
}
