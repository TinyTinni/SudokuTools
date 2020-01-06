#!/usr/bin/env python

import argparse
import csv

preamble = """<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" width="300" height="300" stroke="#000" stroke-width="1.2" fill="none">
"""

empty_grid = """<rect x="1" y="1" width="298" height="298"/>
<path d="M100,1V299m99,0V1m100,99H1m0,99H299"/>
<path stroke-width=".3" d="m34,1V299m33,0V1m66,0V299m33,0V1m66 0V299m33,0V1m34,264H1m0-33H299m0-66H1m0-33H299m0-66H1m0-33H299m0-33H1"/>
"""

ending = """</svg>
"""

def cell_text(n, x, y):
    return_str = """<text
     xml:space="preserve"
     style="font-size:30px;line-height:1.25;font-family:sans-serif;text-align:center;text-anchor:middle;fill:#000000;fill-opacity:1"
     x="{}"
     y="{}"
     id="text883"><tspan
       sodipodi:role="line"
       id="tspan881"
       x="{}"
       y="{}">{}</tspan></text>""".format(x, y, x, y, n)
       
    return return_str

def insert_numbers_from_csv(output, csv_data):  # -> list[list[int]]
    csv_reader = csv.reader(csv_data)
    return_grid = []
    for (row_n, r) in enumerate(csv_reader):
        for (col_n, c) in enumerate(r):
            if c != '' and 0 < int(c) <= 9:
                output.write(cell_text(c, 16+33*row_n, 30+33*col_n))
        
def main(i, o):
    o.write(preamble)
    o.write(empty_grid)
    insert_numbers_from_csv(o, i)
    o.write(ending)

if __name__ == "__main__":
    p = argparse.ArgumentParser(description="""svg generator for csv Sudokus.""")
    p.add_argument("-i", type=argparse.FileType("r"), default="-",
                   help="csv input file.")
    p.add_argument("-o", type=argparse.FileType("w"), default="-",
                   help="svg output file.")
    args = p.parse_args()
    result = main(args.i, args.o)