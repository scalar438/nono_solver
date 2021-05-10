#include "iterators.hpp"
#include <algorithm>

PartitionsIterator::PartitionsIterator(int sum) : m_data(std::vector<int>(size_t(sum), 1)) {}

std::optional<std::vector<int>> PartitionsIterator::next()
{
	auto res = m_data;
	if (!res) return res;
	if (m_data->size() == 1)
	{
		m_data = std::nullopt;
		return res;
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
	return std::move(res);
}


PermutationsIterator::PermutationsIterator(std::vector<int> numbers) : m_data(std::move(numbers))
{
	std::sort(m_data->begin(), m_data->end());
}

std::optional<std::vector<int>> PermutationsIterator::next()
{
	auto res = m_data;
	if (!res) return res;
	if (!std::next_permutation(m_data->begin(), m_data->end()))
	{
		m_data = std::nullopt;
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


AllBlocksIterator::AllBlocksIterator(int max_sum_len, int colors_count)
    : m_blocks_perms(std::vector<int>()),
      m_block_lengths(0),
      m_colors_iter(0, 0),
      current_block_sum(0),
      m_ended(false)
{
	m_current_blocks_perm    = *m_blocks_perms.next();
	m_current_blocks_lengths = *m_block_lengths.next();
	m_current_blocks_colors  = *m_colors_iter.next();
}

std::optional<std::vector<Block>> AllBlocksIterator::next()
{
	if (m_ended) return std::nullopt;
}