Railway planner problem solved with dinamyc programming
april 2020
eviatarc

problem:
the railway problem is a problem where during run time we'll recieve the data
we need to be a able to plan the cheapest railway with the next information:
1.requested total length of the railway
2.type and amount of connections (that connect each rail piece to another)
3.set of parts of to build the rail with, described as: start connection type,
end connection type, price and length
techical information:
all of the data mention are being read from a file (given to the program
by executing it with the name of the txt file), and print the solution to
a txt file at the same directory as the program (can be modified manually)
in case of any error during runtime the program will stop itself and will
denote the error in the putput file, except mamory allocation and memory
managment issues, also in a problem of writing to an output file, in this
cases, we will make sure all recent memory heap used is freed and after it
the program will stop itself and return 1 to the user's cmd.
required fil

assumptios:
the program required input is a txt file with the following structure:
(in case of other input the program will exit with code 1)
first line: total length required of the rail
second line: the number of connections types
third line: the connection types repreented as single characters
forth line and else: part line described as: the four properties above
in the same order saperated by only a comma.

solution:
using dynamic programming, we are able to solve this problem at a runtime of
O(|types of connections|*|length of railway required|*|num of parts|)
in me i solution i will fill a matrix of ([length of rail]x[types of connections]) 
where each cell will contain the cheapest price to build a rail in this length 
with a given end connection, at successful run it will return to the putput file
the cheapestprice for building a railway at the given parameters
if this rail is not possible to build this cell will hold in it INT_MAX to
represent infinity.

notes and place for improvment:
1.i decided to write some unnecessary assignment for the code to be clearer for
the next reader.
2.the global variables are should be local and can be so
3.the matrix can become to a 3D matrix (using minor changes) and hold the data
 of the parts that are used in each stage of the building.
4. using an integer very close to the INT_MAX will make the program run and 
stores a lot of data (most personal computers might not handle this well)
 => do not test this programon your personal computer, instead use a modified
programming computer and make sure it defines its heap mamory availability 
to be well known to yoy and limited, in case any runtime memory error.
