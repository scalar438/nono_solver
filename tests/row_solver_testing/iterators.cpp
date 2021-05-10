#include "iterators.hpp"
#include <algorithm>

PartitionsIterator::PartitionsIterator(int sum)
{
	m_data.assign(size_t(sum), 1);
}

std::optional<std::vector<int>> PartitionsIterator::next()
{
	if (m_data.size() == 0)
	{
		return std::nullopt;
	}
	auto res = m_data;
	if (m_data.size() == 1)
	{
		m_data.clear();
		return res;
	}
	int sum2 = m_data.back();
	m_data.pop_back();
	sum2 += m_data.back();

	++m_data.back();

	while (1)
	{
		sum2 -= m_data.back();
		if (sum2 < m_data.back())
		{
			m_data.back() += sum2;
			break;
		}
		auto tmp = m_data.back();
		m_data.push_back(tmp);
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
