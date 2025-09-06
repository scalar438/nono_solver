#include "puzzle_reader.hpp"
#include <chrono>
#include <iostream>
#include <line_solver/line_solver.hpp>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <termios.h>
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

void clear_screen()
{
	std::cout << "\033c";
}

void reset_cursor()
{
	std::cout << "\033[H";
}

void alternate_screen()
{
	std::cout << "\033[?47h";
}

void restore_screen()
{
	std::cout << "\033[?47l";
}

void write_fld(const std::vector<std::vector<Cell>> &fld, const std::pair<bool, int> &marked)
{
	reset_cursor();
	for (int i = 0, ei = int(fld.size()); i != ei; ++i)
	{
		if (marked.first && marked.second == i) set_yellow();

		for (int j = 0, ej = fld[i].size(); j != ej; ++j)
		{
			if (!marked.first && marked.second == j) set_yellow();
			const char *c;
			auto &cell = fld[i][j];
			if (cell.is_color_possible(0))
			{
				if (cell.is_color_possible(1))
					c = "<>";
				else
					c = "--";
			}
			else
			{
				if (cell.is_color_possible(1))
					c = "NN";
				else
				{
					// It won't happen in normal puzzles
					c = "69";
				}
			}
			std::cout << c;
			if (!marked.first && marked.second == j) set_default();
		}
		std::cout << std::endl;
		if (marked.first && marked.second == i) set_default();
	}
}

int main()
{
	auto puzzle_data = read_puzzle(std::cin);

	int width = puzzle_data.width(), height = puzzle_data.height();

	std::vector<std::vector<Block>> blocks_h(height);
	for (int i = 0; i < height; ++i)
	{
		auto &cur_blocks = blocks_h[i];

		for (auto clue : puzzle_data.row_clue(i))
		{
			cur_blocks.emplace_back(1, clue.len());
		}
	}

	std::vector<std::vector<Block>> blocks_v(width);
	for (int i = 0; i < width; ++i)
	{
		auto &cur_blocks = blocks_v[i];

		for (auto clue : puzzle_data.col_clue(i))
		{
			cur_blocks.emplace_back(1, clue.len());
		}
	}

	std::vector<std::vector<Cell>> fld;
	for (int i = 0; i < height; ++i)
	{
		std::vector<Cell> row;
		for (int j = 0; j < width; ++j)
		{
			Cell c;
			c.set_impossible();
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

	alternate_screen();
	clear_screen();

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
			// it's possible we have found an element but it is from another direction
			// Just change the current direction
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
				fld[cur->second][idx] = vc[idx];
			}
			else
			{
				fld[idx][cur->second] = vc[idx];
			}
			s.emplace(!cur->first, int(idx));
		}

		s.erase(cur);

		std::this_thread::sleep_for(2000ms);
	}
	restore_screen();
}
