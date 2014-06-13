/*
 * Atom.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef ATOM_H_
#define ATOM_H_

#include "Formula.h"
#include "../common.h"

namespace ctl_sat {

class Atom : public Formula {
public:

	Atom(char c);
	virtual ~Atom();

	Formula * removeDoubleNegations();

	char c;

	vector<Formula*> * positiveClosure();

};

} /* namespace ctl_sat */
#endif /* ATOM_H_ */
