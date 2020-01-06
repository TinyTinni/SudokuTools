import unittest
from sudoku_solver import *

class CSVTest(unittest.TestCase):
    def test_csv_in_out(self):
        grid = []
        with open("../test_files/sudoku01.csv", "r") as csv_file:
            grid = grid_from_csv(csv_file)
        s = solver.SudokuSolver()
        add_grid_to_solver(s, grid)
        sol = s.solve()
        solution = [[5, 3, 4, 6, 7, 8, 9, 1, 2],
                    [6, 7, 2, 1, te9, 5, 3, 4, 8],
                    [1, 9, 8, 3, 4, 2, 5, 6, 7],
                    [8, 5, 9, 7, 6, 1, 4, 2, 3],
                    [4, 2, 6, 8, 5, 3, 7, 9, 1],
                    [7, 1, 3, 9, 2, 4, 8, 5, 6],
                    [9, 6, 1, 5, 3, 7, 2, 8, 4],
                    [2, 8, 7, 4, 1, 9, 6, 3, 5],
                    [3, 4, 5, 2, 8, 6, 1, 7, 9]]
        self.assertEqual(sol, solution)

if __name__ == '__main__':
    unittest.main()
