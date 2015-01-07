/*
 * Tableau.cpp
 *
 *  Created on: May 28, 2014
 *      Author: nicola
 */

#include "Tableau.h"

namespace ctl_sat {

int sign(Formula * f){
	if(f->getType()==NEGATION)
		return -1;

	return 1;
}

Tableau::~Tableau() {}

Tableau::Tableau(Formula * f){//builds the tableau for formula f

	//firstly, positive closure set is computed. Here there are only positive formulas (i.e. not negations). formulas must be sorted and copies must be removed.
	positive_closure = f->positiveClosure();

	//sort positive closure set
	std::sort (positive_closure->begin(), positive_closure->end(), formulaLessThan);

	//remove duplicates and resize
	positive_closure->resize( std::distance(positive_closure->begin(),
			std::unique (positive_closure->begin(), positive_closure->end(), formulaEqual)) );

	cout << "Positive closure set (only non-negated formulas in the closure set):\n";

	for(uint i=0;i<positive_closure->size();i++)
		cout << (i+1) << ".  " << positive_closure->at(i)->toString()<<endl;

	cout << "\nClosure set has size " << positive_closure->size()*2<< ". \n\nBuilding initial feasible states of the tableau ... "<< endl;

	computeSubFormulas();

	formula_index orig_position = indexOf(f);//position of the original formula f in positive form in the positive closure

	if(f->getType()==NEGATION)
		initial_formula = indexToNegativeFormula(orig_position);
	else
		initial_formula = indexToPositiveFormula(orig_position);

	states = vector<state>();
	buildFeasibleStates();

	number_of_states = states.size();

	//these vectors mark formulas that are globally valid in a state (i.e. they have been previously checked and we don't need to check them again)
	globally_satisfied_formulas_positive = vector<state>(number_of_states);
	globally_satisfied_formulas_negative = vector<state>(number_of_states);

	//visited_during_mark_removal = vector<bool>(number_of_states,false);

	for(uint i=0;i<number_of_states;++i){

		globally_satisfied_formulas_positive[i] = newState();
		globally_satisfied_formulas_negative[i] = newState();

	}


	cout << "done. Initial model has " << number_of_states << " states"<<endl<<endl;

	isRemoved =  vector<bool>(states.size());//tracks removed states

	for(ulint i=0;i<states.size();i++)
		isRemoved.at(i)=false;

	status = new state_status[states.size()];
	mark_timestamps.assign(states.size(),0);

	//now build edges

	cout << "Building initial feasible edges of the tableau ... "<< endl;

	edges = vector<set<uint> >(states.size());
	back_edges = vector<set<uint> >(states.size());

	buildEdges();
	cout << "done. Initial model has " << number_of_edges << " edges"<<endl<<endl;

	//count how many states have the original formula

	number_of_states_with_original_formula=0;
	for(uint i=0;i<states.size();i++)//for all states
		if(belongsTo(states.at(i),initial_formula))//if the original formula is in the state
			number_of_states_with_original_formula++;

	//Now apply repeated cull.
	//culls are divided in 3 categories: easy, medium, hard:
	//easy: EXT, EX, ENX
	//medium: EU, ENU
	//hard: AU, ANU
	//heuristic:
	//1) firstly, apply only easy culls until it is no more possible to apply them
	//2) then, apply medium culls together with easy culls until is no more possible to apply medium/easy culls
	//3) at this point, try with the hard culls. Once a hard cull has been executed, restart from 1)
	//repeat until no more culls are possible.
	//when a state is removed, its predecessors are checked (and so on, recursively)

	cout << "Applying repeated cull ..."<< endl;
	timestamp=time(0);
	last_time_printed=(ulint)timestamp+1;
	cull();
	cout << "\ndone. Number of states left : " << number_of_states <<endl<<endl;

	/*cout << endl;
	for(uint i=0;i<states.size();i++){

		cout << "State nr " << i << "\nedges : ";

		for(uint j=0;j<edges.at(i).size();j++)
			cout << j << " ";

		cout << "\nREMOVED? " << isRemoved.at(i) << endl;

		printState(states.at(i));
		cout << endl;
	}
	cout << endl;*/

}

void Tableau::clearMarked(){

	current_timestamp=0;

	for(ulint i=0;i<states.size();i++){
		status[i] = NOT_VISITED;
		mark_timestamps[i]=0;
	}
}

void Tableau::computeSubFormulas(){

	leftSubformula = new vector<int>(positive_closure->size());
		rightSubformula = new vector<int>(positive_closure->size());

		for(uint i=0;i<positive_closure->size();i++){

			switch(positive_closure->at(i)->getType()){

				case ATOMIC: /*no subformulas*/ break;

				case NEGATION:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getSubFormula())*(indexOf(positive_closure->at(i)->getSubFormula())+1); break;

				case CONJUNCTION:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getLeftSubFormula())*(indexOf(positive_closure->at(i)->getLeftSubFormula())+1);
					rightSubformula->at(i) = sign(positive_closure->at(i)->getRightSubFormula())*(indexOf(positive_closure->at(i)->getRightSubFormula())+1); break;

				case ALL_TOMORROW:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getSubFormula())*(indexOf(positive_closure->at(i)->getSubFormula())+1); break;

