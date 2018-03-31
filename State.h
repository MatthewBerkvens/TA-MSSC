#ifndef MSSC_STATE_H
#define MSSC_STATE_H

#include <algorithm>

class State {
public:
	std::string name;
	bool starting;
	bool accepting;

	State(const std::string _name, const bool _starting, const bool _accepting);

	bool operator< (const State &other) const
	{
		return name < other.name;
	};

	bool operator==(const std::string r)
	{
		return name == r;
	};
};

class NFAState : public State {
public:
	std::set<std::pair<char, std::shared_ptr<NFAState>>> transitions;

	using State::State;

	std::set<std::shared_ptr<NFAState>> operator()(char c)
	{
		std::set<std::shared_ptr<NFAState>> result;

		for (std::set<std::pair<char, std::shared_ptr<NFAState>>>::iterator it_trans = transitions.begin(); it_trans != transitions.end(); it_trans++)
		{
			if (it_trans->first == c)
				result.insert(it_trans->second);
		}

		return result;
	};
};

class DFAState : public State {
public:
	std::set<std::pair<char, std::shared_ptr<DFAState>>> transitions;

	using State::State;

	std::shared_ptr<DFAState> operator()(char c)
	{
		return std::find_if(transitions.begin(), transitions.end(), [&](const std::pair<char, std::shared_ptr<DFAState>>& p) { return p.first == c; })->second;
	}
};

std::string stateSetToString(const std::set<std::shared_ptr<NFAState>>& states);
std::string stateSetToString(const std::set<std::shared_ptr<DFAState>>& states);

#endif