/*
 * Negation.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef NEGATION_H_
#define NEGATION_H_

#include "Formula.h"
#include "../common.h"

namespace ctl_sat {

class Negation : public Formula {
public:

	Negation(Formula * f);
	virtual ~Negation();

	Formula * getSubFormula(){return f;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f;

};

} /* namespace ctl_sat */
#endif /* NEGATION_H_ */
