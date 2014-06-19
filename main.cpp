/*
 * main.cpp
 *
 *  Created on: May 20, 2014
 *      Author: nicola
 */

#include "parser/CTLParser.h"
#include "tableau/Tableau.h"

using namespace ctl_sat;

int main(int argc,char** argv){

	if(argc != 2){
		cout << "*** CTL-sat solver ***\n";
		cout << "Usage: ctl-sat formula\n";
		cout << "For more informations, read the file README.\n";
		exit(0);
	}

	CTLParser * parser = new CTLParser();

	Formula * f = parser->parse(string(argv[1]));

	Tableau * tableau = new Tableau(f);

	if(tableau->isSatisfiable())
		cout << "Input formula is satisfable!\n";
	else
		cout << "Input formula is NOT satisfable!\n";

}

