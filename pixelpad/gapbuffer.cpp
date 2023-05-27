#include "gapbuffer.h"

void GapBuffer::SetGapEnd(int new_position) {
	this->gap_end = new_position;
}

void GapBuffer::SetGapStart(int new_position) {
	this->gap_start = new_position;
}

void GapBuffer::SetGapSize(int new_size) {
	this->gap_size = new_size;
}

void GapBuffer::MoveGapLeft() {
	if (this->gap_start - 1 < 0) { // Check if moving the gap left would result in below zero
		return;
	}

	// In order to move the gap to the left, you first have to see if there are any elements that need to be copied to the right
	// Say you have the array: [a, b, c, d, _, _, _, _] where the gap is represented by _
	// if you move it one to the left, the resulting array would look like this: [a, b, c, _, _, _, _, d]

	this->content.push_back(this->content.at(gap_start - 1)); // Push the character on the left of the gap to the end
	this->gap_start = gap_start - 1;
	this->gap_end = gap_end - 1;
}

void GapBuffer::MoveGapRight() {
	if (this->gap_end + 1 >= this->content.size()) { // Check if going right would exceed the size of the content array
		return;
	}

	content[gap_start] = content[gap_end + 1]; // Move the element on the right of the gap to the start of the gap
	this->gap_end = gap_end + 1;
	this->gap_start = gap_start + 1;
}

