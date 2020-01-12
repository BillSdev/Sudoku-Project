Souce code for a Sudoku Project developed together with yotam2010.
Some features of this sudoku solver\generators are : 
-manually solve a sudoku puzzle
-validate whether a puzzle has a valid solution
     Achieved by translating the puzzle into a ILP problem
     and feeding it into the commercial optimizer, Gurobi.
-load a sudoku puzzle from a file
-save a sudoku puzzle into a file
-generate a new sudoku puzzle 
-give a hint (fill the requested cell)
-guess a solution
      done by translating the puzzle into a LP problem
      which the solution to will contain scores for all
      values for all cells and then randomly fill cells
      while taking into account the scores.
-edit existing sudoku 
-fill all the cells of the board (solve the puzzle)
-report how many possible solutions are there for the puzzle.
....
