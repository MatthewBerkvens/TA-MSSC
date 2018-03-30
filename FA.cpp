#include <assert.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <set>
#include <memory>
#include <vector>
#include <algorithm>

#include "FA.h"
#include "State.h"

void DFA::checkIfValid()
{
	assert(alphabet.size() != 0);

	unsigned int startingStatesCount = 0;
	for (std::set<std::shared_ptr<DFAState>>::iterator it_state = states.begin(); it_state != states.end(); it_state++) //loop over states
	{
		std::set<char> transChars;
		for (std::vector<std::pair<char, std::shared_ptr<DFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++) //loop over transitions of current state
		{
			assert(std::find(alphabet.begin(), alphabet.end(), it_trans->first) != alphabet.end()); //check if alphabet contains input
			assert(std::find_if(states.begin(), states.end(), [&](const std::shared_ptr<DFAState> p) { return p->name == it_trans->second->name; }) != states.end()); //check if states contains transitionstate

			assert(std::find(transChars.begin(), transChars.end(), it_trans->first) == transChars.end()); //make sure there are no duplicate transitions
			transChars.insert(it_trans->first);
		}

		assert((*it_state)->transitions.size() == alphabet.size()); //make sure every transition is handled by each state

		if ((*it_state)->starting)
			startingStatesCount++;
	}
	assert(startingStatesCount == 1); //only 1 starting state allowed
}

void DFA::printDot(std::ostream& stream)
{
	std::string output = "digraph {\n\"start\" -> \"" + startState->name + "\"\n\n";

	for (std::set<std::shared_ptr<DFAState>>::iterator it_state = states.begin(); it_state != states.end(); it_state++)
	{
		output += "\"" + ((*it_state)->name == "" ? "garbage" : (*it_state)->name) + "\"" + ((*it_state)->accepting ? " [peripheries=2]" : "") + "\n";

		for (std::vector<std::pair<char, std::shared_ptr<DFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++)
		{
			output += "\"" + ((*it_state)->name == "" ? "garbage" : (*it_state)->name) + "\" -> \"" + (it_trans->second->name == "" ? "garbage" : it_trans->second->name) + "\" [label=\"" + it_trans->first + "\"]\n";
		}

		output += "\n";
	}
	stream << output.substr(0, output.size() - 1) << "}\n\n";
}




void eNFA::checkIfValid()
{
	unsigned int startingStatesCount = 0;
	for (std::set<std::shared_ptr<NFAState>>::iterator it_state = states.begin(); it_state != states.end(); it_state++)
	{
		for (std::vector<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++)
		{
			if (it_trans->first != eps)
				assert(std::find(alphabet.begin(), alphabet.end(), it_trans->first) != alphabet.end()); //check if alphabet contains input if not an eps transition
			assert(std::find_if(states.begin(), states.end(), [&](const std::shared_ptr<NFAState> p) { return p->name == it_trans->second->name; }) != states.end()); //check if states contains transitionstate
		}

		if ((*it_state)->starting)
			startingStatesCount++;
	}
	assert(startingStatesCount == 1); //only 1 starting state allowed
}

void eNFA::printDot(std::ostream& stream)
{
	std::string output = "digraph {\n\"start\" -> \"" + startState->name + "\"\n\n";

	for (std::set<std::shared_ptr<NFAState>>::iterator it_state = states.begin(); it_state != states.end(); it_state++)
	{
		output += "\"" + ((*it_state)->name == "" ? "garbage" : (*it_state)->name) + "\"" + ((*it_state)->accepting ? " [peripheries=2]" : "") + "\n";

		for (std::vector<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++)
		{
			output += "\"" + ((*it_state)->name == "" ? "garbage" : (*it_state)->name) + "\" -> \"" + (it_trans->second->name == "" ? "garbage" : it_trans->second->name) + "\" [label=\"" + it_trans->first + "\"]\n";
		}

		output += "\n";
	}
	stream << output.substr(0, output.size() - 1) << "}\n\n";
}

void eNFA::eclose(std::set<std::shared_ptr<NFAState>>& set, std::shared_ptr<NFAState> state)
{
	set.insert(state);
	for (std::vector<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = state->transitions.begin(); it_trans != state->transitions.end(); it_trans++)
	{
		if (it_trans->first == eps)
		{
			if (std::find(set.begin(), set.end(), it_trans->second) == set.end()) eclose(set, it_trans->second);
		}
	}
}

void eNFA::eclose(std::set<std::shared_ptr<NFAState>>& set, std::set<std::shared_ptr<NFAState>>& states)
{
	for (std::set<std::shared_ptr<NFAState>>::iterator it_state = states.begin(); it_state != states.end(); it_state++)
	{
		set.insert(*it_state);
		for (std::vector<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++)
		{
			if (it_trans->first == eps)
			{
				if (std::find(set.begin(), set.end(), it_trans->second) == set.end()) eclose(set, it_trans->second);
			}
		}
	}
}