				case EXISTS_TOMORROW:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getSubFormula())*(indexOf(positive_closure->at(i)->getSubFormula())+1); break;

				case ALL_UNTIL:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getLeftSubFormula())*(indexOf(positive_closure->at(i)->getLeftSubFormula())+1);
					rightSubformula->at(i) = sign(positive_closure->at(i)->getRightSubFormula())*(indexOf(positive_closure->at(i)->getRightSubFormula())+1); break;

				case EXISTS_UNTIL:
					leftSubformula->at(i) = sign(positive_closure->at(i)->getLeftSubFormula())*(indexOf(positive_closure->at(i)->getLeftSubFormula())+1);
					rightSubformula->at(i) = sign(positive_closure->at(i)->getRightSubFormula())*(indexOf(positive_closure->at(i)->getRightSubFormula())+1); break;

			}

		}

}

state Tableau::newState(){

	//each state is a vector of bools with one bit for every formula in positive_closure_set. if the bit is 1, the formula appears
	//in positive form; otherwise, it appears in negative form (negated)

	state x = vector<bool>(positive_closure->size(),false);

	return x;

}

uint Tableau::indexOf(Formula * f){//returns index of the formula f in closure_set. Linear-time, but used only at the beginning while constructing closure set

	//assumption: f in positive form is in positive_closure

	Formula * f1 = f;

	if(f->getType()==NEGATION)
		f1 = f->getSubFormula();

	uint idx=0;
	while(idx < positive_closure->size() and *(positive_closure->at(idx))!=(*f1))
		idx++;

	return idx;

}

void Tableau::printState(state B){

	for(uint i=0;i<B.size();i++){
		cout << (B.at(i)==0?"NOT ":" ") << positive_closure->at(i)->toString() <<endl;
	}

}

void Tableau::buildFeasibleStates(){

	state inserted = newState();//tracks if the formula number i has been inserted (1) or not (0)
	state polarity = newState();//tracks if the formula number i has been inserted in positive (1) or negative (0) form

	buildFeasibleStatesRecursive(inserted,polarity,0);

}

void Tableau::handleConjunction(state inserted, state polarity, formula_index formula_nr){

	formula a = leftSubformula->at(formula_nr);
	formula b = rightSubformula->at(formula_nr);

	if( not (
			belongsTo(inserted,polarity,indexToNegativeFormula(formula_nr)) or
			belongsTo(inserted,polarity,negateFormula(a)) or
			belongsTo(inserted,polarity,negateFormula(b)) or
			a == negateFormula(b) /*otherwise a and b cannot be both inserted*/
		)
	){

		//insert {a^b,a,b} and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToPositiveFormula(formula_nr));
		polarity2 = insert(polarity2,a);
		polarity2 = insert(polarity2,b);

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr)) or
			belongsTo(inserted,polarity,a) or
			belongsTo(inserted,polarity,negateFormula(b)) or
			a == b
		)
	){

		//insert {~(a^b),~a,b} and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));
		polarity2 = insert(polarity2,negateFormula(a));
		polarity2 = insert(polarity2,b);

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr)) or
			belongsTo(inserted,polarity,a) or
			belongsTo(inserted,polarity,b) or
			a == negateFormula(b)
		)
	){

		//insert {~(a^b),~a,~b} and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));
		polarity2 = insert(polarity2,negateFormula(a));
		polarity2 = insert(polarity2,negateFormula(b));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr)) or
			belongsTo(inserted,polarity,negateFormula(a)) or
			belongsTo(inserted,polarity,b) or
			a == b
		)
	){

		//insert {~(a^b),a,~b} and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));
		polarity2 = insert(polarity2,a);
		polarity2 = insert(polarity2,negateFormula(b));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

}

