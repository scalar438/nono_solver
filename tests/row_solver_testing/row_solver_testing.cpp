#include "iterators.hpp"
#include <algorithm>
#include <atomic>
#include <row_solver/cell.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <row_solver/block.hpp>
#include <row_solver/row_solver.hpp>
#include <row_solver_bf.hpp>
#include <thread>
#include <vector>

using namespace std;

class DataProducer
{
public:
	DataProducer(int block_sum, int colors_count, size_t max_cells_count)
	    : m_cells_iter(max_cells_count, colors_count),
	      m_blocks_iter(block_sum, colors_count),
	      m_blocks_sum(block_sum),
	      m_colors_count(colors_count)
	{
		m_current_cells = *m_cells_iter.next();
	}

	optional<pair<vector<Cell>, vector<Block>>> next()
	{
		lock_guard l(mtx);
		auto blocks = m_blocks_iter.next();

		if (!blocks)
		{
			if (auto cells = m_cells_iter.next())
			{
				for (auto &c : *cells)
				{
					cout << c.is_color_possible(0) + c.is_color_possible(1) * 2 +
					            c.is_color_possible(2) * 4 + c.is_color_possible(3) * 8
					     << ' ';
				}
				cout << '\n';
				m_current_cells = std::move(*cells);
				m_blocks_iter   = AllBlocksIterator(m_blocks_sum, m_colors_count);
				blocks          = m_blocks_iter.next();
			}
		}
		if (!blocks) return std::nullopt;
		return make_pair(m_current_cells, std::move(*blocks));
	}

private:
	std::mutex mtx;

	AllCellsIterator m_cells_iter;
	AllBlocksIterator m_blocks_iter;

	int m_blocks_sum;
	int m_colors_count;

	vector<Cell> m_current_cells;
};

void check(shared_ptr<DataProducer> data_producer, shared_ptr<atomic<bool>> finished,
           shared_ptr<optional<pair<vector<Cell>, vector<Block>>>> failed_result,
           shared_ptr<mutex> res_mtx)
{
	while (!*finished)
	{
		auto data = data_producer->next();
		if (!data) return;

		vector<size_t> changed_bf;
		std::vector<Cell> cells_bf;
		{
			auto cells         = data->first;
			const auto &blocks = data->second;

			changed_bf = calculate_row_bf(cells, blocks);
			sort(changed_bf.begin(), changed_bf.end());
			cells_bf.swap(cells);
		}

		vector<size_t> changed_dp;
		std::vector<Cell> cells_dp;
		{
			auto cells         = data->first;
			const auto &blocks = data->second;

			changed_dp = calculate_line(cells, blocks);
			sort(changed_dp.begin(), changed_dp.end());
			cells_dp.swap(cells);
		}

		if (changed_bf != changed_dp || cells_bf != cells_dp)
		{
			*finished = true;

			lock_guard l(*res_mtx);
			if (!*failed_result)
			{
				*failed_result = std::move(data);
			}
			return;
		}
	}
}

int main()
{
	auto data          = make_shared<DataProducer>(4, 3, 4);
	auto finish_flag   = make_shared<atomic<bool>>(false);
	auto failed_result = make_shared<optional<pair<vector<Cell>, vector<Block>>>>();
	auto res_mtx       = make_shared<mutex>();
	vector<thread> threads;
	for (size_t i = 0; i != thread::hardware_concurrency(); ++i)
		threads.emplace_back(bind(check, data, finish_flag, failed_result, res_mtx));
	for (auto &x : threads)
		x.join();
	if (*failed_result)
	{
		cout << "Fail!\n";
		// Just for debugging
		calculate_line((*failed_result)->first, (*failed_result)->second);
		return -1;
	}
	else
		cout << "Ok\n";
}