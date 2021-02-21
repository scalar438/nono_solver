#pragma once
#include "cell.hpp"
#include <vector>

/// Calculate puzzle for one row.
/// @param cells      - row, vector of cells with current state
/// @param block_list - vector of pair (color number, block length)
/// @return  number of cells with changed state
std::vector<size_t> calculate_row(std::vector<Cell> &cells, std::vector<std::pair<int, int>> &blocks);