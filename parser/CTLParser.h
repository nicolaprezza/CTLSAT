/*
 * CTLParser.h
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#ifndef CTLPARSER_H_
#define CTLPARSER_H_

#include "../common.h"

#include "../formulas/Formula.h"
#include "../formulas/Conjunction.h"
#include "../formulas/Negation.h"
#include "../formulas/Atom.h"
#include "../formulas/ExistsUntil.h"
#include "../formulas/AllUntil.h"
#include "../formulas/AllTomorrow.h"
#include "../formulas/ExistsTomorrow.h"

namespace ctl_sat {

class CTLParser {
public:

	CTLParser();
	virtual ~CTLParser();

	Formula * parse(string f);

private:

	//shortcuts to build the truth
	static Formula * T();

	//shortcuts to build AF,AG,EF,EG formulas
	static Formula * AF(Formula * f);
	static Formula * AG(Formula * f);
	static Formula * EF(Formula * f);
	static Formula * EG(Formula * f);

	string preProcess(string f);

	Formula * parseRecursive(string f, uint * idx, char nt);

	Formula * applyRule1(stack <char> * stck, string f, uint * idx);
	Formula * applyRule3(stack <char> * stck, string f, uint * idx);
	Formula * applyRule4(stack <char> * stck, string f, uint * idx);
	Formula * applyRule5(stack <char> * stck, string f, uint * idx);
	Formula * applyRule6(stack <char> * stck, string f, uint * idx);
	Formula * applyRule7(stack <char> * stck, string f, uint * idx);
	Formula * applyRule8(stack <char> * stck, string f, uint * idx);
	Formula * applyRule9(stack <char> * stck, string f, uint * idx);
	Formula * applyRule10(stack <char> * stck, string f, uint * idx);
	Formula * applyRule11(stack <char> * stck, string f, uint * idx);
	Formula * applyRule13(stack <char> * stck, Formula * last_formula, string f, uint * idx);
	Formula * applyRule14(stack <char> * stck, Formula * last_formula, string f, uint * idx);
	Formula * applyRule15(stack <char> * stck, Formula * last_formula, string f, uint * idx);
	Formula * applyRule16(stack <char> * stck, string f, uint * idx);
	Formula * applyRule18(stack <char> * stck, string f, uint * idx);
	Formula * applyRule19(stack <char> * stck, string f, uint * idx);
	Formula * applyRule20(stack <char> * stck, string f, uint * idx);
	Formula * applyRule21(stack <char> * stck, string f, uint * idx);
	Formula * applyRule22(stack <char> * stck, string f, uint * idx);
	Formula * applyRule23(stack <char> * stck, string f, uint * idx);
	Formula * applyRule24(stack <char> * stck, string f, uint * idx);
	Formula * applyRule25(stack <char> * stck, string f, uint * idx);
	Formula * applyRule26(stack <char> * stck, string f, uint * idx);

	unsigned int ** parsing_table;
	bool * isAtom;

	bool * isTerminal;

};

} /* namespace ctl_sat */
#endif /* CTLPARSER_H_ */
