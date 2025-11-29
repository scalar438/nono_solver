#pragma once
#include <puzzle_solver/puzzle_data.hpp>
#include <exception>
#include <istream>

class PuzzleReadException : public std::exception
{
public:
	PuzzleReadException(const std::string &arg) {}
};

PuzzleData read_puzzle(std::istream &is);