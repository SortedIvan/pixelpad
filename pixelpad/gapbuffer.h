#pragma once
#include <vector>



class GapBuffer {

	private:
		std::vector<char> content;
		int gap_start;
		int gap_end;
		int gap_size;
		int curr_index;
		int line_amount;
		bool insert_mode = false;

	public:
		GapBuffer(std::vector<char> content, int gap_start, int gap_end, int gap_size);
		GapBuffer() : gap_start(0), gap_end(0), gap_size(0), curr_index(0) {} // Default constructor initialization
		void SetGapStart(int new_position);
		void SetGapEnd(int new_position);
		void SetGapSize(int new_gap_size);

		void MoveGapLeft();
		void MoveGapRight();

		void MoveIndexLeft();
		void MoveIndexRight();

		void TurnOnInsertMode();
		void TurnOffInsertMode();

		void InsertCharacter(char character);
		void DeleteCharacter();
		
		std::vector<char> GetContent();
		void SetContent(std::vector<char> content);
		void ResizeGapMemory();
		void ResizeGapMemoryFromBack();

		int GetGapStart();
		int GetGapEnd();
		int GetGapSize();
};

