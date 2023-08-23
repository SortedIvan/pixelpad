#include "textfile.h"
#include <string>
#include <iostream>
#include <fstream>

const int GAP_RESIZE_BY = 200;

TextFile::TextFile(std::string document_name, std::string document_path) {
	this->document_name = document_name;
	this->document_path = document_path;
	
	//std::vector<char> content_empty;
	//std::ifstream file(document_path, std::ios::binary); // Open the file to read its content after

	//if (!file) { // If the file is empty
	//	gap_buffer.SetContent(content_empty);  // Set to empty content
	//	gap_buffer.SetGapSize(GAP_RESIZE_BY); // Set the gap size to the predefined value TODO: Change to dynamic value
	//	gap_buffer.ResizeGapMemory(gap_buffer.GetGapSize(), 0); // Populate the array with the gap size from index 0
	//	this->gap_buffer = gap_buffer;
	//	return;
	//}

	//std::vector<char> content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // Read the content

	//std::vector<char> content = { 'a','b','c' };

	std::vector<char> content {
		'p', 'e', 't', 'e', 'r', ' ', 'p', 'i', 'p', 'e', 'r', ' ', 'p', 'i', 'c', 'k', 'e', 'd', ' ', 'a', ' ', 'p', 'e', 'c', 'k', ' ', 'o', 'f', ' ', 'p', 'i', 'c', 'k', 'l', 'e', 'd', ' ', 'p', 'e', 'p', 'p', 'e', 'r', 's',
			'\n', 'a', 'b', 'c', 'd',
			'\n', 'a',
			'\n', 'o'};

	GapBuffer gap_buffer(content);
	this->gap_buffer = gap_buffer;
}
 

std::string TextFile::GetDocumentName() {
	return document_name;
}

std::string TextFile::GetDocumentPath() {
	return document_path;
}

GapBuffer TextFile::GetGapBuffer() {
	return this->gap_buffer;
}

