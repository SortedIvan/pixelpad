#include "textaction.h"

TextAction::TextAction(TextActionType actionType) {
	this->actionType = actionType;
}

TextAction::TextAction() {

}

TextActionType TextAction::GetActionType() {
	return this->actionType;
}

void TextAction::SetActionType(TextActionType newTextAction) {
	this->actionType = newTextAction;
}

int TextAction::GetActionLineNr() {
	return this->line_of_action;
}

void TextAction::SetLineOfAction(int new_line_action) {
	this->line_of_action = new_line_action;
}

void TextAction::SetLineAffected(std::vector<char>& line_affected) {
	this->line_affected = line_affected;
}

std::vector<char> TextAction::GetLineAffected(){
	return line_affected;
}

void TextAction::SetTextLineGraphics(sf::Text textline) {
	this->text_line_g = textline;
}

sf::Text TextAction::GetTextLineGraphics() {
	return this->text_line_g;
}

void TextAction::SetGapStart(int gap_start) {
	this->gap_start = gap_start;
}

int TextAction::GetGapStart() {
	return this->gap_start;
}

void TextAction::SetGapEnd(int gap_end) {
	this->gap_end = gap_end;
}

int TextAction::GetGapEnd() {
	return this->gap_end;
}

void TextAction::SetGapSize(int gap_size) {
	this->gap_size = gap_size;
}

int TextAction::GetGapSize() {
	return this->gap_size;
}