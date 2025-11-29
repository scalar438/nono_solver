#include "puzzle_solver/puzzle_data.hpp"
#include <set>

Color::Color(uint_fast8_t r, uint_fast8_t g, uint_fast8_t b) : m_color(r || (g << 8) || (b << 16))
{}

uint_fast8_t Color::R() const
{
	return m_color & 255;
}

uint_fast8_t Color::G() const
{
	return (m_color >> 8) & 255;
}

uint_fast8_t Color::B() const
{
	return (m_color >> 16) & 255;
}

PuzzleData::PuzzleData(int width, int height, std::vector<std::vector<BlockData>> row_clues,
                       std::vector<std::vector<BlockData>> col_clues,
                       std::optional<Color> background_color)
{
	if (width != col_clues.size()) throw InvalidPuzzleException();
	if (height != row_clues.size()) throw InvalidPuzzleException();

	m_width  = width;
	m_height = height;

	std::set<Color> all_colors;
	for (auto &clues : {row_clues, col_clues})
	{
		for (auto &blocks : clues)
		{
			for (auto block : blocks)
				all_colors.insert(block.color());
		}
	}

	m_colors           = std::vector(all_colors.begin(), all_colors.end());
	m_background_color = background_color;
	m_row_clues.swap(row_clues);
	m_col_clues.swap(col_clues);
}

int PuzzleData::height() const
{
	return m_height;
}

int PuzzleData::width() const
{
	return m_width;
}

const std::vector<BlockData> &PuzzleData::row_clue(int index) const
{
	return m_row_clues.at(index);
}

const std::vector<BlockData> &PuzzleData::col_clue(int index) const
{
	return m_col_clues.at(index);
}

Color PuzzleData::get_color(int index) const
{
	return m_colors.at(index);
}

std::optional<Color> PuzzleData::background_color(int index) const
{
	return m_background_color;
}