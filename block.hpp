#pragma once

struct Block
{
	int color_number;
	size_t block_length;
	Block(int color, size_t block_len) : color_number(color), block_length(block_len) {}
};