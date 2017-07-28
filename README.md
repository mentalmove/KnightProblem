# KnightProblem

The problem is about finding a knight's tour on a chessboard
or a similar board of different size, preferable a closed one.
*A knight's tour is a sequence of moves of a knight on a chessboard
such that the knight visits every square only once.
If the knight ends on a square that is one knight's move from the beginning square
(so that it could tour the board again immediately, following the same path),
the tour is closed, otherwise it is open.*
(Wikipedia)

There are algorithms with greater chances of success
(following H. C. von Warnsdorf's rule consequently);
for large boards (beginning with 20 x 20 squares),
it would be successful in every case and much faster
to divide the complete board into smaller pieces.

## Basic idea
A possible strategy for solving problems like this starts with a board;
the board's most important property is having cells.  
In this approach, the emphasis is on the cells itselves;
cells are connected following distinct rules;
all cells together could be seen as a board, but this aspect is not important.
All cells are instances of the same idea.  
(Not to be confused with neural network solutions
where the emphasis is on the possible moves.)