void Tableau::handleUntil(state inserted, state polarity, formula_index formula_nr){

	formula a = leftSubformula->at(formula_nr);
	formula b = rightSubformula->at(formula_nr);

	if( not (
			belongsTo(inserted,polarity,indexToNegativeFormula(formula_nr)) or
			belongsTo(inserted,polarity,negateFormula(a)) or
			belongsTo(inserted,polarity,negateFormula(b)) or
			a == negateFormula(b) /*otherwise a and b cannot be both inserted*/
		)
	){

		//insert {Y(aUb),a,b} (Y = A or E) and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToPositiveFormula(formula_nr));
		polarity2 = insert(polarity2,a);
		polarity2 = insert(polarity2,b);

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToNegativeFormula(formula_nr)) or
			belongsTo(inserted,polarity,a) or
			belongsTo(inserted,polarity,negateFormula(b)) or
			a == b /*otherwise ~a and b cannot be both inserted*/
		)
	){

		//insert {Y(aUb),~a,b} (Y = A or E) and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToPositiveFormula(formula_nr));
		polarity2 = insert(polarity2,negateFormula(a));
		polarity2 = insert(polarity2,b);

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToNegativeFormula(formula_nr)) or
			belongsTo(inserted,polarity,negateFormula(a)) or
			belongsTo(inserted,polarity,b) or
			a == b /*otherwise a and ~b cannot be both inserted*/
		)
	){

		//insert {Y(aUb),a,~b} (Y = A or E) and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToPositiveFormula(formula_nr));
		polarity2 = insert(polarity2,a);
		polarity2 = insert(polarity2,negateFormula(b));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr)) or
			belongsTo(inserted,polarity,a) or
			belongsTo(inserted,polarity,b) or
			a == negateFormula(b) /*otherwise ~a and ~b cannot be both inserted*/
		)
	){

		//insert {~Y(aUb),~a,~b} (Y = A or E) and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));
		polarity2 = insert(polarity2,negateFormula(a));
		polarity2 = insert(polarity2,negateFormula(b));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr)) or
			belongsTo(inserted,polarity,negateFormula(a)) or
			belongsTo(inserted,polarity,b) or
			a == b
		)
	){

		//insert {~Y(aUb),a,~b} (Y = A or E) and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;
		inserted2.at(formulaToIndex(a)) = true;
		inserted2.at(formulaToIndex(b)) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));
		polarity2 = insert(polarity2,a);
		polarity2 = insert(polarity2,negateFormula(b));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

}

void Tableau::handleOtherCases(state inserted, state polarity, formula_index formula_nr){

	if( not (
			belongsTo(inserted,polarity,indexToPositiveFormula(formula_nr))
		)
	){

		//insert ~a and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;

		polarity2 = insert(polarity2,indexToNegativeFormula(formula_nr));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}

	if( not (
			belongsTo(inserted,polarity,indexToNegativeFormula(formula_nr))
		)
	){

		//insert a and call recursively
		state inserted2 = inserted;
		state polarity2 = polarity;

		inserted2.at(formula_nr) = true;

		polarity2 = insert(polarity2,indexToPositiveFormula(formula_nr));

		buildFeasibleStatesRecursive(inserted2, polarity2, formula_nr+1);

	}
}

void Tableau::buildFeasibleStatesRecursive(state inserted, state polarity, formula_index formula_nr){

	if(formula_nr>=positive_closure->size()){
		states.push_back(polarity);

		if(states.size()%1000==0){
			cout << " " << states.size() << " feasible states created until now\n";
		}

		return;
	}

	switch(positive_closure->at(formula_nr)->getType()){

		case CONJUNCTION: //a^b. create new states inserting the combinations: {a^b,a,b},{~(a^b),~a,b},{~(a^b),~a,~b},{~(a^b),a,~b}
			handleConjunction(inserted, polarity, formula_nr); break;

		case ALL_UNTIL://A(aUb). create new states inserting the combinations: {A(aUb),a,b},{A(aUb),~a,b},{A(aUb),a,~b},{~A(aUb),~a,~b},{~A(aUb),a,~b}
			handleUntil(inserted, polarity, formula_nr); break;

		case EXISTS_UNTIL://E(aUb). create new states inserting the combinations: {E(aUb),a,b},{E(aUb),~a,b},{E(aUb),a,~b},{~E(aUb),~a,~b},{~E(aUb),a,~b}
			handleUntil(inserted, polarity, formula_nr); break;

		default://other cases: insert the combinations {a} and {~a}
			handleOtherCases(inserted, polarity, formula_nr); break;

	}

}

