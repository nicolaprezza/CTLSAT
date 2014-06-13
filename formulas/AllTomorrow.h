/*
 * AllTomorrow.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef ALLTOMORROW_H_
#define ALLTOMORROW_H_

#include "Formula.h"

namespace ctl_sat {

class AllTomorrow : public Formula {
public:

	AllTomorrow(Formula * f);
	virtual ~AllTomorrow();

	Formula * getSubFormula(){return f;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f;

};

} /* namespace ctl_sat */
#endif /* ALLTOMORROW_H_ */
