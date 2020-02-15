from unittest import TestCase
from solver import TestableSolver as Solver
from solver import SudokuSolver


class TestSudokuSolver(TestCase):
    def test_to_var(self):
        s = Solver()
        r = 5
        c = 2
        v = 8
        var = s.to_var(r, c, v)
        self.assertEqual(var, 432)

    def test_from_var(self):
        s = Solver()
        var = 432
        r, c, v = s.from_var(var)
        self.assertEqual(r, 5)
        self.assertEqual(c, 2)
        self.assertEqual(v, 8)

    def test_exactly_once(self):
        s = Solver()
        lit = [1, 2, 3]
        s.exactly_one(lit)

        ass = [-1, -3]
        sat, sol = s.solve(ass)
        self.assertTrue(sat)
        self.assertEqual(sol, (None, False, True, False))

        ass = [-1, -2, -3]
        sat, sol = s.solve(ass)
        self.assertFalse(sat)

        ass = []
        sat, sol = s.solve(ass)
        self.assertTrue(sat)
        self.assertEqual(sol.count(True), 1)

    def test_add_row_rule(self):
        s = Solver()
        s.add_row_rule(0)
        s.add_uniqueness(1, 9)
        ass = [
            s.to_var(0, x, x) for x in range(8)
        ]
        sat, sol = s.solve(ass)
        self.assertTrue(sat)
        for i, v in enumerate(sol):
            if v:
                r, c ,v = s.from_var(i)
                print("c: {}\tr: {}\tv: {}".format(r, c, v+1))
        self.assertTrue(sol[s.to_var(0, 8, 8) - 1])

    def test_add_column_rule(self):
        s = Solver()
        s.add_column_rule(0)
        s.add_uniqueness(9, 1)
        ass = [
            s.to_var(x, 0, x) for x in range(8)
        ]
        sat, sol = s.solve(ass)
        self.assertTrue(sat)
        for i, v in enumerate(sol):
            if v:
                r, c, v = s.from_var(i)
                print("c: {}\tr: {}\tv: {}".format(r, c, v+1))
        self.assertTrue(sol[s.to_var(8, 0, 8) - 1])

    def test_add_box_rule(self):
        s = Solver()
        s.add_box_rule(0, 0)
        s.add_uniqueness(9, 9)
        ass = [
            s.to_var(x // 3, x % 3, x) for x in range(8)
        ]
        sat, sol = s.solve(ass)
        self.assertTrue(sat)
        for i, v in enumerate(sol):
            if v:
                r, c, v = s.fromVar(i)
                print("c: {}\tr: {}\tv: {}".format(r, c, v + 1))
        self.assertTrue(sol[s.to_var(2, 2, 8) - 1])

    def test_complete_solver(self):
        s = SudokuSolver()
        # 5 3 0 0 7 0 0 0 0
        # 6 0 0 1 9 5 0 0 0
        # 0 9 8 0 0 0 0 6 0
        # 8 0 0 0 6 0 0 0 3
        # 4 0 0 8 0 3 0 0 1
        # 7 0 0 0 2 0 0 0 6
        # 0 6 0 0 0 0 2 8 0
        # 0 0 0 4 1 9 0 0 5
        # 0 0 0 0 8 0 0 7 9

        s.add_filled_number(8, 4, 8)
        s.add_filled_number(8, 7, 7)
        s.add_filled_number(8, 8, 9)

        s.add_filled_number(7, 3, 4)
        s.add_filled_number(7, 4, 1)
        s.add_filled_number(7, 5, 9)
        s.add_filled_number(7, 8, 5)

        s.add_filled_number(6, 1, 6)
        s.add_filled_number(6, 6, 2)
        s.add_filled_number(6, 7, 8)

        s.add_filled_number(5, 0, 7)
        s.add_filled_number(5, 4, 2)
        s.add_filled_number(5, 8, 6)

        s.add_filled_number(4, 0, 4)
        s.add_filled_number(4, 3, 8)
        s.add_filled_number(4, 5, 3)
        s.add_filled_number(4, 8, 1)

        s.add_filled_number(3, 0, 8)
        s.add_filled_number(3, 4, 6)
        s.add_filled_number(3, 8, 3)

        s.add_filled_number(2, 1, 9)
        s.add_filled_number(2, 2, 8)
        s.add_filled_number(2, 7, 6)

        s.add_filled_number(1, 0, 6)
        s.add_filled_number(1, 3, 1)
        s.add_filled_number(1, 4, 9)
        s.add_filled_number(1, 5, 5)

        s.add_filled_number(0, 0, 5)
        s.add_filled_number(0, 1, 3)
        s.add_filled_number(0, 4, 7)

        grid = s.solve()
        if len(grid) == 0:
            print("not solvable")
        else:
            print(grid)
        solution = [[5, 3, 4, 6, 7, 8, 9, 1, 2],
                    [6, 7, 2, 1, 9, 5, 3, 4, 8],
                    [1, 9, 8, 3, 4, 2, 5, 6, 7],
                    [8, 5, 9, 7, 6, 1, 4, 2, 3],
                    [4, 2, 6, 8, 5, 3, 7, 9, 1],
                    [7, 1, 3, 9, 2, 4, 8, 5, 6],
                    [9, 6, 1, 5, 3, 7, 2, 8, 4],
                    [2, 8, 7, 4, 1, 9, 6, 3, 5],
                    [3, 4, 5, 2, 8, 6, 1, 7, 9]]
        self.assertEqual(grid, solution)