bool Tableau::checkEdgeConditionsType1(ulint i, ulint j, formula_index k){

	//		1.1) i(k) = AXa -> a must be in j
	//		1.2) i(k) = A~Xa = ~EXa -> ~a must be in j
	//		1.3) i(k) = A(aUb) AND ~b in i -> A(aUb) must be in j
	//		1.4) i(k) = A~(aUb) = ~E(aUb) AND a in i -> A~(aUb) = ~E(aUb) must be in j

	switch(positive_closure->at(k)->getType()){

		case ALL_TOMORROW: /*case 1.1*/

			if(states.at(i).at(k)){//AXa is present in positive form inside i

				formula a = leftSubformula->at(k);//extract subformula a of AXa

				if( not belongsTo(states.at(j),a) )//a must be in j
					return false;

			}

		break;

		case EXISTS_TOMORROW: /*case 1.2*/

			if(not states.at(i).at(k)){//~EXa is present inside i (at position k). i(k)=~EXa

				formula a = leftSubformula->at(k);//extract subformula a of ~EXa

				if( not belongsTo(states.at(j),negateFormula(a) ) )//~a must be in j
					return false;

			}

		break;

		case ALL_UNTIL: /*case 1.3*/

			if(states.at(i).at(k)){//A(aUb) is present in positive form inside i

				formula b = rightSubformula->at(k);//extract subformula b of A(aUb)

				if( belongsTo(states.at(i),negateFormula(b)) ){//~b is in i

					if( not belongsTo(states.at(j),indexToPositiveFormula(k)) )//A(aUb) must be in j
						return false;

				}

			}

		break;

		case EXISTS_UNTIL: /*case 1.4*/

			if(not states.at(i).at(k)){//~E(aUb) is present inside i (at position k). i(k)=~E(aUb)

				formula a = leftSubformula->at(k);//extract left subformula a of ~E(aUb)

				if( belongsTo(states.at(i),a) ){//a is inside i

					if( not belongsTo(states.at(j),indexToNegativeFormula(k) ) )//~E(aUb) must be in j
						return false;

				}

			}

		break;

		default: return true; break;

	}


	return true;

}

bool Tableau::checkEdgeConditionsType2(ulint i, ulint j, formula_index k){

	//2) formula present in j -> check i
	//		2.1) j(k) = E(aUb) AND a in i -> E(aUb) must be in i
	//		2.2) j(k) = E~(aUb) = ~A(aUb) AND ~b in i -> E~(aUb) = ~A(aUb) must be in i

	switch(positive_closure->at(k)->getType()){

		case EXISTS_UNTIL:/*case 2.1*/

			if(states.at(j).at(k)){//E(aUb) is present in positive form inside j. j(k)=E(aUb)

				formula a = leftSubformula->at(k);//extract left subformula a of E(aUb)

				if( belongsTo(states.at(i),a) ){//a is in i

					if(not belongsTo(states.at(i),indexToPositiveFormula(k)) )//E(aUb) must be in i
						return false;

				}

			}

		break;

		case ALL_UNTIL:/*case 2.2*/

			if(not states.at(j).at(k)){//~A(aUb) is present inside j. j(k)=~A(aUb)

				formula b = rightSubformula->at(k);//extract right subformula b of ~A(aUb)

				if( belongsTo(states.at(i), negateFormula(b)) ){//~b is inside i

					if( not belongsTo(states.at(i),indexToNegativeFormula(k) ) )//~A(aUb) must be in i
						return false;

				}

			}

		break;

		default: return true; break;

	}

	return true;

}

void Tableau::buildEdges(){

	number_of_edges = 0;

	for(ulint i=0;i<states.size();i++){

		for(ulint j=0;j<states.size();j++){//for each pair (i,j) of states

			//2 types of conditions:
			//1) formula present in i -> check j
			//		1.1) AXa in i -> a must be in j
			//		1.2) A~Xa = ~EXa in i -> ~a must be in j
			//		1.3) { A(aUb), ~b } in i -> A(aUb) must be in D
			//		1.4) { A~(aUb) = ~E(aUb), a } in i -> A~(aUb) = ~E(aUb) must be in D
			//2) formula present in j -> check i
			//		2.1) E(aUb) in j AND a in i -> E(aUb) must be in i
			//		2.2) E~(aUb) = ~A(aUb) in j AND ~b in i -> E~(aUb) = ~A(aUb) must be in i

			bool conditions_passed=true;

			//check conditions of type 1
			for(formula_index k=0;conditions_passed and k<positive_closure->size();k++)//for each formula in i
				conditions_passed &= checkEdgeConditionsType1(i,j,k);

			//check conditions of type 2
			if(conditions_passed)
				for(formula_index k=0;conditions_passed and k<positive_closure->size();k++)//for each formula in j
					conditions_passed &= checkEdgeConditionsType2(i,j,k);

			if(conditions_passed){//add edge i->j and back_edge j->i

				edges.at(i).insert(j);
				back_edges.at(j).insert(i);
				number_of_edges++;

				if(number_of_edges%50000==0)
					cout << " " << number_of_edges << " feasible edges created until now\n";

			}

		}

	}

}

void Tableau::cull(){

	bool verbose = false;
	uint states_removed = 1;//to enter in the while loop

	while(isSatisfiable() and states_removed>0){//repeat cull until there exists a state where the original formula is satisfiable

		if(verbose) cout << " Checking EX, EXT, ENX conditions ... ";
		states_removed = cullEasy();
		if(verbose) cout << states_removed << " states removed.\n";

		if(states_removed==0){

			if(verbose) cout << " All states satisfy EX, EXT, ENX conditions. Checking EU and ENU conditions ... ";
			states_removed = cullMedium();
			if(verbose) cout << states_removed << " states removed.\n";

			if(states_removed==0){
				if(verbose) cout << " All states satisfy EX, EXT, ENX, EU, and ENU conditions. Checking AU and ANU conditions ... ";
				states_removed = cullHard();
				if(verbose) cout << states_removed << " states removed.\n";
			}
		}


	}

}