DFA eNFA::convertToDFA()
{
	checkIfValid();
	DFA dfa;
	dfa.alphabet = alphabet;

	std::set<std::shared_ptr<NFAState>> startSet;

	std::set<std::set<std::shared_ptr<NFAState>>> powerset;
	eclose(startSet, startState);

	std::set<std::set<std::shared_ptr<NFAState>>> eclosedpowerset;

	stateSetToString(startSet);

	powerset.insert(startSet);
	powerset = generatePowerset<std::set<std::shared_ptr<NFAState>>>(states);

	for (std::set<std::set<std::shared_ptr<NFAState>>>::iterator it_set = powerset.begin(); it_set != powerset.end(); it_set++)
	{
		std::set<std::shared_ptr<NFAState>> newset;
		std::set<std::shared_ptr<NFAState>> wtf = *it_set;
		eclose(newset, wtf);
		eclosedpowerset.insert(newset);
	}

	for (std::set<std::set<std::shared_ptr<NFAState>>> ::iterator it_set = eclosedpowerset.begin(); it_set != eclosedpowerset.end(); it_set++)
	{
		bool accepting = false;
		std::string statename = stateSetToString(*it_set);
		bool starting = (statename == stateSetToString(startSet));

		for (std::set<std::shared_ptr<NFAState>>::iterator it_state = it_set->begin(); it_state != it_set->end(); it_state++)
		{
			if (std::find_if(states.begin(), states.end(), [&](const std::shared_ptr<NFAState>& p) { return (p->name == (*it_state)->name) && p->accepting; }) != states.end())
				accepting = true;
		}

		std::shared_ptr<DFAState> dfastate(new DFAState(statename, starting, accepting));

		dfa.states.insert(dfastate);
		if (starting)
			dfa.startState = dfastate;
	}

	for (std::set<std::set<std::shared_ptr<NFAState>>> ::iterator it_set = eclosedpowerset.begin(); it_set != eclosedpowerset.end(); it_set++)
	{
		std::string statesname = stateSetToString(*it_set);
		std::shared_ptr<DFAState> correspondingdfa = *std::find_if(dfa.states.begin(), dfa.states.end(), [&](const std::shared_ptr<DFAState>& p) { return p->name == statesname; });

		for (std::vector<char>::iterator it_alp = alphabet.begin(); it_alp != alphabet.end(); it_alp++)
		{
			std::set<std::shared_ptr<NFAState>> transitionset;

			for (std::set<std::shared_ptr<NFAState>>::iterator it_state = it_set->begin(); it_state != it_set->end(); it_state++)
			{
				std::set<std::shared_ptr<NFAState>> state_transitionset;
				for (std::vector<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = (*it_state)->transitions.begin(); it_trans != (*it_state)->transitions.end(); it_trans++)
				{
					if (it_trans->first == (*it_alp))
					{
						std::set<std::shared_ptr<NFAState>> state_transitionset_eclosed;

						eclose(state_transitionset_eclosed, it_trans->second);

						state_transitionset.insert(state_transitionset_eclosed.begin(), state_transitionset_eclosed.end());
					}
				}

				transitionset.insert(state_transitionset.begin(), state_transitionset.end());
			}

			std::string transitionsetname = stateSetToString(transitionset);

			correspondingdfa->transitions.push_back(std::pair<char, std::shared_ptr<DFAState>>(*it_alp, *std::find_if(dfa.states.begin(), dfa.states.end(), [&](const std::shared_ptr<DFAState>& p) { return p->name == transitionsetname; })));
		}
	}

	dfa.checkIfValid();
	return dfa;
}

//credit goes to https://rosettacode.org/wiki/Power_set#Recursive_version
template<typename Set> std::set<Set> generatePowerset(const Set& s, size_t n)
{
	typedef typename Set::const_iterator SetCIt;
	typedef typename std::set<Set>::const_iterator PowerSetCIt;
	std::set<Set> res;
	if (n > 0) {
		std::set<Set> ps = generatePowerset(s, n - 1);
		for (PowerSetCIt ss = ps.begin(); ss != ps.end(); ss++)
			for (SetCIt el = s.begin(); el != s.end(); el++) {
				Set subset(*ss);
				subset.insert(*el);
				res.insert(subset);
			}
		res.insert(ps.begin(), ps.end());
	}
	else
		res.insert(Set());
	return res;
}

template<typename Set> std::set<Set> generatePowerset(const Set& s)
{
	return generatePowerset(s, s.size());
}
