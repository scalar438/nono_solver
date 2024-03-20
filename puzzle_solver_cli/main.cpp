#include <chrono>
#include <iostream>
#include <line_solver/line_solver.hpp>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void set_yellow()
{
	std::cout << "\033[93m";
}

void set_default()
{
	std::cout << "\033[39m";
}

void write_fld(const std::vector<std::vector<Cell>> &fld, const std::pair<bool, int> &marked)
{
	for (int i = 0, ei = int(fld.size()); i != ei; ++i)
	{
		if (marked.first && marked.second == i) set_yellow();

		for (int j = 0, ej = fld[i].size(); j != ej; ++j)
		{
			if (!marked.first && marked.second == j) set_yellow();
			char c;
			auto &cell = fld[i][j];
			if (cell.is_color_possible(0))
			{
				if (cell.is_color_possible(1))
					c = '?';
				else
					c = '.';
			}
			else
			{
				if (cell.is_color_possible(1))
					c = 'X';
				else
				{
					// It won't happen in normal puzzles
					c = 'f';
				}
			}
			std::cout << c;
			if (!marked.first && marked.second == j) set_default();
		}
		std::cout << std::endl;
		if (marked.first && marked.second == i) set_default();
	}
	std::cout << "--------------------------------------\n";
}

int main()
{
	int width, height;
	std::cin >> width >> height;

	std::vector<std::vector<Block>> blocks_h(height);
	for (auto &cur_blocks : blocks_h)
	{
		std::string str;
		while (1)
		{
			std::getline(std::cin, str);
			if (str.size() != 0) break;
		}

		std::istringstream is(str);

		int x;
		while (is >> x)
		{
			cur_blocks.emplace_back(1, x);
		}
	}

	std::vector<std::vector<Block>> blocks_v(width);
	for (auto &cur_blocks : blocks_v)
	{
		std::string str;
		while (1)
		{
			std::getline(std::cin, str);
			if (str.size() != 0) break;
		}

		std::istringstream is(str);

		int x;
		while (is >> x)
		{
			cur_blocks.emplace_back(1, x);
		}
	}

	std::vector<std::vector<Cell>> fld;
	for (int i = 0; i < height; ++i)
	{
		std::vector<Cell> row;
		for (int j = 0; j < width; ++j)
		{
			Cell c;
			c.set_color_possible(0, true);
			c.set_color_possible(1, true);
			row.push_back(c);
		}
		fld.push_back(std::move(row));
	}

	// pair (is_row, index)
	std::set<std::pair<bool, int>> s;
	for (int i = 0; i < height; ++i)
		s.emplace(true, i);
	for (int i = 0; i < width; ++i)
		s.emplace(false, i);

	bool cur_d = false;
	while (!s.empty())
	{
		// Pick the first element with direction == cur_d
		std::set<std::pair<bool, int>>::const_iterator cur =
		    s.lower_bound(std::make_pair(cur_d, -1));

		if (cur == s.end())
		{
			cur_d = !cur_d;
			// it will be found because s is not empty
			cur = s.lower_bound(std::make_pair(cur_d, -1));
		}
		else
		{
			// it's possible we have found element but it is from another direction
			// Just change current direction
			if (cur->first != cur_d) cur_d = !cur_d;
		}

		write_fld(fld, *cur);

		std::vector<Cell> vc;
		if (cur->first)
		{
			for (int i = 0; i != width; ++i)
				vc.push_back(fld[cur->second][i]);
		}
		else
		{
			for (int i = 0; i != height; ++i)
				vc.push_back(fld[i][cur->second]);
		}

		auto &blocks = (cur->first ? blocks_h[cur->second] : blocks_v[cur->second]);

		for (auto idx : calculate_line(vc, blocks))
		{
			if (cur->first)
			{
				for (int i = 0; i != width; ++i)
					fld[cur->second][i] = vc[i];
			}
			else
			{
				for (int i = 0; i != height; ++i)
					fld[i][cur->second] = vc[i];
			}
			std::pair<bool, int> v{!cur->first, int(idx)};
			if (!s.contains(v)) s.emplace(v);
		}

		s.erase(cur);

		std::this_thread::sleep_for(2000ms);
	}
}
