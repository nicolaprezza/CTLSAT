/*
 * CTLParser.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "CTLParser.h"

namespace ctl_sat {

CTLParser::CTLParser() {

	//initialize parsing table

	parsing_table = new uint * [256];
	for(uint i=0;i<256;i++)
		parsing_table[i] = new uint[256];

	/*
	  	RULES:

	1) S -> (S)B
	2) S -> pB
	3) S -> ~NB
	4) S -> CNB
	5) S -> DNB
	6) S -> A(SUS)B
	7) S -> E(SUS)B
	8) S -> HNB
	9) S -> INB
	10) S -> JNB
	11) S -> KNB
	12) B -> e
	13) B -> ^SB
	14) B -> vSB
	15) B -> >SB
	16) N -> (S)
	17) N -> p
	18) N -> ~N
	19) N -> CN
	20) N -> DN
	21) N -> A(SUS)
	22) N -> E(SUS)
	23) N -> HN
	24) N -> IN
	25) N -> JN
	26) N -> KN

	 */

	for(uint i=0;i<256;i++)
		for(uint j=0;j<256;j++)
			parsing_table[i][j] = ERROR;

	parsing_table['B']['$'] = 12;
	parsing_table['B'][')'] = 12;
	parsing_table['B']['>'] = 15;
	parsing_table['B']['U'] = 12;
	parsing_table['B']['^'] = 13;
	parsing_table['B']['v'] = 14;

	parsing_table['N']['('] = 16;
	parsing_table['N']['A'] = 21;
	parsing_table['N']['C'] = 19;
	parsing_table['N']['D'] = 20;
	parsing_table['N']['E'] = 22;
	parsing_table['N'][ATOM] = 17;
	parsing_table['N']['~'] = 18;
	parsing_table['N']['H'] = 23;
	parsing_table['N']['I'] = 24;
	parsing_table['N']['J'] = 25;
	parsing_table['N']['K'] = 26;

	parsing_table['S']['('] = 1;
	parsing_table['S']['A'] = 6;
	parsing_table['S']['C'] = 4;
	parsing_table['S']['D'] = 5;
	parsing_table['S']['E'] = 7;
	parsing_table['S'][ATOM] = 2;
	parsing_table['S']['~'] = 3;
	parsing_table['S']['H'] = 8;
	parsing_table['S']['I'] = 9;
	parsing_table['S']['J'] = 10;
	parsing_table['S']['K'] = 11;

	isAtom = new bool[256];

	for(uint i=0;i<256;i++)
		isAtom[i] = true;

	//while reading input, every character c such that isAtom[c]=true is treated as atom

	isAtom['B'] = false;
	isAtom['S'] = false;
	isAtom['N'] = false;
	isAtom['$'] = false;
	isAtom[')'] = false;
	isAtom['('] = false;
	isAtom['>'] = false;
	isAtom['U'] = false;
	isAtom['^'] = false;
	isAtom['v'] = false;
	isAtom['A'] = false;
	isAtom['E'] = false;
	isAtom['C'] = false;
	isAtom['D'] = false;
	isAtom['H'] = false;
	isAtom['I'] = false;
	isAtom['J'] = false;
	isAtom['K'] = false;

	isAtom['~'] = false;

	isTerminal = new bool[256];

	for(uint i=0;i<256;i++)
		isTerminal[i] = true;

	isTerminal['S'] = false;
	isTerminal['B'] = false;
	isTerminal['N'] = false;

	//parse("(~p)");

}

CTLParser::~CTLParser() {}

