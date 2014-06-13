/*
 * ExistsUntil.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef EXISTSUNTIL_H_
#define EXISTSUNTIL_H_

#include "Formula.h"
#include "../common.h"

namespace ctl_sat {

class ExistsUntil : public Formula {
public:

	ExistsUntil(Formula * f1, Formula * f2);
	virtual ~ExistsUntil();

	Formula * getLeftSubFormula(){return f1;};
	Formula * getRightSubFormula(){return f2;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f1, * f2;

};

} /* namespace ctl_sat */
#endif /* EXISTSUNTIL_H_ */
