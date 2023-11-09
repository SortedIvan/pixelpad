#include "textaction.h"

TextAction::TextAction(TextActionType actionType) {
	this->actionType = actionType;
}

TextActionType TextAction::GetActionType() {
	return this->actionType;
}

void TextAction::ChangeActionType(TextActionType newTextAction) {
	this->actionType = newTextAction;
}

int TextAction::GetLineOfAction() {
	return this->line_of_action;
}

void TextAction::ChangeLineOfAction(int new_line_action) {
	this->line_of_action = new_line_action;
}

void TextAction::SetLineAffected(std::vector<char>& line_affected) {
	this->line_affected = line_affected;
}