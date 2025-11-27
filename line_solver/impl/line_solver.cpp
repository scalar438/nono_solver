#include "../line_solver.hpp"
#include "reversed_container.hpp"
#include <algorithm>

namespace
{

class PlaceabilityCalculator
{
public:
	PlaceabilityCalculator(const std::vector<Cell> &cells, const std::vector<Block> &blocks)
	{
		m_prefix_placeability = fill_positions(cells, blocks);
		m_suffix_placeability = fill_positions(ReversedContainer(cells), ReversedContainer(blocks));
	}

	// return true if we can place first block_index blocks on the first cell_index cells
	[[nodiscard]] bool can_place_prefix(size_t block_index, size_t cell_index) const
	{
		return m_prefix_placeability[block_index][cell_index];
	}

	// return true if we can place last block_index blocks on the last cell_index cells
	[[nodiscard]] bool can_place_suffix(size_t block_index, size_t cell_index) const
	{
		return m_suffix_placeability[block_index][cell_index];
	}

private:
	std::vector<std::vector<bool>> m_prefix_placeability;

	std::vector<std::vector<bool>> m_suffix_placeability;

	template <class TData, class TBlock>
	static std::vector<std::vector<bool>> fill_positions(const TData &data, const TBlock &blocks)
	{
		std::vector<std::vector<bool>> res;

		const size_t n = data.size();
		{
			size_t first_non_empty = 0;
			while (first_non_empty != n && data[first_non_empty].is_color_possible(0))
				++first_non_empty;
			std::vector<bool> first_row;
			first_row.reserve(n + 1);
			while (first_row.size() <= first_non_empty)
				first_row.push_back(true);
			while (first_row.size() <= n)
				first_row.push_back(false);
			res.push_back(std::move(first_row));
		}

		for (size_t ib = 0; ib != blocks.size(); ++ib)
		{
			const size_t block_len = blocks[ib].block_length;
			const int block_color  = blocks[ib].color_number;
			const bool need_gap = ib != 0 && blocks[ib - 1].color_number == blocks[ib].color_number;
			size_t cur_matched  = 0;

			std::vector<bool> end_allow_list{false};
			end_allow_list.reserve(n);
			bool simple_pass = false;
			for (size_t i = 0; i != data.size(); ++i)
			{
				if (!data[i].is_color_possible(block_color))
					cur_matched = 0;
				else
					++cur_matched;
				if (simple_pass && data[i].is_color_possible(0))
				{
					end_allow_list.push_back(true);
					continue;
				}
				auto calc_current_flag = [&]() {
					if (cur_matched < block_len) return false;

					size_t start_current_block = i - block_len + 1;
					if (need_gap) --start_current_block;
					// start_current block is unsigned, so decreasing it don't make it negative
					if (start_current_block > n) return false;
					return (!need_gap || data[start_current_block].is_color_possible(0)) &&
					       res.back()[start_current_block];
				};
				end_allow_list.push_back(simple_pass = calc_current_flag());
			}

			res.push_back(std::move(end_allow_list));
		}

		return res;
	}
};

} // namespace

std::vector<size_t> calculate_line(std::vector<Cell> &cells, const std::vector<Block> &blocks)
{
	PlaceabilityCalculator pc(cells, blocks);

	const size_t k = blocks.size();
	const size_t n = cells.size();
	std::vector<Cell> result_values;
	{
		Cell c;
		c.set_impossible();
		result_values.assign(cells.size(), c);
	}

	size_t colors_status[MAX_COLORS] = {0};

	for (size_t i = 0; i != n; ++i)
	{
		const auto &v_data = cells[i];
		for (int j = 0; j != MAX_COLORS; ++j)
		{
			if (v_data.is_color_possible(j + 1))
				colors_status[j] += 1;
			else
				colors_status[j] = 0;
		}
		bool can_be_empty = false;

		for (size_t j = 0; j <= k; ++j)
		{
			if (!can_be_empty)
			{
				can_be_empty = v_data.is_color_possible(0) && pc.can_place_prefix(j, i) &&
				               pc.can_place_suffix(k - j, n - i - 1);
			}
			if (j != k && colors_status[blocks[j].color_number - 1] >= blocks[j].block_length)
			{
				bool need_gap_after =
				    i + 1 != n && j + 1 < k && blocks[j + 1].color_number == blocks[j].color_number;
				if (need_gap_after && !cells[i + 1].is_color_possible(0)) continue;

				bool need_gap_before = i - blocks[j].block_length < n && j != 0 &&
				                       blocks[j - 1].color_number == blocks[j].color_number;
				if (need_gap_before && !cells[i - blocks[j].block_length].is_color_possible(0))
					continue;

				size_t prefix_index = i - blocks[j].block_length + size_t(!need_gap_before);
				if (prefix_index > n) continue;

				size_t suffix_index = n - i - 2u + size_t(!need_gap_after);
				if (suffix_index > n) continue;

				bool can_placed = pc.can_place_prefix(j, prefix_index) &&
				                  pc.can_place_suffix(k - j - 1, suffix_index);
				if (can_placed)
				{
					for (size_t x = i - blocks[j].block_length + 1; x <= i; ++x)
						result_values[x].set_color_possible(blocks[j].color_number, true);
				}
			}
		}
		if (can_be_empty) result_values[i].set_color_possible(0, true);
	}
	std::vector<size_t> res;
	for (size_t i = 0; i != n; ++i)
	{
		if (result_values[i] != cells[i]) res.push_back(i);
	}
	cells.swap(result_values);
	return res;
}