//pre-process the string (formula) f:
// 0) remove blank spaces
// 1) -> becomes >
// 2) E~g -> ~Ag (to reduce number of production and simplify grammar)
// 3) A~g -> ~Eg (to reduce number of production and simplify grammar)
// 4) AXg -> Cg ( to make the grammar LL(1) )
// 5) EXg -> Dg ( to make the grammar LL(1) )
// 6) AFg -> Hg ( to make the grammar LL(1) )
// 7) AGg -> Ig ( to make the grammar LL(1) )
// 8) EFg -> Jg ( to make the grammar LL(1) )
// 9) EGg -> Kg ( to make the grammar LL(1) )
// 10) T -> (pv~p)
// 11) appends $
string CTLParser::preProcess(string f){

	string f0 = string();

	//step 0)

	for(unsigned int i=0;i<f.length();i++)
		if(f.at(i)!=' ')
			f0 += f.at(i);

	string f1 = string();

	//steps 1) 2) 3)

	unsigned int fidx=0;

	while (fidx<f0.length()){

		if(fidx+1 < f0.length()){

			if(f0.at(fidx)=='-' and f0.at(fidx+1)=='>'){
				f1 += ">";
				fidx+=2;
			}

			if(f0.at(fidx)=='E' and f0.at(fidx+1)=='~'){
				f1 += "~A";
				fidx+=2;
			}

			else if(f0.at(fidx)=='A' and f0.at(fidx+1)=='~'){
				f1 += "~E";
				fidx+=2;
			}

			else{
				f1 += f0.at(fidx);
				fidx++;
			}

		}else{//fidx points to the last char

			f1 += f0.at(fidx);
			fidx++;

		}

	}

	// steps 4..9

	string f2 = string();

	fidx=0;

	while (fidx<f1.length()){

		if(fidx+1 < f1.length()){

			if(f1.at(fidx)=='A' and f1.at(fidx+1)=='X'){
				f2 += "C";
				fidx+=2;
			}

			else if(f1.at(fidx)=='E' and f1.at(fidx+1)=='X'){
				f2 += "D";
				fidx+=2;
			}

			else if(f1.at(fidx)=='A' and f1.at(fidx+1)=='F'){
				f2 += "H";
				fidx+=2;
			}

			else if(f1.at(fidx)=='A' and f1.at(fidx+1)=='G'){
				f2 += "I";
				fidx+=2;
			}

			else if(f1.at(fidx)=='E' and f1.at(fidx+1)=='F'){
				f2 += "J";
				fidx+=2;
			}

			else if(f1.at(fidx)=='E' and f1.at(fidx+1)=='G'){
				f2 += "K";
				fidx+=2;
			}

			else{
				f2 += f1.at(fidx);
				fidx++;
			}

		}else{//last char

			f2 += f1.at(fidx);
			fidx++;

		}

	}

	//step 10

	string f3 = string();

	fidx=0;

	while (fidx<f2.length()){

		if(f2.at(fidx)=='T'){
			f3 += "(pv~p)";
			fidx++;
		}else{
			f3 += f2.at(fidx);
			fidx++;
		}

	}

	//step 11
	f3 += '$';

	return f3;

}

Formula * CTLParser::parse(string f){

	uint idx = 0;

	cout << endl;
	cout << "Parsing input formula : \"" << f << "\"" <<endl << endl;

	//pre-process the input formula
	string pre_processed = preProcess(f);

	//cout << "Pre-processed formula : \"" << pre_processed << "\"" << endl;
	//cout << "where 'C' = 'AX', 'D' = 'EX', 'H' = 'AF', 'I' = 'AG', 'J'='EF', 'K'='EG', '>' = '->', and '$' is the formula terminator." << endl << endl;

	Formula * result = parseRecursive(preProcess(f),&idx,'S');;

	cout << "The input formula has been parsed as :  \"" << result->toString() << "\"" << endl;

	//remove double negations
	Formula * simplified = result->removeDoubleNegations();

	if(simplified->toString().compare(result->toString())!=0)
		cout << "\nThe input formula has been normalized as :  \"" << simplified->toString() << "\"" << endl<<endl;
	else
		cout << endl;

	return simplified;

}

