#pragma once
#include <optional>

const int MAX_COLORS = 31;

class Cell
{
	explicit Cell(int max_colors);

	/// Return 0 (background color) or color number (number from 1 to MAX_COLORS) if cell state is
	/// determined If not, return nullopt
	std::optional<int> get_color() const;

	bool is_color_possible(int color_number) const;

	bool is_impossible() const;

	void set_color_possible(int color_number, bool possible);

	int max_colors() const;

private:
	// Bitmask with possible colors. 0 - "impossible" cell, it mustn't appear in normal puzzless
	// Least bit is background (i.e. if m_data == 0 then cell is background) all other bits are
	// colors
	uint32_t m_data;

	int m_max_colors;
};
