#include "textfile.h"
#include <string>

TextFile::TextFile(std::string documentname, std::string documentpath) {
	this->documentName = documentname;
	this->documentPath = documentpath;
}

std::string TextFile::GetDocumentName() {
	return documentName;
}

std::string TextFile::GetDocumentPath() {
	return documentPath;
}
