# This is the solution for the problem
# https://www.codewars.com/kata/5a5519858803853691000069

from collections import deque
import copy

IMPOSSIBLE = 0
FILLED = 1
EMPTY = 2
UNKNOWN = 3

def calc_placeability(cells: list[int], blocks: list[int]):
    n = len(cells)
    i = 0
    while i != n and cells[i] != FILLED:
        i += 1
    res = [True] * (i + 1) + [False] * (n - i)
    res = [res]
    
    for ib, block_len in enumerate(blocks):
        end_allow_list = [False]
        cur_matched = 0
        for i, cell in enumerate(cells):
            if cell == EMPTY:
                cur_matched = 0
            else:
                cur_matched += 1
            
            def calc_current_flag():
                if cur_matched < block_len:
                    return False
                start_current_block = i - block_len + 1
                if ib != 0:
                    start_current_block -= 1
                if start_current_block < 0:
                    return False
                return (ib == 0 or cells[start_current_block] != FILLED) and res[-1][start_current_block]
            end_allow_list.append(calc_current_flag())
        for i in range(n):
            if end_allow_list[i] and not end_allow_list[i + 1] and cells[i] != FILLED:
                end_allow_list[i + 1] = True
        res.append(end_allow_list)
    return res

class Fld:
    def __init__(self, width, height):
        self.__width = width
        self.__height = height
        self.__fld = [UNKNOWN] * (width * height)
        self.__impossible_count = 0
        self.__unknown_cells = set(range(width * height))

    def __recalc_counts(self, idx, v):
        old_val = self.__fld[idx]
        
        if old_val == IMPOSSIBLE and v != IMPOSSIBLE:
            self.__impossible_count -= 1
        elif old_val != IMPOSSIBLE and v == IMPOSSIBLE:
            self.__impossible_count += 1

        if old_val == UNKNOWN and v != UNKNOWN:
            self.__unknown_cells.discard(idx)
        elif old_val != UNKNOWN and v == UNKNOWN:
            self.__unknown_cells.add(idx)

    def set_cell(self, x, y, v):
        self.__recalc_counts(x * self.__width + y, v)
        self.__fld[x * self.__width + y] = v

    def set_cell_index(self, idx, v):
        self.__recalc_counts(idx, v)
        self.__fld[idx] = v

    def is_solved(self):
        return len(self.__unknown_cells) == 0
    
    def is_impossible(self):
        return self.__impossible_count != 0
    
    def row(self, idx):
        return self.__fld[idx * self.__width:(idx + 1) * self.__width]
    
    def col(self, idx):
        return self.__fld[idx::self.__width]
    
    def set_row(self, idx, val):
        i = idx * self.__width
        for j in range(self.__width):
            self.__recalc_counts(i, val[j])
            self.__fld[i] = val[j]
            i += 1

    def set_col(self, idx, val):
        i = idx
        for j in range(self.__height):
            self.__recalc_counts(i, val[j])
            self.__fld[i] = val[j]
            j += self.__width

    def get_empty_iter(self):
        return iter(self.__unknown_cells)
    
    def get_fld_data(self):
        return tuple(tuple(x for x in self.__fld[i *self.__width:(i + 1) * self.__width]) for i in range(self.__height))
    

