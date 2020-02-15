#![allow(unused_variables)]
use std::fs::File;
use csv::ReaderBuilder;

extern crate varisat;
use varisat::{Solver, ExtendFormula};

extern crate clap;
use clap::{Arg, App, SubCommand};

fn index_to_value(index: usize) -> (usize, usize, usize)
{
    let v = index - 1;
    let row = v / (9 * 9);
    let column = (v - row * 9 * 9) / 9;
    let value = v - row * 9 * 9 - column * 9;
    return (row, column, value)
   
}

#[test]
fn test_index_to_value() {
    let (row, column, value) = index_to_value(432);
    assert_eq!(row, 5);
    assert_eq!(column, 2);
    assert_eq!(value, 8);
}



fn value_to_index(row: usize, column: usize, value: usize) -> usize
{
     return row * 9 * 9 + column * 9 + value +1;
}


#[test]
fn test_value_to_index() {

    let (row, column, value) = index_to_value(432);
    assert_eq!(row, 5);
    assert_eq!(column, 2);
    assert_eq!(value, 8);
}

fn exactly_one(solver: &mut varisat::Solver, lits: &[varisat::Lit] )
{
    // at least one has to be true
    solver.add_clause(lits);

    // make pairs, if one is true, the other has to be false
    for i in 0..lits.len()
    {
        for j in (i+1)..lits.len()
        {
            solver.add_clause(&[ !lits[i], !lits[j] ]);
        }
    }    
}

fn add_uniquness(solver: &mut varisat::Solver, rows: usize, columns: usize)
{
    for r in 0..rows
    {
        for c in 0..columns
        {
            let mut lits = Vec::with_capacity(9); 
            for v in 0..9
            {
                let i = value_to_index(r, c, v);
                lits.push(varisat::Lit::from_index(i, true));
            }
            exactly_one(solver, &lits);
        }
    }
}
#[test]
fn test_add_row_rule()
{
    
    let mut solver = varisat::Solver::new();
    add_uniquness(&mut solver, 1, 9);
    add_row_rule(&mut solver, 0);

    let mut vec = Vec::new();
    for i in 0..8
    {
        let i = value_to_index(0, i, i);
        vec.push(varisat::Lit::from_index(i, true));
    }

    solver.assume(&vec);
    assert_eq!(solver.solve().unwrap(), true);
    //assert_eq!(solver.
    
    let proofed_literal = varisat::Lit::from_index(value_to_index(0,8,8), true);
    let m = solver.model().unwrap();
    assert_eq!(m.contains(&proofed_literal), true);

}

fn add_column_rule(solver: &mut varisat::Solver, column: usize)
{
    for v in 0..9
    {
        let mut lits = Vec::with_capacity(9);
        for r in 0..9
        {
            let i = value_to_index(r, column, v);
            lits.push(varisat::Lit::from_index(i, true));
        }
        exactly_one(solver, &lits);

    }
}

fn add_row_rule(solver: &mut varisat::Solver, row: usize)
{
    for v in 0..9
    {
        let mut lits = Vec::with_capacity(9);
        for c in 0..9
        {
            let i = value_to_index(row, c, v);
            lits.push(varisat::Lit::from_index(i, true));
        }
        exactly_one(solver, &lits);

    }
}

fn add_box_rule(solver: &mut varisat::Solver, row: usize, column: usize)
{
    for v in 0..9
    {
        let mut lits = Vec::with_capacity(9);
        for i in 0..9
        {
            let index = value_to_index(row + (i / 3), column + (i % 3), v);
            lits.push(varisat::Lit::from_index(index, true));
        }
        exactly_one(solver, &lits);

    }
}

