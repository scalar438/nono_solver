#ifdef MY_LOCAL_ONLINE_JUDGE
#include <fstream>
std::ifstream cin("input.txt");
std::ofstream cout("output.txt");
#else
#include <iostream>
using namespace std;
#endif
#include <algorithm>
#include <set>
#include <string>
#include <vector>

template <class T, class C> class Slice
{
public:
	Slice(const C &src, int start_index, int count, int delta)
	    : m_data(src), m_start_index(start_index), m_count(count), m_delta(delta)
	{}

	typename C::const_reference operator[](int index) const
	{
		return m_data[m_start_index + index * m_delta];
	}

	int size() const
	{
		return m_count;
	}

	Slice<T, C> rev() const
	{
		return Slice<T, C>(m_data, m_start_index + (m_count - 1) * m_delta, m_count, -m_delta);
	}

	size_t find(const T &a) const
	{
		for (size_t i = 0, idx = m_start_index; i != m_count; ++i, idx += m_delta)
		{
			if (m_data[idx] == a) return i;
		}
		return std::string::npos;
	}

protected:
	const C &m_data;
	int m_start_index;
	int m_count;
	int m_delta;
};

template <class T, class C> class SliceMut : public Slice<T, C>
{
public:
	SliceMut(C &src, int start_index, int count, int delta)
	    : Slice<T, C>(src, start_index, count, delta), m_data_mut(src)
	{}

	typename C::reference operator[](int index)
	{
		return m_data_mut[this->m_start_index + index * this->m_delta];
	}

	SliceMut<T, C> rev_mut() const
	{
		return Slice<T, C>(this->m_data_mut,
		                   this->m_start_index + (this->m_count - 1) * this->m_delta, this->m_count,
		                   -this->m_delta);
	}

private:
	C &m_data_mut;
};

constexpr int UNKNOWN    = 3;
constexpr int FILLED     = 2;
constexpr int EMPTY      = 1;
constexpr int IMPOSSIBLE = 0;

template <class C1, class C2>
std::vector<std::vector<bool>> calc_placeability(const Slice<int, C1> &data, const C2 &blocks)
{
	std::vector<std::vector<bool>> res;

	const size_t n = data.size();
	{
		size_t first_non_empty = data.find(FILLED);
		std::vector<bool> first_row;
		first_row.reserve(n + 1);
		if (first_non_empty == std::string::npos) first_non_empty = n;
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
			if (data[i] == EMPTY)
				cur_matched = 0;
			else
				++cur_matched;
			auto calc_current_flag = [&]() {
				if (cur_matched < block_len) return false;

				size_t start_current_block = i - block_len + 1;
				if (ib != 0) --start_current_block;
				if (start_current_block > n) return false;
				return (ib == 0 || data[start_current_block] != FILLED) &&
				       res.back()[start_current_block];
			};
			end_allow_list.push_back(calc_current_flag());
		}

		for (size_t i = 0; i < n; ++i)
		{
			if (end_allow_list[i] && !end_allow_list[i + 1] && data[i] != FILLED)
			{
				end_allow_list[i + 1] = true;
			}
		}

		res.push_back(std::move(end_allow_list));
	}

	return res;
}

template <class C>
bool line_solver(SliceMut<int, C> data, const std::vector<int> &blocks, std::vector<int> &res)
{
	auto placeability_prefix = calc_placeability(data, blocks);
	auto placeability_suffix = calc_placeability(
	    data.rev(), Slice<int, std::vector<int>>(blocks, blocks.size() - 1, blocks.size(), -1));

	int cur_matched = 0;
	const size_t k  = blocks.size();
	const int n     = data.size();
	std::vector<int> result_values(data.size(), IMPOSSIBLE);

	for (int i = 0; i != n; ++i)
	{
		const int v_data  = data[i];
		bool can_be_empty = false;

		if (v_data == EMPTY)
			cur_matched = 0;
		else
			++cur_matched;
		for (size_t j = 0; j <= k; ++j)
		{
			if (!can_be_empty)
			{
				can_be_empty = (v_data != FILLED) && placeability_prefix[j][i] &&
				               placeability_suffix[k - j][n - i - 1];
			}
			if (j != k && cur_matched >= blocks[j])
			{
				if (i + 1 != n && data[i + 1] == FILLED) continue;

				if (i - blocks[j] >= 0 && data[i - blocks[j]] == FILLED) continue;

				int prefix_index = i - blocks[j] + int(j == 0);
				if (prefix_index < 0) continue;

				int suffix_index = n - i - 2 + int(j == k - 1);
				if (suffix_index < 0) continue;

				bool can_placed = placeability_prefix[j][prefix_index] &&
				                  placeability_suffix[k - j - 1][suffix_index];
				if (can_placed)
				{
					for (int x = i - blocks[j] + 1; x <= i; ++x)
						result_values[x] |= FILLED;
				}
			}
		}
		if (can_be_empty) result_values[i] |= EMPTY;
	}

	res.clear();
	for (int i = 0; i != n; ++i)
	{
		if (result_values[i] == IMPOSSIBLE) return false;
		if (result_values[i] != data[i])
		{
			res.push_back(i);
			data[i] = result_values[i];
		}
	}
	return true;
}

class Field
{
public:
	Field(int width, int height)
	    : m_field(width * height, UNKNOWN), m_width(width), m_height(height)
	{
		for (int i = 0, e = width * height; i != e; ++i)
			m_unknown_cells.insert(i);
	}

