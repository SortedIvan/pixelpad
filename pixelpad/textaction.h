#pragma once
#include <vector>
#include "SFML/Graphics.hpp"


enum TextActionType {
	TextDeletion,
	TextInput,
	NewLine,
	NONE
};

class TextAction {
private:
	TextActionType actionType;
	int line_of_action = 0;
	std::vector<char> line_affected;
	sf::Text text_line_g = sf::Text();

	// <---- Gap buffer state ----> 
	unsigned int gap_start = 0;
	unsigned int gap_end = 0;
	unsigned int gap_size = 0;

public:
	TextAction(TextActionType actionType);
	TextAction();

	TextActionType GetActionType();
	void SetActionType(TextActionType newActionType);

	int GetActionLineNr();
	void SetLineOfAction(int new_line_action);

	void SetLineAffected(std::vector<char>& line_affected);
	std::vector<char> GetLineAffected();

	void SetTextLineGraphics(sf::Text textline);
	sf::Text GetTextLineGraphics();

	void SetGapStart(int gap_start);
	int GetGapStart();

	void SetGapEnd(int gap_end);
	int GetGapEnd();

	void SetGapSize(int gap_size);
	int GetGapSize();


};

