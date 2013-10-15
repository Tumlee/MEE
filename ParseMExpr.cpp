#include "MTokenTable.hpp"
#include "MOpFuncs.hpp"

//Function that returns true if the entire string can be interpereted
//as a numeric constant.
bool is_numeric_constant(string token)
{
	//Whether or not we've found a decimal point.
	bool dpoint = false;

	//An empty string is not a numeric constant.
	if(token.empty())
		return false;

	for(char c : token)
	{
		if(c == '.')
		{
			//Set the flag to inform us that we've reached
			//a decimal point.
			if(!dpoint)
				dpoint = true;

			//If we've already found one, and then we've found another,
			//this is clearly not a numeric constant.
			else
				return false;
		}
		else if (c < '0' || c > '9')	//Not a number.
		{
			return false;
		}
	}

	if(token[token.length() - 1] == '.')
		return false;	//ends on a period.

	return true;
}

size_t match_parenthesis(string buf, size_t lp)
{
	int parenthesis_level = 0;

	if(buf[lp] != '(')
		return -1;	//Never had a parenthesis to begin with.

	for(size_t i = lp; i < buf.length(); i++)
	{
		if(buf[i] == '(')
			parenthesis_level++;

		if(buf[i] == ')')
		{
			if(--parenthesis_level == 0)
				return i;
		}
	}

	return -1;
}

//Returns true if the string can be interpereted as a unary function,
//in a pattern such as "function(argument)"
bool is_unaryfunc(string str)
{
	//Find the location of the first parenthesis.
	size_t left_loc = str.find('(');

	if(left_loc == str.npos)
		return false;

	return match_parenthesis(str, left_loc) == str.length() - 1;
}

//Returns true if the character represents a binary operator.
bool is_binop(char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

//Returns the current binary operator's priority, based on the order
//of operations. (Higher priority goes first).
int binop_priority(char c)
{
	switch(c)
	{
		case '-':	return 0;
		case '+':	return 1;
		case '/':	return 2;
		case '*':	return 3;
		case '^':	return 4;
		default:	return -1;
	}
}

//Converts a character into the corresponding binary function.
BinaryFunc resolve_binop(char c)
{
	switch(c)
	{
		case '-':	return BoSubtract;
		case '+':	return BoAdd;
		case '/':	return BoDivide;
		case '*':	return BoMultiply;
		case '^':	return BoExponent;
		default:	return nullptr;
	}
}

MExpr* MTokenTable::ParseMExpr(string str)
{
	errors.clear();

	int parenthesis_level = 0;
	vector<size_t> ops;
	size_t spos;

	MExpr* lexp = nullptr;
	MExpr* rexp = nullptr;

	//Remove all whitespace from the expression.
	while((spos = str.find(' ')) != str.npos)
		str.erase(spos, 1);

	//If there are parenthesis around the entire expression,
	//then they should be removed.
	while(match_parenthesis(str, 0) == str.length() - 1)
	{
		if(str.length() == 0)
		{
			errors.push_back("SYNTAX ERROR: Empty.");
			return nullptr;
		}

		str.erase(0, 1);
		str.erase(str.length() - 1, 1);
	}

	//Go through the string and find any binary operators.
	for(size_t i = 0; i < str.length(); i++)
	{
		if(str[i] == '(')	parenthesis_level++;
		if(str[i] == ')')	parenthesis_level--;

		//Something's wrong if the parenthesis level drops below zero.
		if(parenthesis_level < 0)
		{
			errors.push_back("SYNTAX ERROR: Parenthesis.");
			return nullptr;
		}

		//Only read binops if parenthesis level is zero.
		if(parenthesis_level == 0)
		{
			if(is_binop(str[i]))
			{
				if((i > 0 && is_binop(str[i - 1])) || i == 0)
				{
					//We've either encountered two binops in a row, or
					//we've started on a binop. This is only acceptable if we have
					//a negative sign here.
					if(str[i] == '-')
						continue;

					errors.push_back("SYNTAX ERROR: Misplaced operator.");
					return nullptr;
				}

				ops.push_back(i);
			}
		}
	}

	//Parenthesis level should be zero by the end.
	//If not, we have a malformed expression.
	if(parenthesis_level != 0)
	{
		errors.push_back("SYNTAX ERROR: Parenthesis.");
		return nullptr;
	}

	if(ops.size())
	{
		size_t lowop = 0;	//The operator with the lowest priority.

		for(size_t o = 0; o < ops.size(); o++)
		{
			if(o == 0 || binop_priority(str[ops[o]]) <= binop_priority(str[ops[lowop]]))
				lowop = o;
		}

		//Position of the lowest priority operator.
		int opos = ops[lowop];

		//Split the expression in to two subexpressions, using the lowest
		//priority operator as the splitting point.
		lexp = ParseMExpr(str.substr(0, opos));
		rexp = ParseMExpr(str.substr(opos + 1, str.length() - opos - 1));

		//If parsing either of these subexpressions results in an error of
		//any kind, the whole expression is invalid and we return null.
		if(!lexp || !rexp)
		{
			if(lexp)	lexp->remove();
			if(rexp)	rexp->remove();

			return nullptr;
		}

		//Otherwise, we return our brand new binary expression.
		return new BinaryMExpr(lexp, rexp, resolve_binop(str[opos]));
	}

	//If there were no binary operators, check to see if we can
	//at least resolve a negative sign.
	if(str[0] == '-')
	{
		//Get rid of that negative sign and parse the
		//rest of the string as its own expression.
		str.erase(0, 1);
		return new UnaryMExpr(ParseMExpr(str), UoNeg);
	}

	//If not, check if it's a numeric constant.
	if(is_numeric_constant(str))
		return new ConstMExpr(stof(str));

	if(is_unaryfunc(str))
	{
		//Find the location of the first parenthesis so we can split the string up.
		size_t left_loc = str.find('(');

		//The portion to the left of the parenthsis is the function name.
		string funcname = str.substr(0, left_loc);

		//What's inside the parenthesis is the argument.
		string argument = str.substr(left_loc + 1, str.length() - left_loc - 2);

		UnaryFunc func = resolve_utoken(funcname);

		if(func == nullptr)
		{
			errors.push_back("ERROR: Unresolved function [ " + funcname + " ]");
			return nullptr;
		}

		return new UnaryMExpr(ParseMExpr(argument), func);
	}

	//Only handles vartokens for now. Other expression types will come later.
	if(resolve_vtoken(str))
		return new VarMExpr(resolve_vtoken(str));

	errors.push_back("ERROR: Unresovled token [ " + str + " ]");
	return nullptr;
}
