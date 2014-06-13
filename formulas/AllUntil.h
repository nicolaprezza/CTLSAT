/*
 * AllUntil.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef ALLUNTIL_H_
#define ALLUNTIL_H_

#include "Formula.h"
#include "../common.h"

namespace ctl_sat {

class AllUntil : public Formula {
public:

	AllUntil(Formula * f1, Formula * f2);
	virtual ~AllUntil();

	Formula * getLeftSubFormula(){return f1;};
	Formula * getRightSubFormula(){return f2;};

	Formula * removeDoubleNegations();

	vector<Formula*> * positiveClosure();

	Formula * f1, * f2;

};

} /* namespace ctl_sat */
#endif /* ALLUNTIL_H_ */
