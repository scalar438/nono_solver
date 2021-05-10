#pragma once
#include <optional>
#include <vector>

// Iterate over all partitions with given sum
class PartitionsIterator
{
public:
	explicit PartitionsIterator(int sum);
	std::optional<std::vector<int>> next();

private:
	std::vector<int> m_data;
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