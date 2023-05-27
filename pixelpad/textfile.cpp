#include "textfile.h"
#include <string>
#include <iostream>
#include <fstream>


TextFile::TextFile(std::string document_name, std::string document_path) {
	this->document_name = document_name;
	this->document_path = document_path;
	
	GapBuffer gap_buffer;
	std::vector<char> content;
	std::ifstream file(document_path, std::ios::binary); // Open the file to read its content after

	if (!file) {
		gap_buffer.SetContent(content);  // Set to empty content
		gap_buffer.SetGapStart(0);
		gap_buffer.SetGapEnd(0);
		gap_buffer.SetGapSize(150);

		this->gap_buffer = gap_buffer;
		return;
	}

	std::vector<char> content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // Read the content
	int content_size = content.size(); // Calculate once for more performance

	gap_buffer.SetContent(content);
	gap_buffer.SetGapSize(150);
	gap_buffer.SetGapStart(content_size);
	gap_buffer.SetGapEnd(content_size + 150);

	this->gap_buffer = gap_buffer;
}

std::string TextFile::GetDocumentName() {
	return document_name;
}

std::string TextFile::GetDocumentPath() {
	return document_path;
}
