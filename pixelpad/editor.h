#pragma once
#include <string>
#include <vector>
#include "textfile.h"
#include "SFML/Graphics.hpp"

class Editor {
	private:
		std::vector<TextFile>* textfiles = new std::vector<TextFile>();
	public:
		int OpenFile(std::string filename);

	
		
};