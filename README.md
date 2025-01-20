# Truthtable

This project takes in a file as input that has the specifications to a circuit, "builds" that circuit, and prints out what its truth table would look like. 

## Input Format 
In this language, circuits are specified with a series of directives. These directives refer to various
named variables, which correspond to wires in a circuit diagram. Many of the directives describe a
logic gate or similar building block, indicating which varibles correspond to its input and output.
Each directive has the form of a name followed by one or more parameters, separated by
whitespace. The name indicates which directive and determines the number of parameters. Some
directives take a variable number of parameters; their first parameter will always be an integer which
is used to determine the number of parameters. Depending on the directive, some parameters will
be inputs and some will be outputs.
Variables in a circuit can be classified into three non-overlapping sets. Input variables must be
declared by the INPUT directive, and may only occur as input parameters. Output variables must be
declared by the OUTPUT directive and may occur exactly once in an output parameter. All other
variables are temporary variables and must occur exactly once as an output parameter and zero or
more times as an input parameter.
A variable name consists of a letter followed by zero or more letters or digits. You may assume
that variable names are no longer than 16 characters.
In addition to variables, the constant inputs 0 and 1 may be used as input parameters. These
are always false and always true, respectively.
Finally, _ may be used as the output of a gate, indicating that the output of a gate is discarded.
Use of _ is equivalent to using a temporary variable that is not used as an input to any other gate.

### Directives

This section describes each of the directives used to describe a circuit. Each directive is followed by
several parameters. A parameter n is always an integer and has a special meaning. Input parameters
are indicated as i and output parameters are indicated as o. Ellipses (· · · ) are used to indicate a
variable number of parameters.
• INPUT n i1 · · · in
Declares n input variables. This directive must always occur first in a circuit description.
3
• OUTPUT n o1 · · · on
Declares n output variables. This directive must always occur second in a circuit description.
• NOT i o
Represents a not gate in logic design. Computes o = i.
• AND i1 i2 o
Represents an and gate in logic design. Computes o = i1i2.
• OR i1 i2 o
Represents an or gate in logic design. Computes o = i1 + i2.
• NAND i1 i2 o
Represents a nand gate in logic design. Computes o = i1i2
• NOR i1 i2 o
Represents a nor gate in logic design. Computes o = i1 + i2
• XOR i1 i2 o
Represents an xor gate in logic design. Computes o = i1 ⊕ i2, where ⊕ indicates exclusive or.
• DECODER n i1 · · · in o0 · · · o2^n−1
Represents an n : 2^n decoder gate in logic design. The first argument gives the number of
inputs, n. The next n parameters are the inputs, followed by 2^n parameters indicating the
outputs.
The inputs are interpreted as an n-bit binary number s in the range 0, · · · , 2^n − 1, where i1
is the most significant bit and in is the least significant bit. The output os will be 1 and all
others will be 0.
• MULTIPLEXER n i0 · · · i2^n−1 i′1 · · · i′n o
Represents a 2^n : 1 multiplexer gate in logic design. The inputs to a multiplexer are either
regular inputs or selectors, indicated by i and i′, respectively. The first parameter, n, gives the
number of selectors. The next 2^n parameters give the regular inputs, followed by n selector
inputs, and finally the output.
The selector inputs are interpreted as an n-bit binary number s in the range 0, · · · , 2^n − 1.
The output is o = is.
• PASS i o
Represents the absence of a gate. Computes o = i. This may be used to convert a temporary
variable into an output variable.

### Parsing 

The specification language can be thought of as a sequence of tokens separated by whitespace. No
keyword or variable name in the language exceeds 16 characters, so it is safe to use fscanf() with
the format code %16s, which will read a token of up to 16 non-whitespace characters.
Each directive will be either be followed by a fixed number of parameters or by a number that
will determine the number of parameters. Thus, the parsing code will always be able to tell whether
it expects a directive name, integer, input parameter, or output parameter next. 

## Output Format

The output of truthtable is a truth table showing each combination of inputs and the
corresponding output for the specified circuit. Each column in the table corresponds to a specific
input or output variable, which are given in the same order as their declaration in the INPUT and
OUTPUT directives. Columns are separated by a single space, and a vertical bar (|) occurs between
the input and output variables.

## Project Implementation

I implemented this project using 
