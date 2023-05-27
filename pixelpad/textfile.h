#pragma once
#include <string>
#include <vector>

class TextFile {
	private:
		std::string documentName;
		std::string documentPath;
		// TODO: Add text file content
	public:
		TextFile(std::string documentname, std::string documentpath);
		std::string GetDocumentName();
		std::string GetDocumentPath();
		TextFile() = default;
};