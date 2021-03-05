#include "row_solver_bf.hpp"

namespace
{
void f(const std::vector<Cell> &cells, const std::vector<Block> &blocks, size_t current_cell_index,
       size_t current_block_index, std::vector<size_t> &blocks_pos, std::vector<Cell> &result_cells)
{
	if (current_block_index == blocks.size())
	{
		std::vector<size_t> empty_cells;
		// Check blocks placement
		for (size_t i = 0, e = (blocks_pos.size() == 0 ? cells.size() : blocks_pos[0]); i != e; ++i)
		{
			if (!cells[i].is_color_possible(0))
			{
				return;
			}
			empty_cells.push_back(i);
		}
		for (size_t i = 1; i != current_block_index; ++i)
		{
			for (size_t j = blocks_pos[i - 1] + blocks[i].block_length; j != blocks_pos[i]; ++j)
			{
				if (!cells[j].is_color_possible(0))
				{
					return;
				}
				empty_cells.push_back(j);
			}
		}
		while (current_cell_index != cells.size())
		{
			if (!cells[current_cell_index].is_color_possible(0))
			{
				return;
			}
			empty_cells.push_back(current_cell_index);
			++current_cell_index;
		}
		// If we are here, blocks can be placed. Write possibilities
		for (size_t i = 0; i != blocks_pos.size(); ++i)
		{
			for (int j = 0; j != blocks[i].block_length; ++j)
			{
				result_cells[blocks_pos[i] + j].set_color_possible(blocks[i].color_number, true);
			}
		}
		for (size_t index : empty_cells)
		{
			result_cells[index].set_color_possible(0, true);
		}
		return;
	}
	// If current block is not first, we have to add one empty cell
	if (current_block_index != 0)
	{
		++current_cell_index;
	}

	size_t in_the_row_count = 0;
	while (current_cell_index != cells.size())
	{
		if (cells[current_cell_index].is_color_possible(blocks[current_block_index].color_number))
		{
			++in_the_row_count;
		}
		else
		{
			in_the_row_count = 0;
		}
		if (in_the_row_count >= blocks[current_block_index].block_length)
		{
			// We can place the block. Save position and move farther
			blocks_pos[current_block_index] =
			    current_cell_index - blocks[current_block_index].block_length;
			f(cells, blocks, current_cell_index + 1, current_block_index + 1, blocks_pos,
			  result_cells);
		}
		++current_cell_index;
	}
}
} // namespace

std::vector<size_t> calculate_row_bf(std::vector<Cell> &cells, std::vector<Block> &blocks)
{
	std::vector<size_t> blocks_pos(blocks.size());
	std::vector<Cell> result_cells;
	{
		Cell cell_template;
		cell_template.set_impossible();
		result_cells.assign(cells.size(), cell_template);
	}
	f(cells, blocks, 0, 0, blocks_pos, result_cells);
	cells.swap(result_cells);

	std::vector<size_t> result;
	return result;
}