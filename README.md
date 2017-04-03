# espresso-logic-minimizer

A NodeJS bridge to the [Espresso heuristic logic minimizer](https://en.wikipedia.org/wiki/Espresso_heuristic_logic_minimizer).

The original source code comes from the [University of California, Berkeley](https://embedded.eecs.berkeley.edu/pubs/downloads/espresso/index.htm).

# Install

`npm install espresso-logic-minimizer`

# API

## minimize(truthTable)

Static method, minimize the input truth table directly.
The truth table must be provided as an array of strings following the PLA format (see below).

Returns an array of strings containing the minimized conditions.

## Espresso(inputSize, outputSize)

Constructor allowing to input the truth table progressively.
`inputSize` is the number of input conditions, and `outputSize` is the number of output conditions.
They match, respectively, the `.i` and `.o` parameters of PLA files.

A class instance can only process one PLA.

### push(input, output)

Pushes into the current PLA a truth table entry.
Both `input` and `output` are arrays of truthy/falsey values.

### minimize()

Ends the current PLA and minimizes it. Subsequent calls will simple return the already computed result.
Returns an array of strings containing the minimized conditions.

# How to use: simple boolean minification

Take the following boolean conditions:

`(NOT (cond1 AND cond2) OR cond3) AND cond4`

Espresso is able to simplify such conditions to [Disjunctive Normal Form (DNF)](https://en.wikipedia.org/wiki/Disjunctive_normal_form).  

To do so, start by creating a truth table describing the above boolean conditions:

|cond1|cond2|cond3|cond4|result|
|-----|-----|-----|-----|------|
|0|0|0|0|0|
|0|0|0|1|1|
|0|0|1|0|0|
|0|0|1|1|1|
|0|1|0|0|0|
|0|1|0|1|1|
|0|1|1|0|0|
|0|1|1|1|1|
|1|0|0|0|0|
|1|0|0|1|1|
|1|0|1|0|0|
|1|0|1|1|1|
|1|1|0|0|0|
|1|1|0|1|0|
|1|1|1|0|0|
|1|1|1|1|1|


## Passing the PLA data directory to espresso

Creating and maintaining a truth table entirely in memory can and will quickly fill up heap space. This way of minimizing PLAs is only to be used on small numbers of input conditions.

To use the in-memory PLA conversion, you first need to convert the truth table above into PLA format:

```
# there are 4 input variables
.i 4

# there is only 1 output result
.o 1

# the following is the truth table
0000 0
0001 1
0010 0
0011 1
0100 0
0101 1
0110 0
0111 1
1000 0
1001 1
1010 0
1011 1
1100 0
1101 0
1110 0
1111 1

# end of the PLA data
.e
```

Then simply store it into an array of strings, and feed it to espresso.

```js
const espresso = require('espresso-logic-minimizer');

const pla = [
    '.i 4',
    '.o 1',
    '0000 0',
    '0001 1',
    '0010 0',
    '0011 1',
    '0100 0',
    '0101 1',
    '0110 0',
    '0111 1',
    '1000 0',
    '1001 1',
    '1010 0',
    '1011 1',
    '1100 0',
    '1101 0',
    '1110 0',
    '1111 1',
    '.e'
  ];

console.log('result = ', espresso.minimize(pla));
```

## Input the truth table progressively

```js
const Espresso = require('espresso-logic-minimizer').Espresso;

const espresso = new Espresso(4, 1);

espresso.push([0, 0, 0, 0], [0]);
espresso.push([0, 0, 0, 1], [1]);
espresso.push([0, 0, 1, 0], [0]);
espresso.push([0, 0, 1, 1], [1]);
espresso.push([0, 1, 0, 0], [0]);
espresso.push([0, 1, 0, 1], [1]);
espresso.push([0, 1, 1, 0], [0]);
espresso.push([0, 1, 1, 1], [1]);
espresso.push([1, 0, 0, 0], [0]);
espresso.push([1, 0, 0, 1], [1]);
espresso.push([1, 0, 1, 0], [0]);
espresso.push([1, 0, 1, 1], [1]);
espresso.push([1, 1, 0, 0], [0]);
espresso.push([1, 1, 0, 1], [0]);
espresso.push([1, 1, 1, 0], [0]);
espresso.push([1, 1, 1, 1], [1]);

console.log('result = ', espresso.minimize());
```


## Interpreting the result

Both examples above output the following result:

```
result = [ '0--1 1', '-0-1 1', '--11 1' ]
```

As explained above, this result is expressed in disjunctive normal form: each array entry contains a set of AND conditions, and a OR condition must be used between entries.

Here is what you need to know to understand the result:

* `-` => "DC" flag => Don't Care
* `0` => "OFF" flag => this condition must be FALSE for the result to be true. This means we need to perform a "NOT" on this condition
* `1` => "ON" flag => this condition must be TRUE for the result to be true.

So we have:

* `0-–1 1` : NOT cond1 AND cond4
* `-0-1 1` : NOT cond2 AND cond4
* `–-11 1` : cond3 AND cond4

Final result:

`NOT cond2 AND cond4 OR NOT cond1 AND cond4 OR cond3 AND cond4`

# Repository content

The original source code is stored in the `espresso-src` folder. The only changes made to this code allow it to be compiled using C99 standards, and a few warnings have also been fixed.

The original `main.c` and `main.h` are still present, even if unused in this NodeJS library. The original executable can still be compiled and executed by executing `make` in the `espresso-src` folder.  
This will create a `bin/` directory in the root of the repository containing the `espresso` executable.

The `man` directory contains the original man pages of Espresso:

* `espresso.1` contains the executable options documentation
* `espresso.5` details the PLA format and contains other, more complex PLA examples

The NodeJS bridge itself is contained in the `bridge` directory.

# License

This library is published under the [MIT License](https://opensource.org/licenses/MIT).
