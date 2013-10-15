#ifndef MTOKENTABLE
#define MTOKENTABLE

#include <vector>
#include <string>

#include "MExpr.hpp"

using std::vector;
using std::string;

//An object that can be used to resolve a token string into a variable.
//Usually, you'll make it refer to a variable, such as "X" or "Time"
class VarMToken
{
	public:
		string identifier;
		VarFunc function;

		VarMToken(string id, VarFunc func) : identifier{id}, function{func}
		{
		}
};

//An object that can be used to resolve a token string into a unary function.
//Usually, it will be something like sin() or cos()
class UnaryMToken
{
	public:
		string identifier;
		UnaryFunc function;

		UnaryMToken(string id, UnaryFunc func) : identifier{id}, function{func}
		{
		}
};

class MTokenTable
{
	private:
		//Variable names and function names.
		vector<VarMToken> VarTokens;
		vector<UnaryMToken> UnaryTokens;

	public:
		//List of errors generated during a call to MTokenTable::ParseMExpr()
		//FIXME: Should this be private?
		vector<string> errors;

		//Takes a string and resolve it into the
		//corresponding VarFunc or UnaryFunc.
		VarFunc resolve_vtoken(string id) const;
		UnaryFunc resolve_utoken(string id) const;

		//Does just the opposite of above --- returns the name
		//of the given VarFunc or UnaryFunc.
		string to_vtoken(VarFunc func) const;
		string to_utoken(UnaryFunc func) const;

		//Adds a new variable name or function name to the list.
		void add_vtoken(VarMToken token);
		void add_utoken(UnaryMToken token);

		//Parses the given string and returns a pointer to a newly
		//allocated MExpr. It is the user's job to delete it when they're done.
		MExpr* ParseMExpr(string str);

		//Remove all known tokens from the tokentable.
		void clear_tokens();
};

#endif
