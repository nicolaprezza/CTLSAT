/*
 * ExistsTomorrow.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "ExistsTomorrow.h"

namespace ctl_sat {

ExistsTomorrow::ExistsTomorrow(Formula * f){

	this->f=f;
	type=EXISTS_TOMORROW;

	string_format = (string("EX(").append(f->toString())).append(")");

};

ExistsTomorrow::~ExistsTomorrow() {}

Formula * ExistsTomorrow::removeDoubleNegations(){

	return new ExistsTomorrow( getSubFormula()->removeDoubleNegations() );

}

vector<Formula*> * ExistsTomorrow::positiveClosure(){

	vector<Formula*> * closure = getSubFormula()->positiveClosure();
	closure->push_back(this);

	return closure;

}

} /* namespace ctl_sat */
