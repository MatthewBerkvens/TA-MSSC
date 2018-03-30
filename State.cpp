#include <assert.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include <set>
#include <memory>
#include <vector>

#include "State.h"

std::string stateSetToString(const std::set<std::shared_ptr<NFAState>>& states)
{
	std::string output;
	if (!states.empty())
	{
		output += (*states.begin())->name;

		for (std::set<std::shared_ptr<NFAState>>::iterator it_state = std::next(states.begin()); it_state != states.end(); it_state++)
		{
			output += ", " + (*it_state)->name;
		}
	}
	return output;
}

std::string stateSetToString(const std::set<std::shared_ptr<DFAState>>& states)
{
	std::string output;
	if (!states.empty())
	{
		output += (*states.begin())->name;

		for (std::set<std::shared_ptr<DFAState>>::iterator it_state = std::next(states.begin()); it_state != states.end(); it_state++)
		{
			output += ", " + (*it_state)->name;
		}
	}
	return output;
}

State::State(const std::string _name, const bool _starting, const bool _accepting)
{
	name = _name;
	starting = _starting;
	accepting = _accepting;
}