uint Tableau::cullEasy(){

	uint initial_nr_of_states = number_of_states;

	//1) check iteratively easy conditions: EXT, EX, ENX

	//search a state that falsifies at least one easy condition and cull it recursively backtracking its back edges
	for(uint i=0;i<states.size();i++)//for all states
		if(not isRemoved.at(i)){//if state is present in the model

			if(not checkEasy(i))
				cullEasyRecursive(i);

		}

	return initial_nr_of_states-number_of_states;//return number of states removed

}

uint Tableau::cullMedium(){

	uint initial_nr_of_states = number_of_states;

	for(uint i=0; i<states.size();i++)//for all states
		if(not isRemoved.at(i)){//if state is present in the model

			if(not checkMedium(i))
				cullEasyRecursive(i);

		}

	return initial_nr_of_states-number_of_states;//return number of states removed

}

uint Tableau::cullHard(){

	uint initial_nr_of_states = number_of_states;

	for(uint i=0; i<states.size();i++)//for all states
		if(not isRemoved.at(i)){//if state is present in the model

			if(not checkHard(i))
				cullEasyRecursive(i);

		}

	return initial_nr_of_states-number_of_states;//return number of states removed

}

void Tableau::cullEasyRecursive(uint i){

	removeState(i);//remove state, all its forward edges (not back edges) and remove the state i from the adj lists of all its neighbors

	for(std::set<uint>::iterator it = back_edges.at(i).begin(); it != back_edges.at(i).end();++it){//for each predecessor of i

		if(not checkEasy(*it) and not isRemoved.at(*it))
			cullEasyRecursive(*it);

	}

}

void Tableau::removeGlobalMarks(uint i){

	for(uint j=0;j<visited_during_mark_removal.size();++j)
		visited_during_mark_removal[j]=false;

	removeGlobalMarksRecursive(i);

}

void Tableau::removeGlobalMarksRecursive(uint i){

	globally_satisfied_formulas_positive[i] = newState();
	globally_satisfied_formulas_negative[i] = newState();

	visited_during_mark_removal[i] = true;

	std::set<uint>::iterator it;

	//loop through states that have an edge towards i and clear their global marks
	for(it = back_edges.at(i).begin(); it != back_edges.at(i).end();++it)
		if(not visited_during_mark_removal[*it])
			removeGlobalMarksRecursive(*it);

}

void Tableau::removeState(uint i){//removes state i and all entering and exiting edges

	if(isRemoved.at(i)){//already removed

		cout << "WARNING: removing state already removed ("<<i<<")\n";
		return;

	}

	//if initial formula is in this state, decrease counter of states having the original formula.
	if(belongsTo(states.at(i),initial_formula)){

		if(number_of_states_with_original_formula==0)
			cout << "WARNING: trying to decrease counter with zero value (number of states with original formula). Debug needed!\n";
		else
			number_of_states_with_original_formula--;

	}

	number_of_states--;

	if(number_of_states%100==0)
		cout << "  " << number_of_states << " states left\n";

	/*if((ulint)difftime( time(0), timestamp)%5==0 and (ulint)difftime( time(0), timestamp) != last_time_printed){
		last_time_printed = (ulint)difftime( time(0), timestamp);
		cout << "  " << number_of_states << " states left\n";
	}*/


	isRemoved.at(i) = true;//mark as removed

	//removeGlobalMarks(i);//backtrack on predecessors and erase their global marks

	std::set<uint>::iterator it;

	//loop through states that have an edge towards i and remove i from their adjacency list
	for(it = back_edges.at(i).begin(); it != back_edges.at(i).end();++it)
		edges.at(*it).erase(i);

	//loop through states in the adj list of i and remove i from their back adj list
	for(it = edges.at(i).begin(); it != edges.at(i).end();++it)
		back_edges.at(*it).erase(i);

	//free memory of state i
	edges.at(i).clear();
	//back_edges.at(i).clear();//back_edges are not removed because they have to be used to backtrack cull effects

}

bool Tableau::checkEasy(ulint i){

	return checkEXT(i) and checkEX(i) and checkENX(i);

}

bool Tableau::checkMedium(ulint i){

	return checkEU(i) and checkENU(i);

}

bool Tableau::checkHard(ulint i){

	return checkAU(i) and checkANU(i);

}

bool Tableau::checkEXT(ulint i){

	//state i must have at least one successor
	return edges.at(i).size()>0;

}

