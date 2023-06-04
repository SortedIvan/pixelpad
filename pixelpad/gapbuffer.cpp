#include "gapbuffer.h"
#include <iostream>

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
	if (this->gap_start == 0) { // Check if moving the gap left would result in below zero
		return;
	}

	// In order to move the gap to the left, you first have to see if there are any elements that need to be copied to the right
	// Say you have the array: [a, b, c, d, _, _, _, _] where the gap is represented by _
	// if you move it one to the left, the resulting array would look like this: [a, b, c, _, _, _, _, d]
	
	content[gap_end] = content[gap_start - 1];

	this->gap_start = gap_start - 1;
	this->gap_end = gap_end - 1;

}

void GapBuffer::MoveGapRight() {
	if (this->gap_end == content.size()) { // Check if going right would exceed the size of the content array
		return;
	}

	content[gap_end + 1] = content[gap_start]; // Move the element on the right of the gap to the start of the gap

	this->gap_end = gap_end + 1;
	this->gap_start = gap_start + 1;
}


void GapBuffer::ResizeGapMemory(int gapSize, int fromIndex) {
	/*
		1) Originally, the array of chars is: [a, b, c, d, _, e ,f]
		2) User inserts a char at index 4 (the gap) [a, b, c, d, a, e ,f]
		3) The gap is now empty and the user tries to insert another char
		4) We resize the gap [a, b, c, d, a, _ , _ , _ , _, e ,f]
		5) The gap start is now the fromIndex (the index where we extend the gap from
		6) The gap end is the fromIndex + gapSize (how big the gap is)
	*/
	std::vector<char> temp;

	for (int i = 0; i < content.size() + 1; i++) {

		if (i == fromIndex) {
			for (int k = fromIndex; k < fromIndex + gapSize; k++) {
				temp.push_back(' ');
			}
		}
		else {
			temp.push_back(content[i]);
		}
	}

	this->gap_start = fromIndex;
	this->gap_end = fromIndex + gapSize;

	this->content = temp;
}

void GapBuffer::ResizeGapMemoryFromBack(int gapSize) {

	int content_size = this->content.size();
	int content_size_ = content_size + 1 + gapSize;


	for (int i = content_size + 1; i < content_size_; i++) {
		this->content.push_back(' ');
	}
}
 

void GapBuffer::InsertCharacter(char character) {
	if (this->gap_size <= 0) {
		SetGapSize(150); // If the gap is going to be 0 after char insertion, make gap bigger again
		ResizeGapMemory(150, gap_start);
	}

	try {
		// Insert character at start
		this->content.at(this->gap_start) = character;
	}
	catch (const std::exception& e) {
		std::cout << e.what(); // information from error printed
	}

	this->curr_index++; // Increment the cursor index
	this->gap_start++;
	this->gap_size--;
	this->gap_end = this->gap_start + this->gap_size;

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

int GapBuffer::GetGapStart() {
	return this->gap_start;
}

int GapBuffer::GetGapEnd() {
	return this->gap_end;
}

int GapBuffer::GetGapSize() {
	return this->gap_size;
}
