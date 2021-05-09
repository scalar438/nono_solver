#ifdef MY_LOCAL_ONLINE_JUDGE
#include <fstream>
std::ifstream cin("../../../timus_task/input.txt");
std::ofstream cout("../../../timus_task/output.txt");
#else
#include <iostream>
#endif
using namespace std;
#include <string>
#include <vector>

namespace timus_sol
{

template <class TData, class TBlock>
std::vector<std::vector<bool>> calc_placeability(const TData &data, const TBlock &blocks)
{
	std::vector<std::vector<bool>> res;

	const size_t n = data.size();
	{
		size_t first_non_empty = data.find('X');
		std::vector<bool> first_row;
		first_row.reserve(n + 1);
		if (first_non_empty == string::npos) first_non_empty = n;
		while (first_row.size() <= first_non_empty)
			first_row.push_back(true);
		while (first_row.size() <= n)
			first_row.push_back(false);
		res.push_back(std::move(first_row));
	}

	for (size_t ib = 0; ib != blocks.size(); ++ib)
	{
		const size_t block_len = blocks[ib];
		std::vector<bool> end_allow_list{false};
		size_t cur_matched = 0;
		for (size_t i = 0; i != data.size(); ++i)
		{
			if (data[i] == '.')
				cur_matched = 0;
			else
				++cur_matched;
			auto calc_current_flag = [&]() {
				if (cur_matched < block_len) return false;

				size_t start_current_block = i - block_len + 1;
				if (ib != 0) --start_current_block;
				if (start_current_block > n) return false;
				return (ib == 0 || data[start_current_block] != 'X') &&
				       res.back()[start_current_block];
			};
			end_allow_list.push_back(calc_current_flag());
		}

		for (size_t i = 0; i < n; ++i)
		{
			if (end_allow_list[i] && !end_allow_list[i + 1] && data[i] != 'X')
			{
				end_allow_list[i + 1] = true;
			}
		}

		res.push_back(std::move(end_allow_list));
	}

	return res;
}

template <class T> class ReversedContainer
{
public:
	ReversedContainer(const T &data) : m_data(data), m_size(data.size()) {}

	typename T::value_type operator[](size_t i) const { return m_data[m_size - i - 1]; }

	size_t size() const { return m_size; }

	size_t find(typename T::value_type c) const
	{
		auto rfind_res = m_data.rfind(c);
		if (rfind_res == string::npos) return string::npos;
		return m_size - rfind_res - 1;
	}

private:
	const T &m_data;
	const size_t m_size;
};

void update_empty(std::vector<char> &v, size_t i)
{
	switch (v[i])
	{
	case char(0): v[i] = '.'; break;
	case 'X': v[i] = '?'; break;
	}
}


void update_block(std::vector<char> &v, size_t i)
{
	switch (v[i])
	{
	case char(0): v[i] = 'X'; break;
	case '.': v[i] = '?'; break;
	}
}

std::string calc(const std::string &data, const std::vector<int> &blocks)
{
	auto placeability_prefix = calc_placeability(data, blocks);
	auto placeability_suffix =
	    calc_placeability(ReversedContainer(data), ReversedContainer(blocks));

	size_t cur_matched = 0;
	const size_t k     = blocks.size();
	const int n        = int(data.size());
	std::vector<char> result_values(data.size());

	for (int i = 0; i != n; ++i)
	{
		const size_t v_data = data[i];
		bool can_be_empty   = false;

		if (v_data == '.')
			cur_matched = 0;
		else
			++cur_matched;
		for (size_t j = 0; j <= k; ++j)
		{
			if (!can_be_empty)
			{
				can_be_empty = (v_data != 'X') && placeability_prefix[j][i] &&
				               placeability_suffix[k - j][n - i - 1];
			}
			if (j != k && cur_matched >= blocks[j])
			{
				if (i + 1 != n && data[i + 1] == 'X') continue;

				if (i - blocks[j] >= 0 && data[i - blocks[j]] == 'X') continue;

				int prefix_index = i - blocks[j] + int(j == 0);
				if (prefix_index < 0) continue;

				int suffix_index = n - i - 2 + int(j == k - 1);
				if (suffix_index < 0) continue;

				bool can_placed = placeability_prefix[j][prefix_index] &&
				                  placeability_suffix[k - j - 1][suffix_index];
				if (can_placed)
				{
					for (int x = i - blocks[j] + 1; x <= i; ++x)
						update_block(result_values, x);
				}
			}
		}
		if (can_be_empty) update_empty(result_values, i);
	}
	std::string res;
	for (auto c : result_values)
	{
		if (c == char(0)) return "Impossible";
		res += c;
	}
	return res;
}

} // namespace timus_sol

#ifndef TESTING_ONLY
int main()
{
	int n;
	cin >> n;
	cin >> n;
	std::vector<int> blocks(n);
	for (auto &x : blocks)
		cin >> x;

	std::string data;
	cin >> data;

	cout << timus_sol::calc(data, blocks);
}
#endif