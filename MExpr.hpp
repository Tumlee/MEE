#ifndef MEXPR
#define MEXPR

#include <string>

using std::string;

//===========================================================================
//Note: When transforming a non-constant token into an MExpr, it will likely
//be of the class VarMExpr which will have its own function pointer that
//will return a float and take no arguments.

//A registry of different function pointers will have to be used to keep
//track of this. For example, if there is a token in the expression called
//"TIME", the parser will have to look that up in a table, which would probably
//match it with a GetTime() function of some sort.
//===========================================================================

//Typedefs for various types of functions, to avoid having to deal with nasty syntax later.
typedef float (*VarFunc)();
typedef float (*UnaryFunc)(float);
typedef float (*BinaryFunc)(float, float);

class MTokenTable;

//Base class for a math expression.
//The number type being used for this class will be float.
class MExpr
{
	public:

		//Calling this function evalutates the math expression.
		virtual float evaluate() = 0;
		virtual string express(const MTokenTable& ttable) = 0;
		virtual int oopriority();

		//This might be handled better by a destructor, but I don't know how to guarantee they're being called.
		virtual void remove();

		operator float();
};

//Class that represents a binary expression, or expression that manipulates two values.
class BinaryMExpr : public MExpr
{
	public:
		virtual float evaluate();
		virtual void remove();
		virtual string express(const MTokenTable& ttable);
		virtual int oopriority();

		MExpr* operand[2];
		BinaryFunc opfunction;

		BinaryMExpr(MExpr* op1 = nullptr, MExpr* op2 = nullptr, BinaryFunc ofun = nullptr)
		{
			operand[0] = op1;
			operand[1] = op2;
			opfunction = ofun;
		}
};

//Class that represents a unary expression, or expression that manipulates one value.
class UnaryMExpr : public MExpr
{
	public:
		virtual float evaluate();
		virtual void remove();
		virtual string express(const MTokenTable& ttable);

		MExpr* operand;
		UnaryFunc opfunction;

		UnaryMExpr(MExpr* op1 = nullptr, UnaryFunc ofun = nullptr)
		{
			operand = op1;
			opfunction = ofun;
		}
};

//Class that represents a variable expression, which will do what the function tells it to do.
class VarMExpr : public MExpr
{
	public:
		virtual float evaluate();
		virtual string express(const MTokenTable& ttable);

		VarFunc function;

		VarMExpr(VarFunc fun = nullptr)
		{
			function = fun;
		}
};

//Class that represents a numeric constant. The evalutate() member function will just return this constant.
class ConstMExpr : public MExpr
{
	public:
		virtual float evaluate();
		virtual string express(const MTokenTable& ttable);

		float constant;

		ConstMExpr(float num = 0.0)
		{
			constant = num;
		}
};

#endif
