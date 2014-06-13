/*
 * common.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef COMMON_H_
#define COMMON_H_

using namespace std;

#include <string>
#include <stdio.h>
#include <iostream>
#include <stack>
#include <cstdlib>
#include <sstream>
#include <set>
#include <vector>

enum formula_type {CONJUNCTION,ATOMIC,NEGATION,ALL_TOMORROW,EXISTS_TOMORROW,ALL_UNTIL,EXISTS_UNTIL};

typedef int formula;
typedef uint formula_index;

typedef unsigned int uint;
typedef unsigned long int ulint;

typedef unsigned char uchar;

typedef vector<bool> state;

//parser errors
# define ERROR 256
# define ATOM 0

#endif /* COMMON_H_ */
