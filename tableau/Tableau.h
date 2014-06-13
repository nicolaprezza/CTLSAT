/*
 * Tableau.h
 *
 *  Created on: May 28, 2014
 *      Author: nicola
 */

#ifndef TABLEAU_H_
#define TABLEAU_H_

#include "../common.h"
#include "../formulas/Formula.h"
#include "../formulas/Negation.h"
#include <algorithm>    // std::sort

namespace ctl_sat {

class Tableau {
public:
	Tableau(Formula * f);//builds the tableau for formula f
	bool isSatisfable();//true iif f admits a model

	uint numberOfStates(){return number_of_states;}//at each phase of the construction, this is the current number of states left.

	virtual ~Tableau();

private:

	struct FormulaLessThan {
	  bool operator() (Formula * f1,Formula * f2) { return *f1 < *f2; }
	} formulaLessThan;

	struct FormulaEqual {
	  bool operator() (Formula * f1,Formula * f2) { return *f1 == *f2; }
	} formulaEqual;

	// STEPS TO BUILD TABLEAU:

	void computeSubFormulas();

	void buildFeasibleStates();
	void buildFeasibleStatesRecursive(state inserted, state polarity, formula_index formula_nr);

	void handleConjunction(state inserted, state polarity, formula_index formula_nr);
	void handleUntil(state inserted, state polarity, formula_index formula_nr);
	void handleOtherCases(state inserted, state polarity, formula_index formula_nr);

	bool checkEdgeConditionsType1(ulint i, ulint j, uint k);
	bool checkEdgeConditionsType2(ulint i, ulint j, uint k);

	void buildEdges();

	void cull();

	uint cullEasy();
	void cullEasyRecursive(uint i);
	// -----------------------

	//check cull rules:
	bool checkEXT(ulint i);//i=index of state
	bool checkEX(ulint i);//i=index of state
	bool checkENX(ulint i);//i=index of state
	bool checkEasy(ulint i);

	void printState(state B);

	uint indexOf(Formula * f);//returns index of the formula f in closure_set


	//the type 'formula' is an integer representation of formulas in the closure set.
	//if a is an index in positive closure, then (a+1) is the corresponding formula in positive version,
	//while -(i+1) is the corresponding formula in negative version.
	static formula indexToPositiveFormula(formula_index i){	return i+1; }
	static formula indexToNegativeFormula(formula_index i){	return -(i+1);	}
	static formula_index formulaToIndex(formula i){

		if(i<0)
			return -i-1;

		return i-1;

	}
	static formula negateFormula(formula i){

		bool positive = i>0;

		if(positive)
			return indexToNegativeFormula(formulaToIndex(i));

		return indexToPositiveFormula(formulaToIndex(i));

	}
	static bool belongsTo(state B, formula i){

		if(i<0)
			return B.at(formulaToIndex(i))==0;

		return B.at(formulaToIndex(i))==1;

	}
	static bool belongsTo(state inserted, state polarity, formula i){

		if(not inserted.at(formulaToIndex(i)))
			return false;

		if(i<0)
			return polarity.at(formulaToIndex(i))==false;

		return polarity.at(formulaToIndex(i))==true;

	}
	static state insert(state B, formula i){

		if(i<0)
			B.at(formulaToIndex(i)) = false;
		else
			B.at(formulaToIndex(i)) = true;

		return B;

	}

	void addEdge(uint i,uint j);//adds edge i->j and back-edge j->i

	state newState();//returns a new state (i.e. vector of bool of length |positive_closure_set|)

	void removeState(uint i);//removes state i and all entering and exiting edges

	vector<Formula*> * positive_closure;//the formulas in lexicographic order. NB:only non-negated formulas are here (negated versions are implicit)

	//for each i=0,...,positive_closure->size, memorize  sign*(index+1), where index is position of the positive subformula in positive_closure
	//negative sign means that formula is negated
	vector<formula> * leftSubformula;
	vector<formula> * rightSubformula;

	//each state S is a vector of bool of length |closure_set|
	//if S(i)=true, then closure_set(i) is inside S
	//if S(i)=false, then ~closure_set(i) is inside S
	vector<state> * states;

	ulint number_of_states;
	ulint number_of_edges;

	//edges: edges(i) is the list i_1,i_2,...,i_m such that there exists an edge from state i to each of the states i_1,i_2,...,i_m
	vector<set<uint> > * edges;

	//back_edges: back_edges(i) is the list i_1,i_2,...,i_m such that there exists an edge from each of the states i_1,i_2,...,i_m to the state i
	//introduced to backtrack effect of the cull
	vector<set<uint> > * back_edges;

	//removed(i) = state in position i has been removed
	vector<bool> * isRemoved;

	formula initial_formula;

};

} /* namespace ctl_sat */
#endif /* TABLEAU_H_ */