Formula * CTLParser::parseRecursive(string f, uint * idx, char nt){

	//init stack of nonterminals
	stack<char> stck = stack<char>();

	//insert initial symbol on stack
	stck.push(nt);

	Formula * last_formula = NULL;//last formula constructed while processing input from left to right

	while(not stck.empty()){

		if(*idx>=f.length()){
			cout << "Error while parsing: reached end of formula with non-empty stack."<<endl;
			exit(1);
		}

		//read symbol on top of stack
		char c = stck.top();
		stck.pop();

		//if symbol is terminal, check input
		if(isTerminal[(uchar)c]){

			//case 1: atom
			if(isAtom[(uchar)c] and isAtom[(uchar)f.at(*idx)]){

				last_formula = new Atom(f.at(*idx));

			} else if(not isAtom[(uchar)c] and c==f.at((uchar)*idx)){

				//symbol recognised; continue parsing

			} else {//symbols on stack and input are different: error

				if(isAtom[(uchar)c])
					cout << "Error while parsing: atom expected at \"" << f.substr(*idx,f.length()) << "\"."<<endl;
				else
					cout << "Error while parsing: symbol \"" << c <<  "\" expected at \"" << f.substr(*idx,f.length()) << "\"."<<endl;

				exit(1);

			}

			*idx = *idx + 1;//next symbol on input string

		}else{//symbol on stack is not terminal: check in parsing table

			uchar symbol = f.at(*idx);
			if(isAtom[symbol])
				symbol = ATOM;

			//cout << "checking parsing table PT[" << c << ", " << f.at(*idx)<<"] = " << parsing_table[(uchar)c][symbol] << endl;

			uint rule = parsing_table[(uchar)c][symbol];//rule to be applied

			if(rule == ERROR){
				cout << "Error while parsing. Please, check input formula.\n";
				exit(0);
			}

			switch(rule){

				case 1: /* S -> (S)B */ last_formula = applyRule1(&stck, f, idx); break;
				case 2: /* S -> pB */ stck.push('B');stck.push('x'); break;//x = generic atom
				case 3: /* S -> ~NB */ last_formula=applyRule3(&stck, f, idx);break;
				case 4: /* S -> CNB */ last_formula=applyRule4(&stck, f, idx);break;
				case 5: /* S -> DNB */ last_formula=applyRule5(&stck, f, idx);break;
				case 6: /* S -> A(SUS)B */ last_formula=applyRule6(&stck, f, idx);break;
				case 7: /* S -> E(SUS)B */ last_formula=applyRule7(&stck, f, idx);break;
				case 8: /* S -> HNB */ last_formula=applyRule8(&stck, f, idx);break;
				case 9: /* S -> INB */ last_formula=applyRule9(&stck, f, idx);break;
				case 10: /* S -> JNB */ last_formula=applyRule10(&stck, f, idx);break;
				case 11: /* S -> KNB */ last_formula=applyRule11(&stck, f, idx);break;
				case 12: /* B->' ' : just remove B from top of stack (has been already removed) */ break;
				case 13: /* B -> ^SB */ last_formula=applyRule13(&stck, last_formula, f, idx); break;
				case 14: /* B -> vSB */ last_formula=applyRule14(&stck, last_formula, f, idx); break;
				case 15: /* B -> >SB */ last_formula=applyRule15(&stck, last_formula, f, idx); break;
				case 16: /* N -> (S) */ last_formula = applyRule16(&stck, f, idx); break;
				case 17: /* N -> p*/ stck.push('x'); break;
				case 18: /* N -> ~N */ last_formula=applyRule18(&stck, f, idx);break;
				case 19: /* N -> CN */ last_formula=applyRule19(&stck, f, idx);break;
				case 20: /* N -> DN */ last_formula=applyRule20(&stck, f, idx);break;
				case 21: /* N -> A(SUS) */ last_formula=applyRule21(&stck, f, idx);break;
				case 22: /* N -> E(SUS) */ last_formula=applyRule22(&stck, f, idx);break;
				case 23: /* N -> HN */ last_formula=applyRule23(&stck, f, idx);break;
				case 24: /* N -> IN */ last_formula=applyRule24(&stck, f, idx);break;
				case 25: /* N -> JN */ last_formula=applyRule25(&stck, f, idx);break;
				case 26: /* N -> KN */ last_formula=applyRule26(&stck, f, idx);break;

				default: 	cout << "Error while parsing. Please, check input formula.\n";
							exit(0); break;

			}

		}

	}

	return last_formula;

}

Formula * CTLParser::T(){
	return new Negation(new Conjunction(new Negation(new Atom('p')), new Atom('p') ));
}