def solve_line(cells: list[int], blocks: list[int]):
    placeability_prefix = calc_placeability(cells, blocks);
    placeability_suffix = calc_placeability(cells[::-1], blocks[::-1]);

    cur_matched = 0
    k = len(blocks)
    n = len(cells)
    result_values = [IMPOSSIBLE] * n

    for i, v_data in enumerate(cells):
        can_be_empty = False

        if v_data == EMPTY:
            cur_matched = 0
        else:
            cur_matched += 1
        for j in range(0, k + 1):
            if not can_be_empty:
                can_be_empty = v_data != FILLED and \
                    placeability_prefix[j][i] and placeability_suffix[k - j][n - i - 1]

            if j != k and cur_matched >= blocks[j]:
                if i + 1 != n and cells[i + 1] == FILLED:
                    continue
                
                if i - blocks[j] >= 0 and cells[i - blocks[j]] == FILLED: 
                    continue

                prefix_index = i - blocks[j] + int(j == 0)
                if prefix_index < 0:
                    continue

                suffix_index = n - i - 2 + int(j == k - 1)
                if suffix_index < 0:
                    continue

                can_placed = placeability_prefix[j][prefix_index] and \
                             placeability_suffix[k - j - 1][suffix_index]
                if can_placed:
                    for x in range(i - blocks[j] + 1, i + 1):
                        result_values[x] |= FILLED
        if can_be_empty:
            result_values[i] |= EMPTY

    res = []
    for i in range(n):
        if result_values[i] == IMPOSSIBLE:
            return None
        if result_values[i] != cells[i]:
            cells[i] = result_values[i]
            res.append(i)
    return res

def solve_deducing(fld: Fld, col_blocks: list[list[int]], row_blocks: list[list[int]], width: int, height: int):
    marked = set(range(width + height))
    q = deque(marked)
    while len(q) != 0:
        elem = q.popleft()
        marked.discard(elem)

        is_row = elem >= width
        if is_row:
            elem -= width
        
        if is_row:
            cells = fld.row(elem)
            blocks = row_blocks[elem]
        else:
            cells = fld.col(elem)
            blocks = col_blocks[elem]

        changed = solve_line(cells, blocks)

        if changed is None:
            return False

        for x in changed:
            if is_row:
                fld.set_cell(elem, x, cells[x])
            else:
                fld.set_cell(x, elem, cells[x])
            idx = x if is_row else x + width
            if idx not in marked:
                marked.add(idx)
                q.append(idx)

    return True

def solve_trying(fld: list[Fld], col_blocks: list[list[int]], row_blocks: list[list[int]], width: int, height: int):
    solve_deducing(fld[0], col_blocks, row_blocks, width, height)
    while not fld[0].is_solved():
        fld_changed = False
        fld_copies = [None, None]
        new_vals = [EMPTY, FILLED]
        deducing_result = [None, None]
        for idx in fld[0].get_empty_iter():
            if fld_changed:
                break
            for j in range(2):
                fld_copies[j] = copy.deepcopy(fld[0])
                fld_copies[j].set_cell_index(idx, new_vals[j])
                deducing_result[j] = solve_deducing(fld_copies[j], col_blocks, row_blocks, width, height)
            
            for j in range(2):
                if deducing_result[j] and not deducing_result[1 - j]:
                    fld[0] = fld_copies[j]
                    fld_changed = True
                    break
        if not fld_changed:
            break

def solve(clues, width, height):
    fld = [Fld(width, height)]
    col_blocks = [list(x) for x in clues[0]]
    row_blocks = [list(x) for x in clues[1]]

    solve_trying(fld, col_blocks, row_blocks, width, height)
    return fld[0].get_fld_data()

dolphin = (
    (
        (25,), (1, 1), (1, 3, 1), (1, 6, 1), (1, 8, 1), (1, 9, 1), (1, 6, 3, 1), (1, 4, 3, 1), 
        (1, 2, 4, 1, 1), (1, 2, 2, 2, 2, 1), (1, 2, 1, 1, 2, 2), (1, 1, 1, 1, 1, 1, 2),
        (4, 1, 1, 4, 3, 1), (2, 1, 4, 2, 2, 1), (1, 1, 3, 2, 1, 2, 1), (1, 2, 2, 8, 1), (1, 3, 6, 1),
        (1, 9, 1), (1, 7, 1), (1, 7, 1), (1, 5, 1), (1, 4, 1), (1, 5, 1), (1, 6, 1), (1, 7, 1), (1, 7, 1),
        (1, 7, 1), (1, 7, 1), (1, 7, 1), (1, 5, 1), (1, 5, 1), (1, 3, 1), (1, 2, 1), (1, 1), (25,)
    ), (
        (35,), (1, 2, 1), (1, 1, 2, 1), (1, 1, 1, 1), (1, 2, 1), (1, 1, 1), (1, 5, 2, 1), (1, 8, 2, 1),
        (1, 9, 1, 1), (1, 6, 3, 2, 1), (1, 4, 1, 2, 1, 1), (1, 4, 2, 5, 3, 1), (1, 2, 1, 1, 6, 6, 1),
        (1, 3, 1, 4, 16, 1), (1, 4, 1, 17, 1), (1, 2, 24, 1), (1, 1, 1, 2, 18, 1), (1, 1, 5, 9, 1),
        (1, 2, 5, 1), (1, 2, 1), (1, 4, 1), (1, 2, 1, 1), (1, 1, 1, 1), (1, 2, 1), (35,)
        )
    )

