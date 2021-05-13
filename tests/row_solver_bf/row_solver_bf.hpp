#pragma once
#include <block.hpp>
#include <cell.hpp>
#include <vector>

/// Calculate puzzle for one row by bruteforce
/// @param cells      - row, vector of cells with current state
/// @param block_list - vector of pair (color number, block length)
/// @return cells indexes with changed state
std::vector<size_t> calculate_row_bf(std::vector<Cell> &cells, const std::vector<Block> &blocks);
