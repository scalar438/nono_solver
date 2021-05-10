#include "row_solver_bf.hpp"
#include <iostream>
#include <vector>

int main()
{
	std::vector<Cell> cells(3);
	std::vector<Block> blocks;
	blocks.push_back(Block(1, 2));
	blocks.push_back(Block(2, 1));
	calculate_row_bf(cells, blocks);
	for (auto &cell : cells)
	{
		for (int i = 0; i < 3; ++i)
		{
			std::cout << i << ": " << int(cell.is_color_possible(i)) << ", ";
		}
		std::cout << '\n';
	}
}