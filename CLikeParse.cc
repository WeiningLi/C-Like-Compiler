#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>
using namespace std;

struct parseTree {
	int prodR;
	vector<parseTree> child;
};


void constructParser(istream &in, vector<string> &T,
	vector<string> &NT, string &S) {
	int i, numTerm, numNonTerm;
	string s;
	// read terminals
	in >> numTerm;
	getline(in, s);
	for (i = 0; i < numTerm; i++) {
		getline(in, s);
		T.push_back(s);
	}

	// read non-terminals
	in >> numNonTerm;
	getline(in, s);
	for (i = 0; i < numNonTerm; i++) {
		getline(in, s);
		NT.push_back(s);
	}

	// read start symbol
	getline(in, S);
}

void traverse(parseTree &tree, vector<string> &Prods, vector<string> &toLexeme) {
	if (tree.prodR >= 0) cout << Prods[tree.prodR] << '\n';
	else cout << toLexeme[tree.prodR * (-1) -1] << '\n';
	for (int i = 0; i < tree.child.size(); i++) {
		traverse(tree.child[i], Prods, toLexeme);
	}
}

void printResult(vector<parseTree> &tree_stack, vector<string> &Prods, vector<string> &toLexeme) {
	for (int i = 0; i < tree_stack.size(); i++) {
		traverse(tree_stack[i], Prods, toLexeme);
	}
}

int main() {
	vector<string> T, NT, symbol_stack, Prods, unread;
	vector<int> state_stack;
	string S, tmp, firstProd;
	vector<map<string, pair<int, int>>> table;

	ifstream inFile;
	inFile.open("in");
	constructParser(inFile, T, NT, S);

	vector<vector<string>> P;
	int numRules;
	// read prod rules
	inFile >> numRules;
	getline(inFile, tmp);
	for (int i = 0; i < numRules; i++) {
		getline(inFile, tmp);
		Prods.push_back(tmp);
		istringstream ss(tmp);
		vector<string> line;
		string word;
		while (ss >> word) {
			line.push_back(word);
		}
		if (line.size() > 1 && line[1] == "BOF") firstProd = Prods.back();
		P.push_back(line);
	}

	// build parser table
	int numStates, numTrans;
	inFile >> numStates;
	getline(inFile, tmp);
	inFile >> numTrans;
	getline(inFile, tmp);
	for (int i = 0; i < numStates; i++) {
		map<string, pair<int, int> > aMap;
		table.push_back(aMap);
	}

	for (int j = 0; j < numTrans; j++) {
		int start, act = 0, next;
		string str, tmp;
		inFile >> start >> str >> tmp >> next;
		if (str == "BOF") state_stack.push_back(start); // init state_stack
		if (tmp == "reduce") act = 1;
		pair<int, int> out;
		out.first = act;
		out.second = next;
		table[start][str] = out;
	}

	unread.push_back("BOF");
	unread.push_back("BOF");
	while (cin >> tmp) {
		unread.push_back(tmp); // type
		cin >> tmp;
		unread.push_back(tmp); // lexeme
	}
	unread.push_back("EOF");
	unread.push_back("EOF");

	// vector<int> track_reduce;
	vector<parseTree> treeStack;
	vector<string> to_lexeme;
	int readIndex = 0;
	int errorIndex = 0;

	while (readIndex < unread.size()) {
		if (table[state_stack.back()].find(unread[readIndex]) != table[state_stack.back()].end()) {
			pair<int, int> next = table[state_stack.back()][unread[readIndex]];
			if (next.first == 0) { // shift
				symbol_stack.push_back(unread[readIndex]);
				string lexeme1 = unread[readIndex] + " " + unread[readIndex + 1];
				to_lexeme.push_back(lexeme1);
				treeStack.push_back(parseTree{ -1 * to_lexeme.size(), vector<parseTree> ()});
				readIndex += 2;
				errorIndex++;
				state_stack.push_back(next.second);
				continue;
			}
			else { // reduce
				int prodRule = next.second;
				parseTree newNode = { next.second, vector<parseTree>() };
				vector<parseTree> tmp;
				for (int i = 0; i < P[prodRule].size() - 1; i++) {
					state_stack.pop_back();
					symbol_stack.pop_back();
					tmp.push_back(treeStack.back());
					treeStack.pop_back();
				}
				for (int i = 0; i < P[prodRule].size() - 1; i++) {
					newNode.child.push_back(tmp.back());
					tmp.pop_back();
				}
				treeStack.push_back(newNode);
				if (table[state_stack.back()].find(P[prodRule][0]) != table[state_stack.back()].end()) {
					symbol_stack.push_back(P[prodRule][0]);
					state_stack.push_back(table[state_stack.back()][P[prodRule][0]].second);
				}
				else {
					cerr << "ERROR at " << errorIndex << '\n';
					return 0;
				}
			}
		}
		else {
			cerr << "ERROR at " << errorIndex << '\n';
			return 0;
		}
	}

	if (symbol_stack.back() == "EOF") {
		cout << firstProd << '\n';
		printResult(treeStack, Prods, to_lexeme);
	}
	else {
		cerr << "ERROR at " << errorIndex << '\n';
		return 0;
	}
}
