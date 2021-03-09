#include "row_solver.hpp"
#include <algorithm>
#include <set>
#include <string>

class PlaceabilityCalculator
{
public:
	PlaceabilityCalculator(const std::vector<Cell> &cells, const std::vector<Block> &blocks)
	{
		m_first_not_empty =
		    std::find_if(cells.cbegin(), cells.cend(),
		                 [](const Cell &cell) { return !cell.is_color_possible(0); }) -
		    cells.cbegin();

		m_last_not_empty =
		    std::find_if(cells.crbegin(), cells.crend(),
		                 [](const Cell &cell) { return !cell.is_color_possible(0); }) -
		    cells.crbegin();

		m_prefix_positions =
		    fill_positions(cells.begin(), cells.end(), blocks.begin(), blocks.end());

		m_suffix_positions =
		    fill_positions(cells.crbegin(), cells.rend(), blocks.rbegin(), blocks.rend());
	}

	// return true if we can place first block_index blocks on first cell_index cells
	bool can_place_prefix(size_t cell_index, size_t block_index) const
	{
		if (block_index == 0)
		{
			return cell_index <= m_first_not_empty;
		}
		else
		{
			return m_prefix_positions[block_index - 1] <= cell_index;
		}
	}

	// return true if we can place last block_index blocks on cell_index cells
	bool can_place_suffix(size_t cell_index, size_t block_index) const
	{
		if (block_index == 0)
		{
			return cell_index <= m_last_not_empty;
		}
		else
		{
			return m_suffix_positions[block_index - 1] <= cell_index;
		}
	}

private:
	size_t m_first_not_empty;
	size_t m_last_not_empty;

	std::vector<size_t> m_prefix_positions;

	std::vector<size_t> m_suffix_positions;

	template <class ItCB, class ItCE, class ItBB, class ItBE>
	static std::vector<size_t> fill_positions(ItCB cell_it, ItCE cell_it_end, ItBB block_it,
	                                          ItBE block_it_end)
	{
		std::vector<size_t> res;
		res.reserve(block_it_end - block_it);

		size_t current_cell_index = 0;

		size_t in_the_row_count = 0;
		while (cell_it != cell_it_end && block_it != block_it_end)
		{
			if (cell_it->is_color_possible(block_it->color_number))
			{
				++in_the_row_count;
			}
			else
			{
				in_the_row_count = 0;
			}
			if (in_the_row_count == block_it->block_length)
			{
				res.push_back(current_cell_index + 1);
				in_the_row_count = 0;
				++block_it;
				in_the_row_count = 0;
				current_cell_index += 2;

				++cell_it;
				if (cell_it != cell_it_end) ++cell_it;
			}
			else
			{
				++cell_it;
				++current_cell_index;
			}
		}
		return res;
	}
};

std::vector<size_t> calculate_row(std::vector<Cell> &cells, std::vector<Block> &blocks)
{
	const size_t n = cells.size();
	if (n == 0) return {};

	while (1)
	{
		PlaceabilityCalculator pc(cells, blocks);

		std::vector<Cell> new_cell_list;
		{
			Cell val;
			val.set_impossible();
			new_cell_list.assign(n, val);
		}

		const size_t blocks_count = blocks.size();
		// Calculate empty cells
		for (size_t i = 0; i != n; ++i)
		{
			if (cells[i].is_color_possible(0))
			{
				bool can_place = false;
				for (size_t j = 0; j <= blocks_count; ++j)
				{
					if (pc.can_place_prefix(i, j) &&
					    pc.can_place_suffix(n - i - 1, blocks_count - j))
					{
						can_place = true;
						break;
					}
				}
				if (!can_place)
				{
					cells[i].set_color_possible(0, false);
				}
			}
		}

		for (size_t k = 0; k != blocks_count; ++k)
		{
			size_t in_the_row_count = 0;
			for (size_t i = 0; i != n; ++i)
			{
				if (cells[i].is_color_possible(blocks[k].color_number))
				{
					in_the_row_count += 1;
				}
				else
				{
					in_the_row_count = 0;
				}
				if (in_the_row_count < blocks[k].block_length) continue;

				if (k != 0 && i - blocks[k].block_length <= n &&
				    !cells[i - blocks[k].block_length].is_color_possible(0))
					continue;
				if (k != blocks_count - 1 && i + 1 != n && !cells[i + 1].is_color_possible(0))
					continue;

				size_t delta_prefix = k == 0 ? 0 : 1;
				size_t delta_suffix = k == blocks_count - 1 ? 0 : 1;

				if (pc.can_place_prefix(i + 1 - blocks[k].block_length - delta_prefix, k) &&
				    pc.can_place_suffix(n - (i + delta_suffix), blocks_count - k - 1))
				{
					for (size_t j = i - blocks[k].block_length + 1; j <= i; ++j)
					{
						new_cell_list[j].set_color_possible(blocks[k].color_number, true);
					}
					if (i - blocks[k].block_length < n)
						new_cell_list[i - blocks[k].block_length].set_color_possible(0, true);
					if (i + 1 < n) new_cell_list[i + 1].set_color_possible(0, true);
				}
			}
		}
		bool all_equal = true;
		for (size_t i = 0; i != n; ++i)
		{
			if (cells[i] != new_cell_list[i]) all_equal = false;
			cells[i] &= new_cell_list[i];
		}
		if (all_equal) break;
	}

	std::vector<size_t> result;
	return result;
}