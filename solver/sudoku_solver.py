#!/usr/bin/env python

import solver
import csv
import argparse
#import time
import io


def grid_from_csv(data):  # -> list[list[int]]
    csv_reader = csv.reader(data)
    return_grid = []
    for r in csv_reader:
        row_list = []
        for i in r:
            if i is None or i == '':
                row_list.append(0)
            elif 0 <= int(i) <= 9:
                row_list.append(int(i))

        #assert (len(row_list) <= 9), "too many values in a row"
        #assert (len(row_list) >= 9), "not enough values in a row"

        return_grid.append(row_list)

    #assert (len(return_grid) <= 9), "too many rows in the grid"
    #assert (len(return_grid) >= 9), "not enough rows in the grid"

    return return_grid


# 2 dimensional list
def add_grid_to_solver(s: solver.SudokuSolver, grid):
    for r in enumerate(grid):
        for c in enumerate(r[1]):
            if c[1]:
                s.add_filled_number(r[0], c[0], c[1])


def grid_as_csv(grid, output):
    csv_writer = csv.writer(output, lineterminator='\n')
    #for r in grid:
    csv_writer.writerows(grid)


def main(csv_data, output):
    grid = grid_from_csv(csv_data)

    s = solver.SudokuSolver()
    add_grid_to_solver(s, grid)
    #start = time.time()
    sol = s.solve()
    #print("solved in {} ms".format((time.time() - start)*1000))
    if sol is None:
        print("No solution found.")

    grid_as_csv(sol, output)


def print_help():
    print("Please enter filename.")


if __name__ == "__main__":
    p = argparse.ArgumentParser(description="""Solves a given Sudoku given in CSV-format 
    and prints the solution as CSV.
    Unknown entries of the Sudoku can be empty or 0 entries in the csv.""",
                                epilog="""Example:
    python sudoku_solver.py -i test_files/sudoku01.csv
    """)
    p.add_argument("-i", type=argparse.FileType("r"), default="-",
                   help="CSV input file of the unsolved Sudoku.")
    p.add_argument("-o", type=argparse.FileType("w"), default="-",
                   help="CSV output file of the solved Sudoku.")
    args = p.parse_args()
    main(args.i, args.o)
