/*
 * Negation.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "Negation.h"

namespace ctl_sat {

Negation::Negation(Formula * f){

	this->f=f;
	type=NEGATION;

	string_format = (string("~(").append(f->toString())).append(")");

};

Negation::~Negation() {}

Formula * Negation::removeDoubleNegations(){

	switch(getSubFormula()->getType()){
		case NEGATION: return getSubFormula()->getSubFormula()->removeDoubleNegations(); break;
		default: return new Negation( getSubFormula()->removeDoubleNegations() ) ; break;
	}

}

vector<Formula*> * Negation::positiveClosure(){

	return getSubFormula()->positiveClosure();

}

} /* namespace ctl_sat */


