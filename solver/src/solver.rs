mod varisat_solver;
use varisat_solver::*;

// pub trait SolverTrait<Lit: ?Sized> {
//     type Lit: ?Sized;
//     fn lit_to_value(l: &Lit) -> (u8, usize, usize);
//     fn value_to_lit(v: u8, row: usize, column: usize) -> Lit;
// }

pub struct Grid {
    pub data: [u8; 81],
}

impl Grid {
    pub fn new() -> Grid {
        Grid { data: [0; 81] }
    }
    fn from_model(model: &[varisat::Lit]) -> Grid {
        let mut result = Grid::new();

        for &i in model.iter().filter(|&i| i.is_positive()) {
            let (v, r, c) = SudokuSolver::lit_to_value(i);
            result[(r, c)] = v + 1;
        }
        result
    }

    fn assumptions(&self) -> Vec<varisat::Lit> {
        let mut result: Vec<varisat::Lit> = Vec::new();

        for (i, &v) in self.data.iter().enumerate().filter(|(_, &v)| v > 0) {
            let row = i % 9;
            let column = i / 9;
            result.push(SudokuSolver::value_to_lit(v - 1, row, column));
        }
        result
    }
}

impl std::ops::IndexMut<(usize, usize)> for Grid {
    fn index_mut(&mut self, row_column: (usize, usize)) -> &mut Self::Output {
        &mut self.data[row_column.0 * 9 + row_column.1]
    }
}

impl std::ops::Index<(usize, usize)> for Grid {
    type Output = u8;
    fn index(&self, row_column: (usize, usize)) -> &Self::Output {
        &self.data[row_column.0 * 9 + row_column.1]
    }
}

pub fn solve(grid: &Grid) -> Option<Grid> {
    let mut solver = SudokuSolver::new();
    let solution = solver.solve(&grid.assumptions());
    match solution {
        Some(m) => Some(Grid::from_model(&m)),
        _ => None,
    }
}
