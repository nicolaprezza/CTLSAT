/*
 * Formula.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef FORMULA_H_
#define FORMULA_H_

#include "../common.h"

namespace ctl_sat {

class Formula {
public:
	Formula();
	virtual ~Formula();

	formula_type getType(){return type;}

	virtual string toString(){return string_format;};

	virtual Formula * getSubFormula(){return NULL;};
	virtual Formula * getLeftSubFormula(){return NULL;};
	virtual Formula * getRightSubFormula(){return NULL;};

	virtual Formula * removeDoubleNegations(){return NULL;};

	virtual vector<Formula*> * positiveClosure(){return NULL;};//returns set of all positive subformulas (i.e. that are not negations)

	inline bool operator<(const Formula & f) const { return string_format.compare(f.string_format)<0; };
	inline bool operator<=(const Formula & f) const { return string_format.compare(f.string_format)<=0; };
	const bool operator>(const Formula & f) const { return string_format.compare(f.string_format)>0; };
	const bool operator==(const Formula & f) const { return string_format.compare(f.string_format)==0; };
	const bool operator!=(const Formula & f) const { return string_format.compare(f.string_format)!=0; };

	string string_format;
protected:

	formula_type type;


};

} /* namespace ctl_sat */
#endif /* FORMULA_H_ */
