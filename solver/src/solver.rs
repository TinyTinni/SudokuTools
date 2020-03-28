mod varisat_solver;
use varisat_solver::*;

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

    pub fn get_value(&self, row: usize, column: usize) -> &u8 {
        &self.data[row * 9 + column]
    }

    pub fn assumptions(&self) -> Vec<varisat::Lit> {
        let mut result: Vec<varisat::Lit> = Vec::new();

        for (i, &v) in self.data.iter().enumerate() {
            if v > 0 {
                let row = i / 9;
                let column = i % 9;
                result.push(SudokuSolver::value_to_lit(v-1, row, column));
            }
        }
        result
    }
}

// impl std::ops::IndexMut<(usize, usize)> for Grid {
//     fn index_mut(&mut self, row_column: (usize, usize)) -> &mut Self::Output {
//         &mut self.data[row_column.0 * 9 + row_column.1]
//     }
// }

impl std::ops::Index<(usize, usize)> for Grid {
    type Output = u8;
    fn index(&self, row_column: (usize, usize)) -> &Self::Output {
        self.get_value(row_column.0, row_column.1)
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

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_solver(){
        let mut grid = Grid::new();
        grid.data = [
            8,0,0,0,0,0,0,0,0,
            0,0,3,6,0,0,0,0,0,
            0,7,0,0,9,0,2,0,0,
            0,5,0,0,0,7,0,0,0,
            0,0,0,0,4,5,7,0,0,
            0,0,0,1,0,0,0,3,0,
            0,0,1,0,0,0,0,6,8,
            0,0,8,5,0,0,0,1,0,
            0,9,0,0,0,0,4,0,0
        ];
        let mut solution = Grid::new();
        solution.data = [
            8,1,2,7,5,3,6,4,9,
            9,4,3,6,8,2,1,7,5,
            6,7,5,4,9,1,2,8,3,
            1,5,4,2,3,7,8,9,6,
            3,6,9,8,4,5,7,2,1,
            2,8,7,1,6,9,5,3,4,
            5,2,1,9,7,4,3,6,8,
            4,3,8,5,2,6,9,1,7,
            7,9,6,3,1,8,4,5,2
        ];

        let computated = solve(&grid).unwrap();

        for i in 0..81 {
            assert_eq!(computated.data[i], solution.data[i]);
        }

    }   
}