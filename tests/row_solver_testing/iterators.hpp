#pragma once
#include <block.hpp>
#include <optional>
#include <vector>

// Iterate over all partitions with given sum
class PartitionsIterator
{
public:
	explicit PartitionsIterator(int sum);
	std::optional<std::vector<int>> next();

private:
	std::optional<std::vector<int>> m_data;
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

class AllBlocksIterator
{
public:
	AllBlocksIterator(int max_sum_len, int colors_count);
	std::optional<std::vector<Block>> next();

private:
	PermutationsIterator m_blocks_perms;
	PartitionsIterator m_block_lengths;
	PermutationsRepIterator m_colors_iter;

	std::vector<int> m_current_blocks_perm;
	std::vector<int> m_current_blocks_lengths;
	std::vector<int> m_current_blocks_colors;

	int current_block_sum;

	bool m_ended;
};