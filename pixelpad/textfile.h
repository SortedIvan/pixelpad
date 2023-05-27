#pragma once
#include <string>
#include <vector>
#include "gapbuffer.h"

class TextFile {
	private:
		GapBuffer gap_buffer;
		std::string document_name;
		std::string document_path;
		// TODO: Add text file content
	public:
		TextFile(std::string document_name, std::string document_path);
		std::string GetDocumentName();
		std::string GetDocumentPath();
		TextFile() = default;
};