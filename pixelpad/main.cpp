#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "SFML/Graphics.hpp"

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Text");
	sf::Event e;

	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
	}
	std::cout << "This is a test" << std::endl;
	return 0;
}