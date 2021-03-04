#include "row_solver.hpp"
#include "reversed_vector.hpp"
#include <set>
#include <string>

template <class TCell, class TBlock, class TResult>
void calc_blocks_placeability(const TCell &cells, const TBlock &blocks, std::vector<TResult> &res)
{
	const size_t n = cells.size();
	if (n == 0) return;
	const size_t k = blocks.size();

	res.resize(k + 1);
	res[0].resize(n + 1);
	res[0][0]      = true;
	bool cur_value = true;
	for (size_t i = 0; i != n; ++i)
	{
		if (cur_value && !cells[i].is_color_possible(0)) cur_value = false;
		res[0][i + 1] = cur_value;
	}
	for (size_t i = 0; i != k; ++i)
	{
		res[i + 1].resize(n + 1);
		res[i + 1][0] = false;

		int current_allowed_count = 0;

		int delta = i == 0 ? 1 : 0;

		cur_value = false;

		for (size_t j = 0; j < n; ++j)
		{
			if (cells[j].is_color_possible(blocks[i].first))
				current_allowed_count += 1;
			else
				current_allowed_count = 0;
			if (!cur_value && j - blocks[i].second + delta <= n &&
			    current_allowed_count >= blocks[i].second && res[i][j - blocks[i].second + delta])
				cur_value = true;
			res[i + 1][j + 1] = cur_value;
		}
	}
}

std::vector<size_t> calculate_row(std::vector<Cell> &cells,
                                  std::vector<std::pair<int, size_t>> &blocks)
{
	const size_t n = cells.size();
	if (n == 0) return {};

	while (1)
	{
		// can_place_prifix[i][j] == true if we can place first i blocks at the first j cells
		// can_place_suffix - same, but for last i blocks and last n - j cells
		std::vector<std::vector<bool>> can_place_prefix, can_place_suffix;

		calc_blocks_placeability(cells, blocks, can_place_prefix);
		{
			std::vector<ReversedVector<bool>> can_place_suffix_rev;
			calc_blocks_placeability(ReversedVector(cells), ReversedVector(blocks),
			                         can_place_suffix_rev);
			for (auto &rev_vector : can_place_suffix_rev)
				can_place_suffix.emplace_back(rev_vector.move_out());
		}

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
					if (can_place_prefix[j][i] && can_place_suffix[blocks_count - j][i + 1])
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
			int in_the_row_count = 0;
			for (size_t i = 0; i != n; ++i)
			{
				if (cells[i].is_color_possible(blocks[k].first))
				{
					in_the_row_count += 1;
				}
				else
				{
					in_the_row_count = 0;
				}
				if (in_the_row_count < blocks[k].second) continue;

				if (k != 0 && i - blocks[k].second <= n &&
				    !cells[i - blocks[k].second].is_color_possible(0))
					continue;
				if (k != blocks_count - 1 && i + 1 != n && !cells[i + 1].is_color_possible(0))
					continue;

				size_t delta_prefix = k == 0 ? 0 : 1;
				size_t delta_suffix = k == blocks_count - 1 ? 0 : 1;

				if (int(i) + 1 - blocks[k].second - delta_prefix <= n &&
				    can_place_prefix[k][i + 1 - blocks[k].second - delta_prefix] &&
				    i + delta_suffix + 1 <= n &&
				    can_place_suffix[blocks_count - k - 1][i + delta_suffix + 1])
				{
					for (size_t j = i - blocks[k].second + 1; j <= i; ++j)
					{
						new_cell_list[j].set_color_possible(blocks[k].first, true);
					}
					if (i - blocks[k].second < n)
						new_cell_list[i - blocks[k].second].set_color_possible(0, true);
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
		new_cell_list.swap(cells);
		if (all_equal) break;
	}

	std::vector<size_t> result;
	return result;
}