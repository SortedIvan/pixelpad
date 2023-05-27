#pragma once
#include <vector>



class GapBuffer {

	private:
		std::vector<char> content;
		int gap_start;
		int gap_end;
		int gap_size;

	public:
		void SetGapStart(int new_position);
		void SetGapEnd(int new_position);
		void SetGapSize(int new_gap_size);

		void MoveGapLeft();
		void MoveGapRight();

		std::vector<char> GetContent();

};

