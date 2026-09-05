const MAX_COLORS: u8 = 31;

#[derive(Clone, Copy, PartialEq, Eq)]
pub struct CellState {
	color: u32,
}

// Calculate position of enabled bit.
fn get_bit_number(val: u32) -> u8 {
	let bit_pos: [u8; 32] = [
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7,
		26, 12, 18, 6, 11, 5, 10, 9,
	];
	return bit_pos[(val.wrapping_mul(0x077CB531u32) >> 27) as usize];
}

#[test]
fn test_get_bit_number() {
	for i in 0..32 {
		assert_eq!(get_bit_number(1u32 << i), i);
	}
}

impl CellState {
	pub fn new_impossible() -> CellState {
		CellState { color: 0 }
	}

	fn new_certain(color: u8) -> CellState {
		CellState { color: 1 << color }
	}

	fn new_empty() -> CellState {
		CellState { color: 1 }
	}

	fn get_color(&self) -> Option<u8> {
		if (self.color & (self.color - 1)) == 0 {
			Some(get_bit_number(self.color))
		} else {
			None
		}
	}

	pub fn is_color_possible(&self, color: u8) -> bool {
		(self.color & (1u32 << color)) != 0
	}

	pub fn set_color_possible(&mut self, color_number: u8, possible: bool) {
		if possible {
			self.color |= 1u32 << color_number;
		} else {
			self.color &= !(1u32 << color_number);
		}
	}
}

pub struct Block {
	pub color: u8,
	pub len: u8,
}

pub fn calculate_line(cells: &mut [CellState], blocks: &[Block]) -> Vec<usize> {
	todo!()
}
