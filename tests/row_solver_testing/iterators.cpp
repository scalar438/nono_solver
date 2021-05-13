#include "iterators.hpp"
#include <algorithm>

PartitionsIterator::PartitionsIterator(int sum) : m_data(std::vector<int>(size_t(sum), 1)) {}

std::optional<std::vector<int>> PartitionsIterator::next()
{
	auto res = m_data;
	if (m_data) update_data();
	return res;
}

void PartitionsIterator::update_data()
{
	if (m_data->size() <= 1)
	{
		m_data = std::nullopt;
		return;
	}
	auto &data = *m_data;
	int sum2   = data.back();
	data.pop_back();
	sum2 += data.back();

	++data.back();

	while (1)
	{
		sum2 -= data.back();
		if (sum2 < data.back())
		{
			data.back() += sum2;
			break;
		}
		auto tmp = data.back();
		data.push_back(tmp);
	}
}

PermutationsIterator::PermutationsIterator(std::vector<int> numbers) : m_data(std::move(numbers))
{
	std::sort(m_data->begin(), m_data->end());
}

std::optional<std::vector<int>> PermutationsIterator::next()
{
	auto res = m_data;
	if (m_data)
	{
		if (!std::next_permutation(m_data->begin(), m_data->end()))
		{
			m_data = std::nullopt;
		}
	}
	return res;
}


PermutationsRepIterator::PermutationsRepIterator(int max_val, int count)
    : m_data(std::vector<int>(size_t(count), 0)), m_max_val(max_val)
{}

std::optional<std::vector<int>> PermutationsRepIterator::next()
{
	auto res = m_data;
	if (m_data)
	{
		auto &data = *m_data;
		size_t i   = 0;
		while (i != data.size() && data[i] == m_max_val)
		{
			data[i] = 0;
			++i;
		}
		if (i == data.size())
			m_data = std::nullopt;
		else
		{
			data[i] += 1;
		}
	}
	return res;
}


OrderedPartitionIterator::OrderedPartitionIterator(int sum)
    : m_part_iter(sum), m_perm_iter(*m_part_iter.next())
{}

std::optional<std::vector<int>> OrderedPartitionIterator::next()
{
	auto res = m_perm_iter.next();
	if (!res)
	{
		if (auto new_part = m_part_iter.next())
		{
			m_perm_iter = PermutationsIterator(std::move(*new_part));
			res         = m_perm_iter.next();
		}
	}
	return res;
}


AllBlocksIterator::AllBlocksIterator(int max_sum, int colors_count)
    : m_blocks_len_iter(0),
      m_colors_iter(colors_count - 1, 0),
      m_current_block_sum(0),
      m_max_blocks_sum(max_sum),
      m_colors_count(colors_count - 1)
{
	m_current_blocks_lengths = *m_blocks_len_iter.next();
}

std::optional<std::vector<Block>> AllBlocksIterator::next()
{
	if (auto new_blocks_colors = m_colors_iter.next())
	{
		m_current_blocks_colors = std::move(*new_blocks_colors);
	}
	else if (auto new_blocks_len = m_blocks_len_iter.next())
	{
		m_current_blocks_lengths = std::move(*new_blocks_len);

		m_colors_iter =
		    PermutationsRepIterator(m_colors_count, int(m_current_blocks_lengths.size()));
		m_current_blocks_colors = *m_colors_iter.next();
	}
	else if (m_current_block_sum < m_max_blocks_sum)
	{
		++m_current_block_sum;
		m_blocks_len_iter = OrderedPartitionIterator(m_current_block_sum);
		return next();
	}
	else
	{
		return std::nullopt;
	}
	return gen_data();
}

std::vector<Block> AllBlocksIterator::gen_data() const
{
	std::vector<Block> res;
	for (size_t i = 0, e = m_current_blocks_lengths.size(); i != e; ++i)
		res.emplace_back(m_current_blocks_colors[i] + 1, size_t(m_current_blocks_lengths[i]));
	return res;
}

AllCellsIterator::AllCellsIterator(size_t max_cells_count, int max_colors_count)
    : m_cells_states_count((1 << (max_colors_count + 1)) - 2),
      m_states_iter(m_cells_states_count, 1),
      m_current_cells_count(1),
      m_max_cells_count(max_cells_count)
{}

std::optional<std::vector<Cell>> AllCellsIterator::next()
{
	if (auto states = m_states_iter.next())
	{
		std::vector<Cell> res;
		res.reserve(states->size());
		for (auto x : *states)
		{
			Cell c;
			c.set_impossible();
			int bitmask = 1;
			int counter = 0;
			x += 1;
			while (x != 0)
			{
				if ((x & bitmask) != 0) c.set_color_possible(counter, true);
				x &= ~bitmask;

				counter += 1;
				bitmask <<= 1;
			}
			res.push_back(c);
		}
		return res;
	}
	else
	{
		if (m_current_cells_count != m_max_cells_count)
		{
			++m_current_cells_count;
			m_states_iter = PermutationsRepIterator(m_cells_states_count, m_current_cells_count);
			return next();
		}
	}
	return std::nullopt;
}