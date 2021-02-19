#include "row_solver.hpp"
#include "reversed_vector.hpp"
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

std::vector<int> adjust(std::vector<Cell> &cells, std::vector<std::pair<int, int>> &blocks)
{
	const size_t n = cells.size();
	std::vector<int> res;
	if (n == 0) return res;

	const int max_colors = cells[0].max_colors();

	// can_place_prifix[i][j] == true if we can place first i blocks at the first j cells
	// can_place_suffix - same, but for last i blocks and last n - j cells
	std::vector<std::vector<bool>> can_place_prefix, can_place_suffix;

	calc_blocks_placeability(cells, blocks, can_place_prefix);
	{
		std::vector<ReversedVector<bool>> can_place_suffix_rev;
		for (auto &vec : can_place_suffix)
			can_place_suffix_rev.push_back(ReversedVector(vec));
		calc_blocks_placeability(ReversedVector(cells), ReversedVector(blocks),
		                         can_place_suffix_rev);
	}

	calc_blocks_placeability(ReversedVector(cells), ReversedVector(blocks), can_place_suffix);

	for (size_t i = 0; i != n; ++i)
	{
		for (int i = 0; i <= max_colors; ++i)
		{}
	}
	return res;
}