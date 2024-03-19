#pragma once
#include <block.hpp>
#include <cell.hpp>
#include <optional>
#include <vector>

// Iterate over all partitions with the given sum
class PartitionsIterator
{
public:
	explicit PartitionsIterator(int sum);
	std::optional<std::vector<int>> next();

private:
	std::optional<std::vector<int>> m_data;

	void update_data();
};

class PermutationsIterator
{
public:
	explicit PermutationsIterator(std::vector<int> numbers);
	std::optional<std::vector<int>> next();

private:
	std::optional<std::vector<int>> m_data;
};

// Iterate over all permutations with repetitions
class PermutationsRepIterator
{
public:
	PermutationsRepIterator(int max_val, int count);
	std::optional<std::vector<int>> next();

private:
	std::optional<std::vector<int>> m_data;
	int m_max_val;
};

class OrderedPartitionIterator
{
public:
	OrderedPartitionIterator(int sum);
	std::optional<std::vector<int>> next();

private:
	PartitionsIterator m_part_iter;
	PermutationsIterator m_perm_iter;
};

// Iterate over all block with sum lenghts <= max_sum and colors count == color_count
class AllBlocksIterator
{
public:
	AllBlocksIterator(int max_sum, int colors_count);
	std::optional<std::vector<Block>> next();

private:
	OrderedPartitionIterator m_blocks_len_iter;
	PermutationsRepIterator m_colors_iter;

	std::vector<int> m_current_blocks_lengths;
	std::vector<int> m_current_blocks_colors;

	int m_current_block_sum;
	int m_max_blocks_sum;
	int m_colors_count;

	std::vector<Block> gen_data() const;
};

// Iterate over vector<Cell> with all possible states
class AllCellsIterator
{
public:
	AllCellsIterator(size_t cells_count, int max_colors_count);
	std::optional<std::vector<Cell>> next();

private:
	// This field MUST be before m_states_iter
	const int m_cells_states_count;

	PermutationsRepIterator m_states_iter;

	size_t m_current_cells_count;
	const size_t m_max_cells_count;
};