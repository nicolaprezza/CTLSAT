/*
 * Atom.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "Atom.h"

namespace ctl_sat {

Atom::Atom(char c) {

	this->c = c;
	type=ATOMIC;

	stringstream ss;
	ss<<c;

	string_format = ss.str();


}

Atom::~Atom() {}

Formula * Atom::removeDoubleNegations(){

	return this;

}

vector<Formula*> * Atom::positiveClosure(){

	vector<Formula*> * closure = new vector<Formula*>();
	closure->push_back(this);

	return closure;

}

} /* namespace ctl_sat */
