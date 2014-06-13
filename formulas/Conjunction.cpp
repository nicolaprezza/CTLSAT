/*
 * Conjunction.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "Conjunction.h"

namespace ctl_sat {

Conjunction::Conjunction(Formula * f1, Formula * f2){

	this->f1 = (*f1<=*f2 ? f1 : f2) ;//normalize: first formula is the lexicographically smallest
	this->f2 = (*f1<=*f2 ? f2 : f1) ;

	type=CONJUNCTION;

	string_format = string("(").append(this->f1->toString()).append(")^(").append(this->f2->toString()).append(")");

};

Conjunction::~Conjunction() {}

Formula * Conjunction::removeDoubleNegations(){

	return new Conjunction(
					getLeftSubFormula()->removeDoubleNegations(),
					getRightSubFormula()->removeDoubleNegations()
			);

}

vector<Formula*> * Conjunction::positiveClosure(){

	vector<Formula*> * closure1 = getLeftSubFormula()->positiveClosure();
	vector<Formula*> * closure2 = getRightSubFormula()->positiveClosure();

	closure1->insert(closure1->end(), closure2->begin(), closure2->end());
	closure1->push_back(this);

	return closure1;

}

} /* namespace ctl_sat */
