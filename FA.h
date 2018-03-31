#ifndef MSSC_FA_H
#define MSSC_FA_H

#include "State.h"

class DFA {
public:
	std::set<std::shared_ptr<DFAState>> states;
	std::shared_ptr<DFAState> startState;
	std::set<char> alphabet;

	void checkIfValid();
	void printDot(std::ostream& stream, bool verbose);
};

class eNFA {
public:
	std::set<std::shared_ptr<NFAState>> states;
	std::shared_ptr<NFAState> startState;
	std::set<char> alphabet;
	char eps;

	void checkIfValid();
	void printDot(std::ostream& stream, bool verbose);

	void eclose(std::set<std::shared_ptr<NFAState>>& set, std::shared_ptr<NFAState> state);
	void eclose(std::set<std::shared_ptr<NFAState>>& set, std::set<std::shared_ptr<NFAState>>& states);

	DFA convertToDFA();
};

template<typename Set> std::set<Set> generatePowerset(const Set& s, size_t n);
template<typename Set> std::set<Set> generatePowerset(const Set& s);
#endif