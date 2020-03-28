mod varisat_solver;
use varisat_solver::*;

// impl std::ops::IndexMut<(usize, usize)> for Grid {
//     fn index_mut(&mut self, row_column: (usize, usize)) -> &mut Self::Output {
//         &mut self.data[row_column.0 * 9 + row_column.1]
//     }
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

        for &i in model {
            if i.is_positive() {
                let (v, r, c) = SudokuSolver::lit_to_value(i);
                result.set_value(v + 1, r, c);
            }
        }
        result
    }

    pub fn set_value(&mut self, value: u8, row: usize, column: usize) {
        self.data[row * 9 + column] = value;
    }

    pub fn get_value(&self, row: usize, column: usize) -> u8 {
        self.data[row * 9 + column]
    }

    pub fn assumptions(&self) -> Vec<varisat::Lit> {
        let mut result: Vec<varisat::Lit> = Vec::new();

        for (i, &v) in self.data.iter().enumerate() {
            if v > 0 {
                let row = i % 9;
                let column = i / 9;
                result.push(SudokuSolver::value_to_lit(v, row, column));
            }
        }
        result
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
    solver.assume(&grid.assumptions());
    let solution = solver.solve();
    match solution {
        None => return None,
        Some(m) => return Some(Grid::from_model(&m)),
    }
}
