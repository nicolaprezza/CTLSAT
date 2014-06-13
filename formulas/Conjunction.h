/*
 * Conjunction.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef CONJUNCTION_H_
#define CONJUNCTION_H_

#include "Formula.h"

namespace ctl_sat {

class Conjunction : public Formula {
public:

	Conjunction(Formula * f1, Formula * f2);

	Formula * getLeftSubFormula(){return f1;};
	Formula * getRightSubFormula(){return f2;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f1, * f2;

	virtual ~Conjunction();
};

} /* namespace ctl_sat */
#endif /* CONJUNCTION_H_ */
