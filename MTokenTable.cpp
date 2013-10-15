#include "MTokenTable.hpp"

//Returns a function that is matched up with the given identifier string.
//If such a function cannot be found, return a null pointer.
VarFunc MTokenTable::resolve_vtoken(string id) const
{
	for(auto vtoken : VarTokens)
	{
		if(vtoken.identifier == id)
			return vtoken.function;
	}

	return nullptr;
}

UnaryFunc MTokenTable::resolve_utoken(string id) const
{
	for(auto utoken : UnaryTokens)
	{
		if(utoken.identifier == id)
			return utoken.function;
	}

	return nullptr;
}

//These functions do the opposite of what is done above. You provide
//them with a function, and this will return the name of the function.
//If such a function isn't registered as as a token, it will return
//an empty string.
string MTokenTable::to_vtoken(VarFunc func) const
{
	for(auto vtoken : VarTokens)
	{
		if(vtoken.function == func)
			return vtoken.identifier;
	}

	return "";
}

string MTokenTable::to_utoken(UnaryFunc func) const
{
	for(auto utoken : UnaryTokens)
	{
		if(utoken.function == func)
			return utoken.identifier;
	}

	return "";
}

void MTokenTable::add_vtoken(VarMToken token)
{
	//Don't add duplicate identifiers.
	if(resolve_vtoken(token.identifier))
		return;

	VarTokens.push_back(token);

	return;
}

void MTokenTable::add_utoken(UnaryMToken token)
{
	//Don't add duplicate identifiers.
	if(resolve_utoken(token.identifier))
		return;

	UnaryTokens.push_back(token);

	return;
}
