#pragma once
#include <string>
#include <vector>
#include "gapbuffer.h"
class TextFile {
	private:
		std::string document_name;
		std::string document_path;

		// TODO: Add text file content
	public:
		GapBuffer gap_buffer;
		TextFile(std::string document_name, std::string document_path);
		std::string GetDocumentName();
		std::string GetDocumentPath();
		GapBuffer GetGapBuffer();
		TextFile() = default;
};