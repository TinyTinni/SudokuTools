mod varisat_solver;
pub use varisat_solver::Grid;
use varisat_solver::*;

// impl std::ops::IndexMut<(usize, usize)> for Grid {
//     fn index_mut(&mut self, row_column: (usize, usize)) -> &mut Self::Output {
//         &mut self.data[row_column.0 * 9 + row_column.1]
//     }
// }

pub fn solve(grid: &Grid) -> Option<Grid> {
    let mut solver = SudokuSolver::new();
    solver.assume(&grid.assumptions());
    let solution = solver.solve();
    solution
}
