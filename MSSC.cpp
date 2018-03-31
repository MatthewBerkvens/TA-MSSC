#include "json\json.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include <set>
#include <memory>
#include <algorithm>

#include "State.h"
#include "FA.h"

int main()
{
	Json::Value root;
	std::ifstream config_doc("C:\\Users\\Matthew\\source\\repos\\MSSC\\Debug\\eNFA.json", std::ifstream::binary);
	config_doc >> root;

	std::string type = root["type"].asString();

	assert(type == "eNFA");

	const Json::Value jsonStates = root["states"];
	const Json::Value jsonTransitions = root["transitions"];
	const Json::Value jsonAlphabet = root["alphabet"];
	const Json::Value jsonEps = root["eps"];

	eNFA enfa = eNFA();

	assert(jsonStates.isArray());
	assert(jsonTransitions.isArray());
	assert(jsonAlphabet.isArray());
	assert(jsonEps.isString());

	enfa.eps = jsonEps.asString()[0];
	for (unsigned int i = 0; i < jsonAlphabet.size(); i++)
	{
		enfa.alphabet.insert(jsonAlphabet[i].asString()[0]);
	}

	for (unsigned int i = 0; i < jsonStates.size(); i++)
	{
		std::shared_ptr<NFAState> newstate(new NFAState(jsonStates[i]["name"].asString(), jsonStates[i]["starting"].asBool(), jsonStates[i]["accepting"].asBool()));
		enfa.states.insert(newstate);
	}

	for (std::set<std::shared_ptr<NFAState>>::iterator it_state = enfa.states.begin(); it_state != enfa.states.end(); it_state++)
	{
		if (it_state->get()->starting) enfa.startState = *it_state;

		for (unsigned int x = 0; x < jsonTransitions.size(); x++)
		{
			if (jsonTransitions[x]["from"].asString() != (*it_state)->name) continue;


			std::pair<char, std::shared_ptr<NFAState>> newtransition =
				std::pair<char, std::shared_ptr<NFAState>>(
					jsonTransitions[x]["input"].asString()[0],
					*std::find_if(
						enfa.states.begin(),
						enfa.states.end(),
						[&](const std::shared_ptr<NFAState> p) { return jsonTransitions[x]["to"].asString() == p->name; }
			));

			(*it_state)->transitions.insert(newtransition);
		}
	}

	enfa.checkIfValid();
	enfa.printDot(std::cout, false);
	std::cout << "\n";

	DFA d = enfa.convertToDFA();

	d.checkIfValid();
	d.printDot(std::cout, false);

	std::cin.get();

	return 0;
}