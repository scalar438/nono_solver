#include "puzzle_reader.hpp"
#include <boost/json.hpp>
#include <boost/json/parse_options.hpp>
#include <boost/json/src.hpp>

namespace
{

constexpr size_t MAX_SIZE = 256;

std::vector<std::vector<std::pair<int, int>>> parse_clues_data(const boost::json::object &obj,
                                                               const std::string &clues_name)
{
	auto it = obj.find(clues_name);
	if (it == obj.end())
		throw PuzzleReadException(std::string("There is not clues data '") + clues_name + "'");
	if (!it->value().is_array()) throw PuzzleReadException("The clues data isn't an array");
	std::vector<std::vector<std::pair<int, int>>> res;
	for (auto &line_clues_raw : it->value().as_array())
	{
		if (!line_clues_raw.is_array()) throw PuzzleReadException("The clues data isnt' an array");

		// Currently, only single-color puzzles are supported.
		std::vector<std::pair<int, int>> line_clues;
		for (auto block_data : line_clues_raw.as_array())
		{
			line_clues.push_back(std::make_pair<int, int>(block_data.as_int64(), 0));
		}
		res.emplace_back(std::move(line_clues));
	}
	return res;
}

PuzzleData read_json(std::istream &is)
{
	boost::json::parse_options opts;
	opts.allow_comments = true;
	std::error_code ec;

	auto data = boost::json::parse(is, ec, {}, opts);

	auto &obj = data.get_object();
	auto it   = obj.find("size");
	if (it == obj.end()) throw PuzzleReadException("There is no 'size' data in the input file");

	size_t width, height;
	{
		// Parse size data
		auto &size_val = it->value();

		if (!size_val.is_object()) throw PuzzleReadException("The size isn't an object");

		auto &size_obj = size_val.as_object();

		auto it_w = size_obj.find("width");
		if (it_w == size_obj.end())
			throw PuzzleReadException("There is no width data in the input file");

		auto it_h = size_obj.find("height");
		if (it_h == size_obj.end())
			throw PuzzleReadException("There is no height data in the input file");

		width  = it_w->value().as_int64();
		height = it_h->value().as_int64();

		if (width > MAX_SIZE || height > MAX_SIZE)
			throw PuzzleReadException(
			    "The the puzzle is too large. Sizes have to be less or equal 256");
	}

	it = obj.find("clues");
	if (it == obj.end()) throw PuzzleReadException("There is no clues");

	// Only a single-color puzzle is supported
	Color colors[1] = {Color(0, 0, 0)};

	std::vector<std::vector<BlockData>> cols, rows;
	{
		if (!it->value().is_object()) throw PuzzleReadException("The 'clues' data isn't an object");
		auto &clues_obj = it->value().as_object();

		for (auto &line_clue : parse_clues_data(clues_obj, "cols"))
		{
			std::vector<BlockData> line_data;
			for (auto block : line_clue)
			{
				line_data.push_back(BlockData(block.first, colors[block.second]));
			}
			cols.emplace_back(std::move(line_data));
		}

		for (auto line_clue : parse_clues_data(clues_obj, "rows"))
		{
			std::vector<BlockData> line_data;
			for (auto block : line_clue)
			{
				line_data.push_back(BlockData(block.first, colors[block.second]));
			}
			rows.emplace_back(std::move(line_data));
		}
	}

	return PuzzleData(width, height, std::move(rows), std::move(cols), std::nullopt);
}

} // namespace

PuzzleData read_puzzle(std::istream &is)
{
	return read_json(is);
}