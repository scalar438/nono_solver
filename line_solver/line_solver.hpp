#pragma once
#include "block.hpp"
#include "cell.hpp"
#include <vector>

/// Reveal as much as possible about one line
/// @param cells      - row, vector of cells with the current state
/// @param block_list - vector of pair (color number, block length)
/// @return cells indexes with the changed state
std::vector<size_t> calculate_line(std::vector<Cell> &cells, const std::vector<Block> &blocks);
