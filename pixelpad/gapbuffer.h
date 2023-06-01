#pragma once
#include <vector>



class GapBuffer {

	private:
		std::vector<char> content;
		int gap_start;
		int gap_end;
		int gap_size;
		int curr_index;


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

		void InsertCharacter(char character);
		
		std::vector<char> GetContent();
		void SetContent(std::vector<char> content);
		void ResizeGapMemory(int gapSize, int fromIndex);
		void ResizeGapMemoryFromBack(int gapSize);

		int GetGapStart();
		int GetGapEnd();
		int GetGapSize();
};