Formula * CTLParser::AF(Formula * f){

	//AFf = A(T U f)
	return new AllUntil(T(),f);

}
Formula * CTLParser::AG(Formula * f){

	//AGf = A~F~f = ~EF~f
	return new Negation(EF(new Negation(f)));

}
Formula * CTLParser::EF(Formula * f){

	//EFf = E( T U f )
	return new ExistsUntil(T(),f);

}
Formula * CTLParser::EG(Formula * f){

	//EGf = E~F~f = ~AF~f
	return new Negation(AF(new Negation(f)));

}

Formula * CTLParser::applyRule1(stack <char> * stck, string f, uint * idx){

	//1) S -> (S)B

	*idx = *idx + 1;//skip '(' on input

	Formula * formula = parseRecursive(f,idx,'S');//will start with S on stack and process input until S is consumed

	//check presence of ) on input
	if(f.at(*idx)!=')'){
		cout << "Error while parsing: ')' expected at \"" << f.substr(*idx,f.length()) << "\"."<<endl;
		exit(1);
	}

	*idx = *idx + 1;//skip ')' on input

	stck->push('B');

	return formula;
}
Formula * CTLParser::applyRule3(stack <char> * stck, string f, uint * idx){

	//1) S -> ~NB

	*idx = *idx + 1;//skip '~' on input

	Formula * formula = new Negation(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule4(stack <char> * stck, string f, uint * idx){

	//1) S -> CNB

	*idx = *idx + 1;//skip 'C' on input

	Formula * formula = new AllTomorrow(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule5(stack <char> * stck, string f, uint * idx){

	//1) S -> DNB

	*idx = *idx + 1;//skip 'D' on input

	Formula * formula = new ExistsTomorrow(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule6(stack <char> * stck, string f, uint * idx){

	//1) S -> A(SUS)B

	*idx = *idx + 1;//skip 'A' on input

	if(f.at(*idx)!='('){

		cout << "Error while parsing: '(' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip '(' on input

	Formula * formula1 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!='U'){

		cout << "Error while parsing: 'U' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip 'U' on input

	Formula * formula2 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!=')'){

		cout << "Error while parsing: ')' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip ')' on input

	Formula * result_formula = new AllUntil(formula1,formula2);

	stck->push('B');

	return result_formula;

}
Formula * CTLParser::applyRule7(stack <char> * stck, string f, uint * idx){

	//1) S -> E(SUS)B

	*idx = *idx + 1;//skip 'E' on input

	if(f.at(*idx)!='('){

		cout << "Error while parsing: '(' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip '(' on input

	Formula * formula1 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!='U'){

		cout << "Error while parsing: 'U' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip 'U' on input

	Formula * formula2 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!=')'){

		cout << "Error while parsing: ')' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip ')' on input

	Formula * result_formula = new ExistsUntil(formula1,formula2);

	stck->push('B');

	return result_formula;

}
Formula * CTLParser::applyRule8(stack <char> * stck, string f, uint * idx){

	//S -> HNB = AFNB

	*idx = *idx + 1;//skip 'H' on input

	Formula * formula =  AF( parseRecursive(f,idx,'N')  );//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule9(stack <char> * stck, string f, uint * idx){

	//S -> INB = AGNB

	*idx = *idx + 1;//skip 'C' on input

	Formula * formula = AG( parseRecursive(f,idx,'N') );//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule10(stack <char> * stck, string f, uint * idx){

	//S -> JNB = EFNB

	*idx = *idx + 1;//skip 'J' on input

	Formula * formula =  EF( parseRecursive(f,idx,'N')  );//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule11(stack <char> * stck, string f, uint * idx){

	//S -> KNB = EGNB

	*idx = *idx + 1;//skip 'K' on input

	Formula * formula = EG( parseRecursive(f,idx,'N') );//will start with N on stack and process input until N is consumed

	stck->push('B');

	return formula;

}
Formula * CTLParser::applyRule13(stack <char> * stck, Formula * last_formula, string f, uint * idx){

	//1) B -> ^SB

	*idx = *idx + 1;//skip '^' on input

	Formula * formula = parseRecursive(f,idx,'S');

	Formula * result_formula = new Conjunction(last_formula,formula);

	stck->push('B');

	return result_formula;

}
Formula * CTLParser::applyRule14(stack <char> * stck, Formula * last_formula, string f, uint * idx){

	//1) B -> vSB

	*idx = *idx + 1;//skip 'v' on input

	Formula * formula = parseRecursive(f,idx,'S');

	Formula * result_formula = new Negation(new Conjunction(new Negation(last_formula),new Negation(formula)));

	stck->push('B');

	return result_formula;

}
Formula * CTLParser::applyRule15(stack <char> * stck, Formula * last_formula, string f, uint * idx){

	//1) B -> >SB

	*idx = *idx + 1;//skip '>' on input

	Formula * formula = parseRecursive(f,idx,'S');

	Formula * result_formula = new Negation(new Conjunction(last_formula,new Negation(formula)));

	stck->push('B');

	return result_formula;

}
Formula * CTLParser::applyRule16(stack <char> * stck, string f, uint * idx){

	//1) N -> (S)

	*idx = *idx + 1;//skip '(' on input

	Formula * formula = parseRecursive(f,idx,'S');//will start with S on stack and process input until S is consumed

	//check presence of ) on input
	if(f.at(*idx)!=')'){
		cout << "Error while parsing: ')' expected at \"" << f.substr(*idx,f.length()) << "\"."<<endl;
		exit(1);
	}

	*idx = *idx + 1;//skip ')' on input

	return formula;
}
Formula * CTLParser::applyRule18(stack <char> * stck, string f, uint * idx){

	//1) N -> ~N

	*idx = *idx + 1;//skip '~' on input

	Formula * formula = new Negation(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule19(stack <char> * stck, string f, uint * idx){

	//1) N -> CN

	*idx = *idx + 1;//skip 'C' on input

	Formula * formula = new AllTomorrow(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule20(stack <char> * stck, string f, uint * idx){

	//1) N -> DN

	*idx = *idx + 1;//skip 'D' on input

	Formula * formula = new ExistsTomorrow(parseRecursive(f,idx,'N'));//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule21(stack <char> * stck, string f, uint * idx){

	//1) N -> A(SUS)

	*idx = *idx + 1;//skip 'A' on input

	if(f.at(*idx)!='('){

		cout << "Error while parsing: '(' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip '(' on input

	Formula * formula1 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!='U'){

		cout << "Error while parsing: 'U' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip 'U' on input

	Formula * formula2 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!=')'){

		cout << "Error while parsing: ')' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip ')' on input

	Formula * result_formula = new AllUntil(formula1,formula2);

	return result_formula;

}
Formula * CTLParser::applyRule22(stack <char> * stck, string f, uint * idx){

	//1) N -> E(SUS)

	*idx = *idx + 1;//skip 'E' on input

	if(f.at(*idx)!='('){

		cout << "Error while parsing: '(' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip '(' on input

	Formula * formula1 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!='U'){

		cout << "Error while parsing: 'U' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip 'U' on input

	Formula * formula2 = parseRecursive(f,idx,'S');

	if(f.at(*idx)!=')'){

		cout << "Error while parsing: ')' expected at " << f.substr(*idx,f.length()) << endl;
		exit(0);

	}

	*idx = *idx + 1;//skip ')' on input

	Formula * result_formula = new ExistsUntil(formula1,formula2);

	return result_formula;

}
Formula * CTLParser::applyRule23(stack <char> * stck, string f, uint * idx){

	//N -> HN = AFN

	*idx = *idx + 1;//skip 'H' on input

	Formula * formula =  AF( parseRecursive(f,idx,'N')  );//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule24(stack <char> * stck, string f, uint * idx){

	//N -> IN = AGN

	*idx = *idx + 1;//skip 'C' on input

	Formula * formula = AG( parseRecursive(f,idx,'N') );//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule25(stack <char> * stck, string f, uint * idx){

	//N -> JN = EFN

	*idx = *idx + 1;//skip 'J' on input

	Formula * formula =  EF( parseRecursive(f,idx,'N')  );//will start with N on stack and process input until N is consumed

	return formula;

}
Formula * CTLParser::applyRule26(stack <char> * stck, string f, uint * idx){

	//N -> KN = EGN

	*idx = *idx + 1;//skip 'K' on input

	Formula * formula = EG( parseRecursive(f,idx,'N') );//will start with N on stack and process input until N is consumed

	return formula;

}


}/* namespace ctl_sat */
