# Sudoku Solver

Solves a given Sudoku.
Current implementation uses [cryptominisat](https://github.com/msoos/cryptominisat).

Example on how to use:
```
python sudoku_solver.py -i ../test_files/sudoku01.csv
```

The input has to be a 9x9 csv file.
Empty entries in the Sudoku can be empty in the csv or 0.