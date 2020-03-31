#![allow(unused_variables)]
use csv::ReaderBuilder;
use std::fs::File;
mod solver;

extern crate clap;
use clap::Arg;

fn write_grid<W: std::io::Write>(mut out: W, grid_data: &[u8; 81]) {
    //let inserter = [",", ",", ",", ",", ",", ",", ",", ",", "\n"];
    //for (&v, &extra) in grid_data.iter().zip(inserter.iter().cycle()) {
    for (i, &v) in grid_data.iter().enumerate() {
        let extra = if (i + 1) % 9 == 0 { "\n" } else { "," };
        write!(out, "{}{}", v, extra).unwrap();
    }
}

fn grid_from_csv<R: std::io::Read>(input: R) -> solver::Grid {
    let mut rdr = ReaderBuilder::new()
        .has_headers(false)
        .flexible(true)
        .from_reader(input);

    let mut grid = solver::Grid::new();
    for (r, result) in rdr.records().enumerate() {
        let record = result.unwrap();
        for (c, i) in record.iter().enumerate() {
            match i {
                "" => grid[(r, c)] = 0,
                "\n" => continue,
                t => {
                    let mut n = i.parse::<u8>().expect(&format!(
                        "Could not parse \"{}\" into number. Line {}, Column {}",
                        i, r, c
                    ));
                    n = std::cmp::min(std::cmp::max(n, 0), 9);
                    grid[(r, c)] = n
                }
            }
        }
    }
    grid
}

fn main() -> std::io::Result<()> {
    let opts = clap::App::new("Sudoku Solver")
        .version("0.1")
        .author("Matthias M.")
        .about("Solves a Sudoku.")
        .arg(
            Arg::with_name("input")
                .long("--input")
                .short("-i")
                .value_name("FILE")
                .help("CSV file of an unsolved Sudoku."),
        )
        .arg(
            Arg::with_name("output")
                .long("--output")
                .short("-o")
                .value_name("FILE")
                .help("CSV file on the solved Sudoku."),
        )
        .get_matches();

    //use std::time::Instant;
    //let now = Instant::now();

    let grid = match opts.value_of("input").unwrap_or("-") {
        "-" => grid_from_csv(std::io::stdin()),
        path => grid_from_csv(File::open(path).unwrap()),
    };

    let g = solver::solve(&grid).expect("No Solution found!");

    match opts.value_of("output").unwrap_or("-") {
        "-" => write_grid(std::io::stdout(), &g.data),
        path => write_grid(File::create(path).unwrap(), &g.data),
    };

    //let n = now.elapsed().as_micros() as f64;
    //println!("Done. Duration: {:.3} ms", n*0.001);

    Ok(())
}
