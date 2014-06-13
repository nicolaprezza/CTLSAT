/*
 * AllUntil.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "AllUntil.h"

namespace ctl_sat {

AllUntil::AllUntil(Formula * f1, Formula * f2){

	this->f1 = f1 ;
	this->f2 = f2 ;

	type=ALL_UNTIL;

	string_format = string("A(").append(this->f1->toString()).append("U").append(this->f2->toString()).append(")");

};

AllUntil::~AllUntil() {}

Formula * AllUntil::removeDoubleNegations(){

	return new AllUntil(
					getLeftSubFormula()->removeDoubleNegations(),
					getRightSubFormula()->removeDoubleNegations()
			);

}

vector<Formula*> * AllUntil::positiveClosure(){

	vector<Formula*> * closure1 = getLeftSubFormula()->positiveClosure();
	vector<Formula*> * closure2 = getRightSubFormula()->positiveClosure();

	closure1->insert(closure1->end(), closure2->begin(), closure2->end());
	closure1->push_back(this);

	return closure1;

}


} /* namespace ctl_sat */
