#include "row_solver.hpp"
#include "reversed_vector.hpp"
#include <set>
#include <string>

template <class TCell, class TBlock, class TResult>
void calc_blocks_placeability(const TCell &cells, const TBlock &blocks, std::vector<TResult> &res)
{
	const size_t n = cells.size();
	if (n == 0) return;
	const int max_colors = cells[0].max_colors();
	const size_t k       = blocks.size();

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

		for (int j = 0; j < n; ++j)
		{
			if (cells[j].is_color_possible(blocks[i].first))
				current_allowed_count += 1;
			else
				current_allowed_count = 0;
			res[i + 1][j + 1] =
			    current_allowed_count >= blocks[i].second && res[i][j - blocks[i].second];
		}
	}
}

std::vector<size_t> calculate_row(std::vector<Cell> &cells,
                                  std::vector<std::pair<int, int>> &blocks)
{
	const size_t n = cells.size();
	if (n == 0) return {};

	const int max_colors = cells[0].max_colors();

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

	std::vector<bool> changed_indexes(n);

	const size_t blocks_count = blocks.size();
	// Calculate empty cells
	for (size_t i = 0; i != n; ++i)
	{
		if (cells[i].is_color_possible(i))
		{
			bool can_place = false;
			for (size_t j = 0; j <= blocks_count; ++j)
			{
				if (can_place_prefix[j][i] && can_place_suffix[j][i + 1])
				{
					can_place = true;
					break;
				}
			}
			if (!can_place)
			{
				cells[i].set_color_possible(i, false);
				changed_indexes[i] = true;
			}
		}
	}

	for (size_t ci = 1; ci <= max_colors; ++ci)
	{
		int in_the_row_count = 0;
		std::vector<bool> can_placed(n, false);
		for (size_t i = 0; i != n; ++i)
		{
			if (cells[i].is_color_possible(ci))
			{
				in_the_row_count += 1;
			}
			else
			{
				in_the_row_count = 0;
			}
			for (size_t k = 0; k != blocks_count; ++k)
			{
				if (blocks[k].second > in_the_row_count) continue;

				int delta_prefix = k == 0 ? 0 : 1;
				int delta_suffix = k == blocks_count - 1 ? 0 : 1;
				if (can_place_prefix[k][i - blocks[k].second - delta_prefix] &&
				    can_place_suffix[blocks_count - k][i + delta_suffix])
				{
					// Can be placed, color ci is possible in range [i - blocks[k]..i]
					for (size_t j = i - blocks[k].second; j <= i; ++j)
					{
						can_placed[j] = true;
					}
				}
			}
		}
		for (size_t i = 0; i != n; ++i)
		{
			if (cells[i].is_color_possible(ci) && !can_placed[i])
			{
				changed_indexes[i] = true;
				cells[i].set_color_possible(ci, false);
			}
		}
	}

	std::vector<size_t> result;
	for (size_t i = 0; i < n; ++i)
	{
		if (changed_indexes[i]) result.push_back(i);
	}

	return result;
}