bool Tableau::checkEX(ulint i){

	//if EXa is in i, then at least one successor of i must have a

	bool all_exists_have_successor=true;

	for(formula_index k = 0; all_exists_have_successor and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in positive form AND k is a EXa (i.e. for all EXa inside i)
		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_TOMORROW){

			formula a = leftSubformula->at(k);//get subformula of EXa

			std::set<uint>::iterator it;
			bool exists_successor=false;

			//loop through states in the adj list of i.
			for(it = edges.at(i).begin(); (not exists_successor) and it != edges.at(i).end();++it)
				exists_successor |= belongsTo(states.at(*it),a);//a must be in at least one successor of i

			all_exists_have_successor &= exists_successor;

		}

	}

	return all_exists_have_successor;

}

bool Tableau::checkENX(ulint i){

	//if EX~a = E~Xa = ~AXa is in i, then at least one successor of i must have ~a

	bool all_neg_exists_have_successor=true;

	for(formula_index k = 0; all_neg_exists_have_successor and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in negative form AND k is a AXa (i.e. for all ~AXa inside i)
		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_TOMORROW){

			formula not_a = negateFormula(leftSubformula->at(k));//get subformula a of AXa and negate it, obtaining ~a

			bool exists_successor=false;

			//loop through states in the adj list of i.
			for(std::set<uint>::iterator it = edges.at(i).begin(); (not exists_successor) and it != edges.at(i).end();++it)
				exists_successor |= belongsTo(states.at(*it),not_a);//~a must be in at least one successor of i

			all_neg_exists_have_successor &= exists_successor;

		}

	}

	return all_neg_exists_have_successor;

}

bool Tableau::checkEU(ulint i){

	//for each E(aUb): find a path starting from i that has always a until finally b is found
	//no loops back: mark states

	bool all_EU_satisfied = true;

	for(formula_index k = 0; all_EU_satisfied and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in positive form AND k is a E(aUb) (i.e. for all E(aUb) inside i)
		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_UNTIL){

			formula a = leftSubformula->at(k);
			formula b = rightSubformula->at(k);

			clearMarked();//unmark all states
			all_EU_satisfied &= checkEUrecursive(i,k,a,b);//start visit (DFS)

		}

	}

	return all_EU_satisfied;

}

bool Tableau::checkAU(ulint i){

	//for each A(aUb): find a tree substructure having always a and the leafs contain b and consistent with EX, E~X, EU, E~U
	//no loops back: mark states

	bool all_AU_satisfied = true;

	for(formula_index k = 0; all_AU_satisfied and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in positive form AND k is a A(aUb) (i.e. for all A(aUb) inside i)
		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==ALL_UNTIL){

			formula a = leftSubformula->at(k);
			formula b = rightSubformula->at(k);

			clearMarked();//unmark all states
			all_AU_satisfied &= checkAUrecursive(i,a,b);//start visit (DFS)

		}

	}

	return all_AU_satisfied;

}

bool Tableau::checkAUrecursive(ulint i,formula a, formula b){

	//b is in this state: return true
	if(belongsTo(states.at(i),b)){
		status[i] = SATISFIED;
		return true;
	}

	//here we know that b is not in i

	if(not belongsTo(states.at(i),a)){//a and b are not in i: return false
		status[i] = NOT_SATISFIED;
		return false;
	}

	status[i] = VISITED;//mark status as visited to prevent loops

	//here we are sure that the state contains a AND does not contain b: check AU rule

	//1) visit recursively successors and check the A(aUb) rule

	uint sat_successors=0;//number of satisfied successors

	for(std::set<uint>::iterator it = edges.at(i).begin(); it != edges.at(i).end();++it){//for all non-visited successors of i

		if(status[*it]==NOT_VISITED)
			checkAUrecursive(*it,a, b);

		if(status[*it]==SATISFIED)
			sat_successors++;

	}

	//now each successor is marked as SATISFIED (A(aUb) satisfied), NOT_SATISFIED (A(aUb) not satisfied) or
	//VISITED (loop: A(aUb) not satisfied because there exists a loop where a is always valid, but b never)

	//extract all EX, E~X, EU, E~U rules present in state i

	vector<formula> existential_formulas;//existential formulas inside state i

	for(formula_index k = 0; k<positive_closure->size();k++){//for each formula in i

		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_TOMORROW)//EX
			existential_formulas.push_back(indexToPositiveFormula(k));

		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_TOMORROW)//~AX = E~X
				existential_formulas.push_back(indexToNegativeFormula(k));

		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_UNTIL)//EU
			existential_formulas.push_back(indexToPositiveFormula(k));

		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_UNTIL)//~AU = E~U
			existential_formulas.push_back(indexToNegativeFormula(k));

	}

	//now, 2 cases:
	//1) if existential_formulas not empty: for each E formula in existential_formulas, check that there exist a successor marked SATISFIED that satisfies the E formula
	//1) if existential_formulas is empty: there must be at least one successor marked SATISFIED

	bool E_satisfied = true;

	for(uint k=0;E_satisfied and k<existential_formulas.size();k++){//for each E formula

		bool exist_good_successor = false;//exists a successor marked SATISFIED that satisfies the E formula

		for(std::set<uint>::iterator it = edges.at(i).begin(); (not exist_good_successor) and it != edges.at(i).end();++it){//for all successors of i

			if(status[*it]==SATISFIED)//only for successors marked SATISFIED
				exist_good_successor |= checkEformula(existential_formulas.at(k),i,*it);//check that the E formula is sat in i->*it

		}

		E_satisfied &= exist_good_successor;

	}

	if(E_satisfied and sat_successors>0)
		status[i] = SATISFIED;
	else
		status[i] = NOT_SATISFIED;

	return status[i] == SATISFIED;

}