dog = (
    (
        (4, 3), (1, 6, 2), (1, 2, 2, 1, 1), (1, 2, 2, 1, 2), (3, 2, 3), (2, 1, 3), (1, 1, 1), 
        (2, 1, 4, 1), (1, 1, 1, 1, 2), (1, 4, 2), (1, 1, 2, 1), (2, 7, 1), (2, 1, 1, 2), 
        (1, 2, 1), (3, 3)
    ),
    (
        (3, 2), (1, 1, 1, 1), (1, 2, 1, 2), (1, 2, 1, 1, 3), (1, 1, 2, 1), (2, 3, 1, 2), (9, 3),
        (2, 3), (1, 2), (1, 1, 1, 1), (1, 4, 1), (1, 2, 2, 2), (1, 1, 1, 1, 1, 1, 2),
        (2, 1, 1, 2, 1, 1), (3, 4, 3, 1)
    ))

small_undeducible = (
    ((2, 1), (1, ), (4,), (1,), (2, 1)),
    ((1, 1), (1, 1, 1), (3, ), (1, ), (1, 1, 1)))

small_unsollvable = (
    ((1, 1), (2,), (3, ), (2, ), (2, )),
    ((2,), (2, ), (3, ), (2, ), (1, 1)))
    

shapes = (
    (
        (1, 1, 5), (3, 3, 5), (1, 5, 1, 6), (3, 1, 3, 4), (1, 1, 5, 1, 3), (3, 3, 3, 3), 
        (1, 1, 1, 1, 4), (3,), (1, 1), (3, ), (1, 1, 1), (3, 1, 2), (1, 3, 2, 4), 
        (1, 3, 5), (5, 3, 4), (1, 3, 4), (3, 2, 4), (1, 2, 4), (1, 5, 1), (4, 2), (4, ),
        (1, 1, 3, 6), (1, 1, 2, 3, 4), (3, 3, 6, 2), (5, 1, 2, 1, 1, 1), (1, 3, 1, 2,),
        (5, 1, 5, 3), (3, 3, 2, 1, 4), (1, 1, 1, 2, 6), (1, 6)
    ),
    (
        (9,), (2, 1, 1, 1, 2), (9,), (), (5, 5, 5,), (3, 3, 2, 1, 2, 3, 3), (5, 5, 5), (3, 3, 3),
        (5, 5, 5), (2, 2, 2, 2, 2, 2), (2, 2, 2, 2, 2, 2), (), (1, 1, 2), (1, 3, 2),
        (1, 2, 1), (2, 1), (1, 2), (3, 1), (3, 2), (1, 2, 1), (), (), (1, 2, 2), (3, 6, 4), 
        (4, 1, 8, 5), (7, 8, 1, 5), (7, 3, 3, 1, 3), (7, 2, 2, 1, 2), (), (1,), (1, ), (3, ),
        (3, ), (5, ), (7,)
    ))


clues = small_unsollvable
a = solve(clues, len(clues[0]), len(clues[1]))
for q in a:
    print([2 - x for x in q])
