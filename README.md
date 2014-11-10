Welcome to the CTL-SAT checker!
===============

### COMPILE

To compile, execute the following commands (in the folder CTLSAT/):

> make clean
> make

### EXAMPLE

In the same directory, the command

> make example

executes the program on the unsatisfable formula "~( AG(p->EXp) -> AG(p->EGp) )"

### RUN

To test satisfability of a CTL formula, execute 

> ./ctl-sat formula

Where 'formula' is a CTL formula. Example:

> ./ctl-sat "~( (A(pUq) ^ AG(q->r) ^ AG(r->EXr)) -> EFEGr )"

### SYNTAX OF FORMULAS

Spaces are automatically removed before parsing and can be inserted anywhere in the formula. 

The following symbols are reserved and cannot be used as atoms:

symbol	meaning

^	AND
v	OR
~	NEGATION
->	IMPLICATION
()	PARENTHESES
T	TRUTH (interpreted as "~((p)^(~(p)))", where p is an atom)
A	ALL PATHS
E	EXIST PATH
U	UNTIL
F	FINALLY
G	EVERYWHERE

moreover, the symbol $ is used by the parser as formula terminator and is reserved.

Any other symbol not appearing in the above list will be interpreted as an atom.

Formulas must be constructed according to the following grammar:

S -> S^S | SvS | S->S | (S) | ATOM | ~S | AXS | EXS | A~XS | E~XS | A(SUS) | A~(SUS) | E(SUS) | E~(SUS) | AFS | A~FS | EFS | E~FS | AGS | A~GS | EGS | E~GS | T

### OUTPUT DESCRIPTION

In the first line ("Parsing input formula : ") the input formula is printed.

The second line ("The input formula has been parsed as") shows how the formula has been parsed. Abbreviations such as EG, AF, T, ->, ecc.. are automatically expanded before parsing.

The third line ("The input formula has been normalized as") shows the normalized formula. Formulas are normalized removing double negations and ordering lexicographically the arguments of AND, OR.

After the line "Positive closure set (only non-negated formulas in the closure set):", the positive closure set of the formula is printed. The Full closure set can be obtained adding also the negations of the formulas in the positive closure set.

After that, the program builds feasible states and edges of the tableau. These steps may require a lot of time, depending on the length of the input formula. 

The next lines show the execution of the repeated cull. The program heuristically executes firstly EX, EXT and ENX rules (easy to check) until all states satisfy them. After that, EU, ENU, AU and ANU rules are checked on all states. The process is repeated iteratively until the initial formula is present in at least one state of the tableau OR until it is possible to cull states.

Once the cull is terminated, the program prints the number of states left and reports the (un)satisfability of the input formula.

### COMPLEXITY

The number of states is |S| = O(2^n), the number of edges is |E| = O((2^n)^2), where n is the size of the input formula (i.e. number of subformulas). There are at most |S| cull steps, each requiring O(|E|) operations. The worst-case time complexity is thus O( (2^n)^3 ), while the worst-case space complexity is O( (2^n)^2 )

### SOME EXAMPLES OF UNSATISFIABLE FORMULAS

~( (AFA~(aUb)) -> ( AF( (~a ^ ~b) v EGa) ) )