#[test]
fn test_exactly_one()
{
    let mut solver = varisat::Solver::new();
    let (x, y, z) = solver.new_lits();
    exactly_one(&mut solver, &[x,y,z]);
    
    solver.assume(&[!y, !z]);
    assert_eq!(solver.solve().unwrap(), true);
    assert!(solver.model().unwrap().contains(&x));


    solver.assume(&[!x, !z]);
    assert_eq!(solver.solve().unwrap(), true);
    assert!(solver.model().unwrap().contains(&y));


    solver.assume(&[!x, !y, !z]);
    assert_eq!(solver.solve().unwrap(), false);


    solver.assume(&[!x]);
    assert_eq!(solver.solve().unwrap(), true);
    assert!(solver.model().unwrap().contains(&y) || solver.model().unwrap().contains(&z));
}

fn grid_to_assumption(grid: &[usize]) -> Vec<varisat::Lit>
{
    let mut result : Vec<varisat::Lit> = Vec::new();

    for (i, v) in grid.iter().enumerate()
    {
        if *v > 0
        {
            let row = i % 9;
            let column = i / 9;
            let index = value_to_index(row, column, *v -1);
            result.push(varisat::Lit::from_index(index, true));
        }
    }

    return result;
}

fn model_to_grid(model: &[varisat::Lit]) -> Vec<usize>
{
    let mut result : Vec<usize> = vec![0;81];

    for i in model
    {
        if i.is_positive()
        {
            let (r, c, v) = index_to_value(i.index());
            result[r+c*9] = v+1;
        }
    }
    return result;
}

fn solve (grid : &[usize] ) -> Vec<usize>
{
    let mut solver = varisat::Solver::new();
    add_uniquness(&mut solver, 9, 9);
    for i in 0..9
    {
        add_row_rule(&mut solver, i);
        add_column_rule(&mut solver, i);
        add_box_rule(&mut solver, 3*(i%3), 3*(i/3));
    }

    let a = grid_to_assumption(&grid);

    let g = model_to_grid(&a);

    solver.assume(&a);

    let sol = solver.solve().expect("No Solution found!");
    let m = solver.model().expect("No Solution found!");

    return model_to_grid(&m);
}

fn write_grid<W: std::io::Write>(mut out :  W, grid: &[usize])
{
    for i in 0..grid.len()
    {
        write!(out, "{}", grid[i]);
        if (i+1) % 9 == 0
        {
            out.write(b"\n");
        }else
        {
            out.write(b",");
        }
    }
}

fn grid_from_csv<R: std::io::Read>(input : R) -> Vec<usize>
{
    let mut rdr = ReaderBuilder::new().has_headers(false)
        .flexible(true)
        .from_reader(input);

    let mut grid = Vec::new();
    for result in rdr.records() {
        let record = result.unwrap();
        for i in record.iter()
        {
            match i
            {
                "" => grid.push(0),
                "\n" => continue,
                t => {
                    let n : usize = i.parse::<usize>().unwrap();
                    grid.push(n);
                }
            }
        }
        
    }
    return grid
}

fn main() -> std::io::Result<()> {

    let opts = clap::App::new("Sudoku Solver")
        .version("0.1")
        .author("Matthias M.")
        .about("Solves a Sudoku.")
        .arg(Arg::with_name("input")
            .long("--input")
            .short("-i")
            .value_name("FILE")
            .help("CSV file of an unsolved Sudoku.")
        )
        .arg(Arg::with_name("output")
            .long("--output")
            .short("-o")
            .value_name("FILE")
            .help("CSV file on the solved Sudoku.")
        )
        .get_matches();


    //use std::time::Instant;
    //let now = Instant::now();

    let grid  = match opts.value_of("input").unwrap_or("-")
    {
        "-" => grid_from_csv(std::io::stdin()),
        path => grid_from_csv(File::open(path).unwrap()),
    };


    let g = solve(&grid);

    match opts.value_of("output").unwrap_or("-")
    {
        "-" => write_grid(std::io::stdout(), &g),
        path => write_grid(File::create(path).unwrap(), &g),
    };

    //let n = now.elapsed().as_micros() as f64;
    //println!("Done. Duration: {:.3} ms", n*0.001);

    Ok(())
}