bool Tableau::checkANU(ulint i){

	bool all_ANU_satisfied = true;

	for(formula_index k = 0; all_ANU_satisfied and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in negative form AND k is a E(aUb) (i.e. for all A~(aUb) inside i)
		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==EXISTS_UNTIL){

			formula a = leftSubformula->at(k);
			formula b = rightSubformula->at(k);

			clearMarked();//unmark all states
			all_ANU_satisfied &= checkANUrecursive(i,a,b);//start visit (DFS)

		}

	}

	return all_ANU_satisfied;

}

bool Tableau::checkANUrecursive(ulint i,formula a, formula b){

	current_timestamp++;
	mark_timestamps[i] = current_timestamp;

	//b is in this state: return false
	if(belongsTo(states.at(i),b)){
		status[i] = NOT_SATISFIED;
		return false;
	}

	//here we know that b is not in i

	if(not belongsTo(states.at(i),a)){//a and b are not in i: return true (A~(aUb) satisfied in this state)
		status[i] = SATISFIED;
		return true;
	}

	//here we know that b is NOT in i and a is in i

	status[i] = VISITED;//mark status as visited

	//1) visit recursively successors and check the A~(aUb) rule

	for(std::set<uint>::iterator it = edges.at(i).begin(); it != edges.at(i).end();++it){//for all non-visited successors of i

		if(status[*it]==NOT_VISITED)
			checkANUrecursive(*it,a, b);

	}

	//now each successor is marked as SATISFIED (A~(aUb) satisfied), NOT_SATISFIED (A~(aUb) not satisfied) or
	//VISITED (loop)

	//extract all EX, E~X, EU, E~U rules present in state i: sub-structure must be consistent

	vector<formula> existential_formulas;//existential formulas inside state i

	for(formula_index k = 0; k<positive_closure->size();k++){//for each formula in i

		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_TOMORROW)//EX
			existential_formulas.push_back(indexToPositiveFormula(k));

		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_TOMORROW)//~AX = E~X
				existential_formulas.push_back(indexToNegativeFormula(k));

		if( belongsTo(states.at(i),indexToPositiveFormula(k)) and positive_closure->at(k)->getType()==EXISTS_UNTIL)//EU
			existential_formulas.push_back(indexToPositiveFormula(k));

		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_UNTIL)//~AU = E~U
			existential_formulas.push_back(indexToNegativeFormula(k));

	}

	bool E_satisfied = true;//all existential formulas satisfied

	for(uint k=0;E_satisfied and k<existential_formulas.size();k++){//for each E formula

		bool exist_good_successor = false;//exists a successor marked SATISFIED or VISITED (loop) that satisfies the E formula

		for(std::set<uint>::iterator it = edges.at(i).begin(); (not exist_good_successor) and it != edges.at(i).end();++it){//for all successors of i

			if(status[*it]==SATISFIED or status[*it]==VISITED)//only for successors marked SATISFIED or VISITED
				exist_good_successor |= checkEformula(existential_formulas.at(k),i,*it);//check that the E formula is sat in i->*it

		}

		E_satisfied &= exist_good_successor;

	}

	if(E_satisfied)
		status[i] = SATISFIED;
	else{

		status[i] = NOT_SATISFIED;

		//in this case, erase all marks made after the timestamp of this state because some assumptions made while
		//accepting loops may no more be valid (e.g. if a loop turned back to this state, now this state is not satisfied so the loop should not
		//have been accepted)

		for(ulint s=0;s<states.size();s++){

			if(mark_timestamps[s]>mark_timestamps[i]){//this status has been computed after the current node: erase it
				mark_timestamps[s]=0;
				status[s] = NOT_VISITED;
			}

		}

	}

	return status[i] == SATISFIED;

}


