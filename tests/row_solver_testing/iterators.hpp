#pragma once
#include <vector>
#include <optional>

// Iterate over all partitions with given sum
class PartitionsIterator
{
public:
	explicit PartitionsIterator(int sum);
	std::optional<std::vector<int>> next();
};

class PermutationsIterator
{
public:
	explicit PermutationsIterator(std::vector<int> numbers);
	std::optional<std::vector<int>> next();
};

// Iterate over all permutations with repetitions
class PermutationsRepIterator
{
public:
	PermutationsRepIterator(int max_val, int count);
	std::optional<std::vector<int>> next();
};