	class IntLike
	{
		friend class Field;
		IntLike(int &a, Field &fld, int index) : m_value(a), m_fld(fld), m_index(index) {}

	public:
		int operator=(int a)
		{
			return m_fld.set_cell_by_index(m_index, a);
		}

		operator int() const
		{
			return m_value;
		}

		bool operator==(int a) const
		{
			return m_value == a;
		}

		bool operator!=(int a) const
		{
			return m_value != a;
		}

		int &m_value;
		Field &m_fld;
		int m_index;
	};

	typedef int const_reference;
	typedef IntLike reference;

	reference operator[](int index)
	{
		return IntLike(m_field[index], *this, index);
	}

	const_reference operator[](int index) const
	{
		return m_field[index];
	}

	SliceMut<int, Field> row(int r)
	{
		return SliceMut<int, Field>(*this, r * m_width, m_width, 1);
	}

	SliceMut<int, Field> col(int c)
	{
		return SliceMut<int, Field>(*this, c, m_height, m_width);
	}

	inline int width() const
	{
		return m_width;
	}

	int height() const
	{
		return m_height;
	}

	const std::set<int> &unknown_cells() const
	{
		return m_unknown_cells;
	}

	int set_cell_by_index(int idx, int val)
	{
		m_field[idx] = val;
		m_unknown_cells.erase(idx);
		return val;
	}

private:
	std::vector<int> m_field;
	int m_width, m_height;
	std::set<int> m_unknown_cells;

	friend std::ostream &operator<<(std::ostream &os, const Field &fld);
};

std::ostream &operator<<(std::ostream &os, const Field &fld)
{
	int idx = 0;

	std::string str;
	str.resize(fld.m_width);
	for (int i = 0; i < fld.m_height; ++i)
	{
		for (int j = 0; j < fld.m_width; ++j, ++idx)
		{
			switch (fld.m_field[idx])
			{
				// On the server characters except # and . are considered as WA
				// But it's convenient to write some other characters for testing purposes
#ifdef MY_LOCAL_ONLINE_JUDGE
			case UNKNOWN: str[j] = '?'; break;
#else
			case UNKNOWN: str[j] = '#'; break;
#endif
			case FILLED: str[j] = '#'; break;
			case EMPTY: str[j] = '.'; break;
			}
		}
		os << str << '\n';
	}
	return os;
}

bool logical_solver(Field &fld, const std::vector<std::vector<int>> &col_blocks,
                    const std::vector<std::vector<int>> row_blocks,
                    std::set<int> marked = std::set<int>())
{
	const int width = fld.width();
	if (marked.empty())
	{
		for (int i = 0, e = width + fld.height(); i != e; ++i)
			marked.insert(i);
	}
	int start_search = 0;
	std::vector<int> pos_changed;
	while (!marked.empty())
	{
		auto it = marked.lower_bound(start_search);

		if (it == marked.end())
		{
			start_search = 0;
			it           = marked.lower_bound(start_search);
		}
		else if (*it >= width)
			start_search = width;

		int elem    = *it;
		bool is_row = elem >= width;

		auto line    = is_row ? fld.row(elem -= width) : fld.col(elem);
		auto &blocks = is_row ? row_blocks[elem] : col_blocks[elem];

		if (line_solver(line, blocks, pos_changed))
		{
			for (auto x : pos_changed)
				if (is_row)
					marked.insert(x);
				else
					marked.insert(x + width);
		}
		else
			return false;

		marked.erase(it);
	}

	return true;
}

void guessing_solver(Field &fld, const std::vector<std::vector<int>> &col_blocks,
                     const std::vector<std::vector<int>> &row_blocks)
{
	logical_solver(fld, col_blocks, row_blocks);
	while (!fld.unknown_cells().empty())
	{
		bool contradiction_found = false;
		auto tmp                 = fld.unknown_cells();
		for (auto x : tmp)
		{
			if (fld[x] != UNKNOWN) continue;
			Field fld_copies[2] = {fld, fld};
			int vals[2]         = {FILLED, EMPTY};
			bool results[2];

			for (int i = 0; i < 2; ++i)
			{
				fld_copies[i].set_cell_by_index(x, vals[i]);
				results[i] = logical_solver(fld_copies[i], col_blocks, row_blocks,
				                            {x % fld.width(), x / fld.width() + fld.width()});
			}
			for (int i = 0; i < 2; ++i)
			{
				if (results[i] && !results[1 - i])
				{
					fld                 = std::move(fld_copies[i]);
					contradiction_found = true;
					break;
				}
			}
		}
		if (!contradiction_found) break;
	}
}

std::vector<std::vector<int>> read_lines(int l)
{
	std::vector<std::vector<int>> res;
	while (l-- != 0)
	{
		std::vector<int> blocks;
		while (true)
		{
			int x;
			cin >> x;
			if (x != 0)
				blocks.push_back(x);
			else
			{
				res.push_back(std::move(blocks));
				break;
			}
		}
	}
	return res;
}

int main()
{
	int n;
	cin >> n;

	while (n != 0)
	{
		--n;

		int w, h;
		cin >> h >> w;

		std::vector<std::vector<int>> blocks_h = read_lines(h);
		std::vector<std::vector<int>> blocks_v = read_lines(w);

		Field fld(w, h);
		guessing_solver(fld, blocks_v, blocks_h);
		cout << fld;
	}
}