bool Tableau::checkEformula(formula f, ulint s1, ulint s2){//check if existential formula f is valid in the states s1->s2

	//assumption: f is either EX, ~AX, EU or ~AU

	formula_index k = formulaToIndex(f);
	formula a,b;

	switch(positive_closure->at(k)->getType()){

		case EXISTS_TOMORROW: /*f is EXa: s2 must contain a*/

			a = leftSubformula->at(k);
			return belongsTo(states.at(s2),a);

		break;

		case ALL_TOMORROW: /*f is E~Xa: s2 must contain ~a*/

			a = leftSubformula->at(k);
			return belongsTo(states.at(s2),negateFormula(a));

		break;

		case EXISTS_UNTIL: /*f is E(aUb): either b must be in s1 or E(aUb) must be in s2*/

			b = rightSubformula->at(k);

			if(belongsTo(states.at(s1),b))
				return true;

			//b is not in s1: check that E(aUb) is in s2

			return belongsTo(states.at(s2),f);

		break;

		case ALL_UNTIL: /* f is E~(aUb): by construction, ~b is in s1. either ~a must be in s1 or E~(aUb) must be in s2*/

			a = leftSubformula->at(k);

			if(belongsTo(states.at(s1),negateFormula(a)))
				return true;

			//s1 contains a and ~b. E~(aUb) must be in s2

			return belongsTo(states.at(s2),f);

		break;

		default: cout << "WARNING: unexpected formula in function checkEformula\n. Debug needed!\n"; return false; break;

	}

	return false;

}

//parameters: state, index of the EU formula, left and right subformulas
bool Tableau::checkEUrecursive(ulint i,formula_index k, formula a, formula b){

	//check if the state had already been checked in the past for this formula
	//if(globally_satisfied_formulas_positive[i][k])
	//	return true;

	//find a path starting from i that has always a until finally b is found (DFS visit)
	//no loops back: mark states

	status[i] = VISITED;
	//marked->at(i) = true;//mark this state

	if(belongsTo(states.at(i),b)){//b is in this state: mark globally and return success
		//globally_satisfied_formulas_positive[i][k] = true;
		return true;
	}

	if(not belongsTo(states.at(i),a))//a is not in this state (and neither b): return false
		return false;

	//b is not in this state and a is in this state. return success iif at least one non-visited successor returns success

	bool path_found = false;

	//first of all, check if there are successors marked globally (avoid recursion if possible)

	/*for(std::set<uint>::iterator it = edges.at(i).begin(); (not path_found) and it != edges.at(i).end();++it){//for all successors of i

		if(status[*it] == NOT_VISITED){//if the successor is not marked

			path_found |= globally_satisfied_formulas_positive[*it][k];

		}

	}*/

	//then, recurse only if no successors were already marked globally

	for(std::set<uint>::iterator it = edges.at(i).begin(); (not path_found) and it != edges.at(i).end();++it){//for all successors of i

		if(status[*it] == NOT_VISITED){//if the successor is not marked

			path_found |= checkEUrecursive(*it,k,a,b);

		}

	}

	//if(path_found)
		//globally_satisfied_formulas_positive[i][k] = true;

	return path_found;

}

bool Tableau::checkENU(ulint i){

	//for each E~(aUb) = ~A(aUb): find a path starting from i that has always ~b until finally ~b and ~a is found, OR
	//the path has always ~b and at some point it loops

	clearMarked();//unmark all states

	bool all_ENU_satisfied = true;

	for(formula_index k = 0; all_ENU_satisfied and k<positive_closure->size();k++){//for each formula in i

		//if k is inside i in negative form AND k is a A(aUb) (i.e. for all E~(aUb) inside i)
		if( belongsTo(states.at(i),indexToNegativeFormula(k)) and positive_closure->at(k)->getType()==ALL_UNTIL){

			formula a = leftSubformula->at(k);
			formula b = rightSubformula->at(k);

			all_ENU_satisfied &= checkENUrecursive(i,a,b);//start visit (DFS). note that states are all unmarked after call is finished (no need to call clearMarked())

		}

	}

	return all_ENU_satisfied;

}

bool Tableau::checkENUrecursive(ulint i,formula a, formula b){

	if(belongsTo(states.at(i),b))//b is in this state: return false
		return false;

	//here we are sure that ~b is in this state

	if(belongsTo(states.at(i),negateFormula(a)))//~a is in this state return true
		return true;

	status[i] = VISITED;
	//marked->at(i) = true;//mark this state

	//here we know that the state contains ~b and a

	bool path_found = false;

	for(std::set<uint>::iterator it = edges.at(i).begin(); (not path_found) and it != edges.at(i).end();++it){//for all successors of i

		if(status[*it] == NOT_VISITED){//if the successor is not marked: call recursive DFS

			path_found |= checkENUrecursive(*it,a,b);

		}else{//the successor is marked: since we are unmarking states returning from recursion, this means that we found a loop

			return true;

		}

	}

	status[i] = NOT_VISITED;
	//marked->at(i) = false;//unmark this state

	return path_found;

}

bool Tableau::isSatisfiable(){//true iif f admits a model

	return number_of_states_with_original_formula>0;

}


} /* namespace ctl_sat */



