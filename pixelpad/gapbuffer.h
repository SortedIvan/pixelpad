#pragma once
#include <vector>

/*
	Pixelpad initially used a regular gap buffer with some modifications to keep the total line count, 
	the current line and newline characters's positions.
	However, due to pixelpad being a prototype, using a more complex ds such as ropes or data tables, space complexity is not a big issue, 
	as long as it is time efficient. Thus, for those reasons, pixelpad's current implementation of a gap buffer changed to the original.

	Pixelpad uses a collection of gap buffers, all of which have their gap start, gap end and gap size. 
	This allows for each line to be represented as its own gap buffer, making it extremely efficient,
	especially when used for regular text writing.
*/

class GapBuffer {

private:
	std::vector<std::vector<char>> lines; // Contains each seperate "gap buffer" which runs parallel with the vectors bellow
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
	void InsertCharacterManually(int line_index, char character);
	void DeleteCharacter();
	void DeleteLineManually(int line_count);

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

