use line_solver::{Block, CellState};

fn f(
	cells: &[CellState],
	blocks: &[Block],
	mut current_cell_index: usize,
	current_block_index: usize,
	blocks_pos: &mut [usize],
	result_cells: &mut [line_solver::CellState],
) {
	if current_block_index == blocks.len() {
		let mut empty_cells = Vec::new();
		// Check if cells before the first block can be empty
		for i in 0..*blocks_pos.first().unwrap_or(&cells.len()) {
			if !cells[i].is_color_possible(0) {
				return;
			}
			empty_cells.push(i);
		}

		// Check if cells between the blocks can be empty
		for i in 1..current_block_index {
			for j in blocks_pos[i - 1] + (blocks[i - 1].len as usize)..blocks_pos[i] {
				if !cells[j].is_color_possible(0) {
					return;
				}
				empty_cells.push(j);
			}
		}

		// Check if cells after the last block can be empty
		while current_cell_index != cells.len() {
			if !cells[current_cell_index].is_color_possible(0) {
				return;
			}
			empty_cells.push(current_cell_index);
			current_cell_index += 1;
		}

		// If we are here, blocks can be placed. Write possibilities
		for (block_pos, block) in blocks_pos.iter().zip(blocks.iter()) {
			for j in 0..block.len {
				result_cells[*block_pos + (j as usize)].set_color_possible(block.color, true);
			}
		}
		for idx in empty_cells {
			result_cells[idx].set_color_possible(0, true);
		}
		return;
	}
	// If the current block is not first, we have to add one empty cell
	if (current_block_index != 0
		&& blocks[current_block_index - 1].color == blocks[current_block_index].color)
	{
		current_cell_index += 1;
	}

	let mut in_the_row_count = 0;
	while current_cell_index < cells.len() {
		if cells[current_cell_index].is_color_possible(blocks[current_block_index].color) {
			in_the_row_count += 1;
		} else {
			in_the_row_count = 0;
		}
		if in_the_row_count >= blocks[current_block_index].len {
			// We can place the block. Save the position and move further
			blocks_pos[current_block_index] =
				current_cell_index - (blocks[current_block_index].len as usize) + 1;
			f(
				cells,
				blocks,
				current_cell_index + 1,
				current_block_index + 1,
				blocks_pos,
				result_cells,
			);
		}
		current_cell_index += 1;
	}
}

pub fn calculate_row_bf(cells: &mut [CellState], blocks: &[Block]) -> Vec<usize> {
	let mut blocks_pos = vec![0; blocks.len()];
	let mut result_cells = vec![CellState::new_impossible(); cells.len()];

	f(cells, blocks, 0, 0, &mut blocks_pos, &mut result_cells);

	let mut result = Vec::new();

	for (idx, (cell_target, cell_source)) in cells.iter_mut().zip(result_cells.iter()).enumerate() {
		if cell_target != cell_source {
			result.push(idx);
		}
		*cell_target = *cell_source;
	}

	result
}
