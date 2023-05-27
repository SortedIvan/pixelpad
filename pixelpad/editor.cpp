#include "editor.h"


int Editor::OpenEditor(std::string filename, std::string filepath) 
{




	// Use one method for handling the opening of files? Can add an explicit check if there are more than 1 files opened, if yes -> don't open a new window
	// 1) First open the file at the filepath and read its contents
	// 1.2) Create a text file object and add it to the array of other text objects
	// 2) Initialize a gap buffer and add the content into a char array
	// 3) 
}


//int main() {
//	sf::RenderWindow window(sf::VideoMode(800, 800), "Text");
//	sf::Event e;
//
//	while (window.isOpen()) {
//		while (window.pollEvent(e)) {
//			if (e.type == sf::Event::Closed) {
//				window.close();
//			}
//		}
//	}
//	std::cout << "This is a test" << std::endl;
//	return 0;
//}