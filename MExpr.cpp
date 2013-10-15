#include "MExpr.hpp"
#include "MOpFuncs.hpp"
#include "MTokenTable.hpp"

//Note: For now, we will respond to error conditions by return 0.0
//In future versions, we might want to throw an exception or
//even make a fatal error.

//====================================
//Evaluation of various types of MExpr
//====================================
float BinaryMExpr::evaluate()
{
	if(operand[0] == nullptr || operand[1] == nullptr)
		return 0.0;

	if(opfunction == nullptr)
		return 0.0;

	return opfunction(operand[0]->evaluate(), operand[1]->evaluate());
}

float UnaryMExpr::evaluate()
{
	if(operand == nullptr)
		return 0.0;

	if(opfunction == nullptr)
		return 0.0;

	return opfunction(operand->evaluate());
}

float VarMExpr::evaluate()
{
	if(function == nullptr)
		return 0.0;

	return function();
}

float ConstMExpr::evaluate()
{
	return constant;
}

//============================================
//Removal functions for various types of MExpr
//============================================
void MExpr::remove()
{
	//For expressions that don't allocate any memory,
	//we just delete.
	delete this;
}

void BinaryMExpr::remove()
{
	//Make sure the operands are removed as well.
	if(operand[0])
		operand[0]->remove();

	if(operand[1])
		operand[1]->remove();

	delete this;
}

void UnaryMExpr::remove()
{
	if(operand)
		operand->remove();

	delete this;
}

//=============================================================
//Functions to express the mathematical expression as a string.
//These functions return an empty string in case of failure.
//=============================================================
string BinaryMExpr::express(const MTokenTable& ttable)
{
	//First, express the two operands as strings.
	string lop = operand[0]->express(ttable);
	string rop = operand[1]->express(ttable);

	//If expressing either one results in an error, the
	//whole expression is an error.
	if(lop.empty() || rop.empty())
		return "";

	char opchar = ' ';

	//Find a character that represents the opfunction.
	if(opfunction == BoAdd)			opchar = '+';
	if(opfunction == BoSubtract)	opchar = '-';
	if(opfunction == BoMultiply)	opchar = '*';
	if(opfunction == BoDivide)		opchar = '/';
	if(opfunction == BoExponent)	opchar = '^';

	//If there isn't one, we can't express this expression.
	if(opchar == ' ')
		return "";

	//Find the priority of the two subexpressions in the order of operations.
	int lpriority = operand[0]->oopriority();
	int rpriority = operand[1]->oopriority();

	//If subexpression on the left hand side has a lower priority than our main
	//expression, it must be contained within parenthesis to protect it.
	if(lpriority <= oopriority() && lpriority != -1)
	{
		//We can skip using parenthesis if these operations are associative (like 5+3+2)
		bool associative = (lpriority == oopriority()) && (opfunction == BoAdd || opfunction == BoMultiply);

		if(!associative)
			lop = "(" + lop + ")";
	}

	//Same deal with the right hand side.
	if(rpriority <= oopriority() && rpriority != -1)
	{
		bool associative = (rpriority == oopriority()) && (opfunction == BoAdd || opfunction == BoMultiply);

		if(!associative)
			rop = "(" + rop + ")";
	}

	//Put the string together and return it, adding some spacing between the operator
	//and the subexpressions for readability.
	return lop + ' ' + opchar + ' ' + rop;
}

string UnaryMExpr::express(const MTokenTable& ttable)
{
	string argument = operand->express(ttable);

	if(argument.empty())
		return "";

	//Special case: Check to see if there should be a negative sign.
	if(opfunction == UoNeg)
	{
		//If the priority in the order of operations isn't -1, it's
		//a binary expression and needs to be parenthesized.
		if(operand->oopriority() != -1)
			argument = "(" + argument + ")";

		return "-" + argument;
	}

	string funcname = ttable.to_utoken(opfunction);

	if(funcname.empty())
		return "";

	return funcname + "(" + argument + ")";
}

string VarMExpr::express(const MTokenTable& ttable)
{
	return ttable.to_vtoken(function);
}

string ConstMExpr::express(const MTokenTable& ttable)
{
	string constring = std::to_string(constant);

	//Get rid of any unnecessary trailing zeros.
	if(constring.find('.') != constring.npos)
	{
		while(constring.back() == '0')
			constring.erase(constring.length() - 1, 1);

		//If there's nothing left but a decimal point at the
		//end, get rid of that too.
		if(constring.back() == '.')
			constring.erase(constring.length() - 1, 1);
	}

	return constring;
}

//====================================================================
//Determines the priority given to a specific operation in the order
//of operations. A higher value means higher priority.
//====================================================================
int MExpr::oopriority()
{
	//Note: -1 is a special value that means it's not a real
	//binary expression and therefore doesn't have a place
	//in the order of operations.
	return -1;
}

int BinaryMExpr::oopriority()
{
	if(opfunction == BoSubtract)	return 0;
	if(opfunction == BoAdd)			return 1;
	if(opfunction == BoDivide)		return 2;
	if(opfunction == BoMultiply)	return 3;
	if(opfunction == BoExponent)	return 4;

	return -1;
}

//==================================
//Automatically typecast to a float.
//==================================
MExpr::operator float()
{
	return evaluate();
}
