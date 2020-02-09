from pycryptosat import Solver


class TestableSolver:
    def __init__(self):
        self.solver = Solver()

    def solve(self, assumptions=[]):
        return self.solver.solve(assumptions)

    @staticmethod
    def to_var(row, column, value):
        return row * 9 * 9 + column * 9 + value + 1

    # return a tuple (row, column, value)
    @staticmethod
    def from_var(var):
        v = var - 1
        row = v // (9 * 9)
        column = (v - row * 9 * 9) // 9
        value = v - row * 9 * 9 - column * 9
        return row, column, value

    # helper function: one and only one literal can be true
    def exactly_one(self, literals):
        # at least once
        self.solver.add_clause(literals)

        for i, v in enumerate(literals):
            for j in range(i + 1, len(literals)):
                self.solver.add_clause([-literals[i], -literals[j]])

    # adds the rule, that for each position, only one value can be true
    def add_uniqueness(self, rows, columns):
        for r in range(rows):
            for c in range(columns):
                lit = [self.to_var(r, c, v) for v in range(9)]
                self.exactly_one(lit)

    # adds the rule, that for each rule, the values 1..9 exist
    def add_row_rule(self, row):
        for v in range(9):
            lit = [self.to_var(row, c, v) for c in range(9)]
            self.exactly_one(lit)

    # adds the rule, that for each column, the values 0..8 exist
    def add_column_rule(self, column):
        for v in range(9):
            lit = [self.to_var(r, column, v) for r in range(9)]
            self.exactly_one(lit)

    # adds the rule, that for each box, the values 0..8 exist
    def add_box_rule(self, column, row):
        for v in range(9):
            lit = [self.to_var(row + (i // 3), column + (i % 3), v) for i in range(9)]
            self.exactly_one(lit)


class SudokuSolver:
    def __init__(self):
        self.solver = TestableSolver()
        self.assumptions = []
        self.solver.add_uniqueness(9, 9)
        for i in range(9):
            self.solver.add_row_rule(i)
            self.solver.add_column_rule(i)
            self.solver.add_box_rule(3*(i // 3), 3*(i % 3))

    def solve(self):
        sat, sol = self.solver.solve(self.assumptions)
        if not sat:
            return None

        field = [[0 for i in range(9)] for j in range(9)]
        for i, v in enumerate(sol):
            if v:
                r, c, v = self.solver.from_var(i)
        #        print("r: {}\tc: {}\tv: {}".format(r, c, v+1))
                field[r][c] = v+1
        return field

    # x [0..8]
    # y [0..8]
    # v [1..9]
    def add_filled_number(self, x, y, v):
        self.assumptions.append(self.solver.to_var(x, y, v-1))
