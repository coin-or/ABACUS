@q 1996 by Stefan Thienel @>

@* MAIN PROGRAM.

  The main program creates an object of the class |TSPMASTER| and invokes
  the optimization. The command line arguments are only used on UNIX
  platforms. On Windows NT the program asks the user to specify an
  input file.

  \BeginReturn
  \docarg{|0|}{If the optimum solution has been computed,}
  \docarg{|1|}{otherwise.}
  \EndReturn

  \BeginArg
  \docarg{|argc|}{The number of arguments of the command line.}
  \docarg{|argv|}{The command line. The second string is the name of the
                  file storing the problem instance that should be solved.}
  \EndArg

@(tspmain.cc@>=
#include <iostream.h>
#include "tspmaster.h"

int main(int argc, char **argv)
{
  @<local variables (|main|)@>;
  @<analyze command line arguments@>;
  @<generate a tsp and optimize@>;
  @<clean up and return (|main()|)@>;
}


@ @<local variables (|main|)@>=
  char *inputFileName;

@ @<analyze command line arguments@>=
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " <file>" << endl;
    return 1;
  }

  inputFileName = argv[1];
  

@ @<generate a tsp and optimize@>=
  TSPMASTER *tsp = new TSPMASTER(inputFileName);
  ABA_MASTER::STATUS status = tsp->optimize();
  
  delete tsp;


@ @<clean up and return (|main()|)@>=
  if (status) return 1;
  else        return 0;
  
