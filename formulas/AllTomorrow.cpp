/*
 * AllTomorrow.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "AllTomorrow.h"

namespace ctl_sat {

AllTomorrow::AllTomorrow(Formula * f){

	this->f=f;
	type=ALL_TOMORROW;

	string_format = (string("AX(").append(f->toString())).append(")");

};

AllTomorrow::~AllTomorrow() {}

Formula * AllTomorrow::removeDoubleNegations(){

	return new AllTomorrow( getSubFormula()->removeDoubleNegations() );

}

vector<Formula*> * AllTomorrow::positiveClosure(){

	vector<Formula*> * closure = getSubFormula()->positiveClosure();
	closure->push_back(this);

	return closure;

}

} /* namespace ctl_sat */
