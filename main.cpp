#include "cell.hpp"
#include "row_solver.hpp"
#include "row_solver_bf.hpp"
#include <iostream>
#include <string_view>
#include <vector>

using namespace std;

vector<Cell> parse_string(string_view str)
{
	vector<Cell> res;
	for (char c : str)
	{
		Cell cell;
		switch (c)
		{
		case '.': cell.set_color_possible(1, false); break;
		case 'X': cell.set_color_possible(0, false); break;
		}
		res.push_back(cell);
	}
	return res;
}

string from_vec_cell(const vector<Cell> &vc)
{
	string res;
	for (auto &cell : vc)
	{
		int bitmask = 0;
		if (cell.is_color_possible(0)) bitmask += 1;
		if (cell.is_color_possible(1)) bitmask += 2;
		switch (bitmask)
		{
		case 0: return "Impossible";
		case 1: res.push_back('.'); break;
		case 2: res.push_back('X'); break;
		case 3: res.push_back('?'); break;
		}
	}
	return res;
}

// strategy == 0 - bruteforce
// strategy == 1 - actual solution
bool solve(string str, vector<Block> &blocks)
{
	auto cells_exp = parse_string(str);
	auto cells_sol = cells_exp;

	calculate_row_bf(cells_exp, blocks);
	calculate_row(cells_sol, blocks);
	return cells_exp == cells_sol;
}

// Generate all vectors with sum <= n
vector<vector<Block>> gen_all_blocks(int n)
{
	// buf[i] - all vectors with sum == i
	vector<vector<vector<int>>> buf;
	buf.resize(n + 1);
	buf[0].resize(1);
	for (int i = 1; i <= n; ++i)
	{
		for (int first = 1; first <= i; ++first)
		{
			for (const auto &head : buf[i - first])
			{
				auto tmp = head;
				tmp.push_back(first);
				buf[i].push_back(tmp);
			}
		}
	}
	vector<vector<Block>> res;
	for (auto &vecs : buf)
	{
		for (auto &vec : vecs)
		{
			vector<Block> blocks;
			for(auto val : vec)
				blocks.emplace_back(Block(1, val));
			res.emplace_back(std::move(blocks));
		}
	}
	return res;
}

vector<string> gen_all_rows(int n)
{
	const char arr[3] = {'.', 'X', '?'};
	vector<int> vs(n);
	vector<string> res;
	while(1)
	{
		string str;
		for(int v : vs) str.push_back(arr[v]);
		res.emplace_back(std::move(str));
		int i = 0;
		while(i != n && vs[i] == 2)
		{
			vs[i] = 0;
			++i;
		}
		if(i == n) break;
		++vs[i];
	}
	return res;
}

int main()
{
	const int n = 7;
	auto all_blocks = gen_all_blocks(n);
	auto all_rows = gen_all_rows(n);
	std::cout << all_blocks.size() * all_rows.size();
}