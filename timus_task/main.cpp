#ifdef MY_LOCAL_ONLINE_JUDGE
#include <fstream>
std::ifstream cin("../../../timus_task/input.txt");
std::ofstream cout("../../../timus_task/output.txt");
#else
#include <iostream>
#endif
using namespace std;
#include <block.hpp>
#include <row_solver.hpp>
#include <string>

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

int main()
{
	int n;
	cin >> n;
	cin >> n;
	std::vector<Block> blocks;
	for (int i = 0; i < n; ++i)
	{
		int bl;
		cin >> bl;
		blocks.emplace_back(Block(1, size_t(bl)));
	}
	vector<Cell> cells;
	{
		string row;
		cin >> row;
		cells = parse_string(row);
	}
	calculate_row(cells, blocks);
	cout << from_vec_cell(cells);
}