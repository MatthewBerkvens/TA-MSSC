#include "json\json.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <set>
#include <memory>
#include <algorithm>
#include <exception>

#include "State.h"
#include "FA.h"

int main(int argc, char* argv[])
{
	bool verbose = false;
	std::string filename;
	std::string outputname;

	if (argc <= 2)
	{
		std::cout << "Usage: " << argv[0] << " (-v) <json input file> <dot output file>";
		return 0;
	}
	else
	{
		if (strcmp(argv[1], "-v") == 0 && argc == 4)
		{
			verbose = true;
			filename = argv[2];
			outputname = argv[3];
		}
		else
		{
			filename = argv[1];
			outputname = argv[2];
		}
	}

	Json::Value root;

	try {
		std::ifstream config_doc(filename, std::ifstream::binary);
		config_doc >> root;
	}
	catch (std::exception& e)
	{
		(void)e; //remove unreferences local variable warning
	}

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

	DFA d = enfa.convertToDFA();

	d.checkIfValid();
	std::ofstream outputfile;
	outputfile.open(outputname);
	d.printDot(outputfile, verbose);

	return 0;
}