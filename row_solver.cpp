#include "row_solver.hpp"
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
	res[0]         = true;
	bool cur_value = true;
	for (size_t i = 0; i != n; ++i)
	{
		if (cur_value && !cells[i].is_color_possible(0)) cur_value = false;
		res[i + 1] = cur_value;
	}
}

std::vector<int> adjust(std::vector<Cell> &cells, std::vector<std::pair<int, int>> &block_list)
{
	const size_t n = cells.size();
	std::vector<int> res;
	if (n == 0) return res;

	const int max_colors = cells[0].max_colors();
	const size_t k       = block_list.size();

	// can_place_prifix[i][j] == true if we can place first i blocks at the first j cells
	// can_place_suffix - same, but for last i blocks and last n - j cells
	std::vector<std::vector<bool>> can_place_prefix, can_place_suffix;

	{
		std::vector<bool> prefix, suffix;
		prefix.reserve(n + 1);
		prefix.push_back(true);

		size_t last_non_empty = std::string::npos;
		bool prefix_possible  = true;
		for (size_t i = 0; i < n; ++i)
		{
			if (!cells[i].is_color_possible(0))
			{
				prefix_possible = false;
				last_non_empty  = i;
			}
			prefix.push_back(prefix_possible);
		}
		if (last_non_empty == std::string::npos)
		{
			suffix    = std::vector<bool>(n + 1, false);
			suffix[n] = true;
		}
		else
		{
			suffix.reserve(n + 1);
			for (size_t i = 0; i <= last_non_empty; ++i)
				suffix.push_back(false);
			for (size_t i = last_non_empty + 1; i <= n; ++i)
				suffix.push_back(true);
		}
		can_place_prefix.emplace_back(std::move(prefix));
		can_place_suffix.emplace_back(std::move(suffix));
	}

	for (size_t i = 0; i != n; ++i)
	{
		for (int i = 0; i <= max_colors; ++i)
		{}
	}
	return res;
}