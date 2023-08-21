#pragma once
#include <vector>



class GapBuffer {

private:
	std::vector<std::vector<char>> lines;
	std::vector<int> gap_starts;
	std::vector<int> gap_ends;
	std::vector<int> gap_sizes;
	std::vector<int> newline_positions;
	int current_line = 0;

public:
	GapBuffer(const std::vector<char>& content);
	GapBuffer(); // Default constructor initialization
	void SetGapStart(int new_position);
	void SetGapEnd(int new_position);
	void SetGapSize(int new_gap_size);

	void MoveGapLeft();
	void MoveGapRight();
	void MoveLineUp();
	void MoveLineDown();

	void InsertNewLine();


	std::vector<std::vector<char>> GetLines();
	void SetLine(int line_nr, const std::vector<char>& content);
	void SetCharacter(int line_nr, int index, char character);


	void InsertCharacter(char character);
	void DeleteCharacter();

	void FillBuffersFromContent(const std::vector<char>& content);
	void ResizeGapMemory();
	void ResizeGapMemoryFromBack();

	int GetCurrentLine();
	void SetCurrentLine(int new_current_line);

	int GetGapStart();
	int GetGapEnd();
	int GetGapSize();

	std::vector<int> GetGapSizes();
	std::vector<int> GetGapStarts();
	std::vector<int> GetGapEnds();
};

