/*
 * ExistsTomorrow.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef EXISTSTOMORROW_H_
#define EXISTSTOMORROW_H_

#include "Formula.h"
#include "../common.h"

namespace ctl_sat {

class ExistsTomorrow : public Formula {
public:

	ExistsTomorrow(Formula * f);
	virtual ~ExistsTomorrow();

	Formula * getSubFormula(){return f;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f;

};

} /* namespace ctl_sat */
#endif /* EXISTSTOMORROW_H_ */
