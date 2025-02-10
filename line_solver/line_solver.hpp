#pragma once
#include "block.hpp"
#include "cell.hpp"
#include <vector>

/// Reveals as much as possible about a single line
/// @param cells      - the line - vector of cells with the current state
/// @param block_list - vector of pairs (color number, block length)
/// @return cells indexes where the state was changed
std::vector<size_t> calculate_line(std::vector<Cell> &cells, const std::vector<Block> &blocks);
