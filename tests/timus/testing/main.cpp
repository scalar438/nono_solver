#include "row_solver_bf.hpp"
#include <cell.hpp>
#include <row_solver.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <vector>

#include "solution/main.cpp"

using namespace std;

struct OneRunData
{
	string row;
	vector<Block> blocks;

	string ans_exp;
	string ans_actual;
};

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

pair<string, string> solve(string str, vector<Block> &blocks)
{
	auto cells_exp = parse_string(str);
	auto cells_sol = cells_exp;

	std::vector<int> blocks_timus;
	for (auto b : blocks)
	{
		blocks_timus.push_back(int(b.block_length));
	}

	calculate_row_bf(cells_exp, blocks);
	// calculate_row(cells_sol, blocks);
	return make_pair(from_vec_cell(cells_exp), timus_sol::calc(str, blocks_timus));
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
			for (auto val : vec)
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
	while (1)
	{
		string str;
		for (int v : vs)
			str.push_back(arr[v]);
		res.emplace_back(std::move(str));
		int i = 0;
		while (i != n && vs[i] == 2)
		{
			vs[i] = 0;
			++i;
		}
		if (i == n) break;
		++vs[i];
	}
	return res;
}

void run_in_one_thread(std::mutex &mtx, vector<pair<vector<Block> *, string *>> &data,
                       optional<OneRunData> &answer_mismatch)
{
	while (1)
	{
		pair<vector<Block> *, string *> one_run_data;
		{
			std::lock_guard g(mtx);
			if (data.empty()) break;
			one_run_data = data.back();
			data.pop_back();
		}
		auto res = solve(*one_run_data.second, *one_run_data.first);
		if (res.first != res.second)
		{
			std::lock_guard g(mtx);
			data.clear();
			OneRunData data;
			data.ans_exp    = move(res.first);
			data.ans_actual = move(res.second);
			data.blocks     = *one_run_data.first;
			data.row        = *one_run_data.second;
			answer_mismatch = move(data);
		}
	}
}

void items_counter(std::mutex &mtx, vector<pair<vector<Block> *, string *>> &input_data)
{
	while (1)
	{
		this_thread::sleep_for(std::chrono::seconds(1));
		lock_guard g(mtx);
		auto s = input_data.size();
		if (s == 0) break;
		cout << "Items remaining: " << s << '\n';
	}
}

int main()
{
	const int n     = 10;
	auto all_blocks = gen_all_blocks(n);
	auto all_rows   = gen_all_rows(n);
	vector<pair<vector<Block> *, string *>> all_input;
	for (auto &block : all_blocks)
	{
		for (auto &row : all_rows)
		{
			all_input.emplace_back(make_pair(&block, &row));
		}
	}
	vector<thread> v_thrd;
	std::mutex mtx;
	optional<OneRunData> answer_mismatch;
	for (unsigned int i = 0; i != std::thread::hardware_concurrency(); ++i)
		v_thrd.emplace_back(thread(run_in_one_thread, std::ref(mtx), std::ref(all_input),
		                           std::ref(answer_mismatch)));
	v_thrd.push_back(thread(items_counter, ref(mtx), ref(all_input)));
	for (auto &thrd : v_thrd)
		thrd.join();
	if (answer_mismatch)
	{
		cout << "Answer mismatch!\nInput: " << answer_mismatch->row << "\nBlocks: ";
		for (auto &block : answer_mismatch->blocks)
			cout << block.block_length;
		cout << "\nRight answer is: " << answer_mismatch->ans_exp
		     << ", actual answer is: " << answer_mismatch->ans_actual;
	}
	else
	{
		cout << "Ok";
	}
	cout << '\n';
}
