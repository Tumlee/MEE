#include <cmath>

//Unary operators.
float UoNeg(float input)
{
	return -input;
}

float UoSin(float input)
{
	return sin(input);
}

float UoCos(float input)
{
	return cos(input);
}

float UoTan(float input)
{
	return tan(input);
}

float UoAsin(float input)
{
	return asin(input);
}

float UoAcos(float input)
{
	return acos(input);
}

float UoAtan(float input)
{
	return atan(input);
}

//Binary operators.
float BoMultiply(float op1, float op2)
{
	return op1 * op2;
}

float BoAdd(float op1, float op2)
{
	return op1 + op2;
}

float BoSubtract(float op1, float op2)
{
	return op1 - op2;
}

float BoExponent(float op1, float op2)
{
	return pow(op1, op2);
}

float BoDivide(float op1, float op2)
{
	return op1 / op2;
}
