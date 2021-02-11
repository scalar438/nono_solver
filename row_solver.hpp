#pragma once
#include "cell.hpp"
#include <vector>

/// Calculate puzzle for one row.
/// @param cells      - row, vector of cells with current state
/// @param block_list - vector of pair (color number, block length)
/// @return  number of cells with changed state
std::vector<int> adjust(std::vector<Cell> &cells, std::vector<std::pair<int, int>> &block_list);