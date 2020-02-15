# Sudoku Solver

Solves a given Sudoku.
For a Python version, based on cryptominisat, have a look in the [python](./python) subdirectory.

Example on how to use:
```
sudoku_solver -i ../test_files/sudoku01.csv
```

The input has to be a 9x9 csv file.
Empty entries in the Sudoku can be empty in the csv or 0.

Currently, the program misses error detection, will come later.