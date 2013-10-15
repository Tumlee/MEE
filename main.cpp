#include <iostream>
#include <string>

#include "MTokenTable.hpp"
#include "MOpFuncs.hpp"

using std::endl;
using std::cout;
using std::cin;
using std::string;
using std::getline;

//Information for storing the 'ans' variable.
float last_answer = 0.0;

float getans()
{
	return last_answer;
}

int main()
{
	//Create a tokentable that knows the meaning
	//of 'ans' along with various trigonometic functions.
	MTokenTable tokentable;

	tokentable.add_vtoken({"ans", getans});

	tokentable.add_utoken({"sin", UoSin});
	tokentable.add_utoken({"cos", UoCos});
	tokentable.add_utoken({"tan", UoTan});
	tokentable.add_utoken({"asin", UoAsin});
	tokentable.add_utoken({"acos", UoAcos});
	tokentable.add_utoken({"atan", UoAtan});

	while(true)
	{
		//Grab a line of input and parse it via the tokentable.
		string input;
		getline(cin, input);
		MExpr* expression = tokentable.ParseMExpr(input);

		if(expression)
		{
			//If parsing the expression was a success, re-express
			//the expression as a string and print its value.
			cout << expression->express(tokentable) << " = ";
			cout << expression->evaluate() << endl << endl;

			//Set 'ans'
			last_answer = expression->evaluate();

			//Delete the expression to prevent any memory leaks.
			expression->remove();
		}
		else
		{
			//Otherwise, explain the syntax error to the user.
			for(string error : tokentable.errors)
				cout << error << endl;

			cout << endl;
		}
	}

	return 0;
}
