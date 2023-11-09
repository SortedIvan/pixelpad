#pragma once
#include <vector>

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

public:
	TextAction(TextActionType actionType);
	TextActionType GetActionType();
	void ChangeActionType(TextActionType newActionType);
	int GetLineOfAction();
	void ChangeLineOfAction(int new_line_action);
	void SetLineAffected(std::vector<char>& line_affected);



};

