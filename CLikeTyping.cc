#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

vector<string> Terminal = { "BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE",
							"GT", "ID", "IF","INT", "LBRACE", "LE", "LPAREN", "LT",
							"MINUS","NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE",
							"RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN",
							"WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL" };

map<string, pair<vector<string>, map<string, string>>> symbolTable;

vector<string> fcnStack;
string currP;
int errorFlag = 0;

struct parseTree {
	string rule;
	vector<string> tokens;
	vector<parseTree> children;
};

void trav(istream &in, parseTree& tree) {
	string tmp;
	getline(in, tmp);
	istringstream ss(tmp);
	vector<string> tokens;
	string word;
	while (ss >> word) {
		tokens.push_back(word);
	}
	tree.rule = tmp;
	tree.tokens = tokens;
	for (int i = 0; i < Terminal.size(); i++) {
		if (tokens[0] == Terminal[i]) return;
	}
	for (int i = 0; i < tokens.size() - 1; i++) {
		parseTree T;
		tree.children.push_back(T);
	}
	for (int i = 0; i < tokens.size() - 1; i++) {
		trav(in, tree.children[i]);
	}
}

void checkDcl(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		string type, id;
		type = "int*";
		if (tree.children[0].tokens.size() == 2) type = "int";
		id = tree.children[1].tokens[1];
		if (symbolTable[fcn].second.find(id) != symbolTable[fcn].second.end()) {
			cerr << "ERROR: double declaration!" << '\n';
			errorFlag = 1;
		}
		else symbolTable[fcn].second[id] = type;
		return;
	}
	else if (tree.rule == "factor ID" || tree.rule == "lvalue ID") {
		if (symbolTable[fcn].second.find(tree.children[0].tokens[1]) == symbolTable[fcn].second.end()) {
			cerr << "ERROR: use undeclared ID!" << '\n';
			errorFlag = 1;
		}
		return;
	}
	else if (tree.rule == "factor ID LPAREN arglist RPAREN" || tree.rule == "factor ID LPAREN RPAREN") {
		if (symbolTable.find(tree.children[0].tokens[1]) == symbolTable.end() ||
			symbolTable[currP].second.find(tree.children[0].tokens[1]) != symbolTable[currP].second.end()) {
			cerr << "ERROR: use undeclared fcn!" << '\n';
			errorFlag = 1;
		}
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkDcl(tree.children[i], fcn);
	}
}

void checkParams(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		string type;
		type = "int*";
		if (tree.children[0].tokens.size() == 2) type = "int";
		symbolTable[fcn].first.push_back(type);
		return;
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkParams(tree.children[i], fcn);
	}
}

void checkFcn(parseTree& tree) {
	if (tree.rule == "procedures procedure procedures") {
		if (symbolTable.find(tree.children[0].children[1].tokens[1]) != symbolTable.end()) {
			cerr << "ERROR: double declaration!" << '\n';
			errorFlag = 1;
		}
		fcnStack.push_back(tree.children[0].children[1].tokens[1]);
		pair<vector<string>, map<string, string>> tmp;
		symbolTable[tree.children[0].children[1].tokens[1]] = tmp;
		currP = tree.children[0].children[1].tokens[1];
		checkDcl(tree.children[0], tree.children[0].children[1].tokens[1]);
		checkParams(tree.children[0].children[3], tree.children[0].children[1].tokens[1]);
		checkFcn(tree.children[1]);
	}
	else if (tree.rule == "procedures main") {
		currP = "wain";
		fcnStack.push_back("wain");
		checkDcl(tree.children[0], "wain");
		checkParams(tree.children[0].children[3], "wain");
		checkParams(tree.children[0].children[5], "wain");
	}
	else {
		for (int i = 0; i < tree.children.size(); i++) {
			checkFcn(tree.children[i]);
		}
	}
}

void printTable() {
	for (int i = 0; i < fcnStack.size(); i++) {
		cerr << fcnStack[i] << ' ';
		for (int j = 0; j < symbolTable[fcnStack[i]].first.size(); j++) {
			cerr << symbolTable[fcnStack[i]].first[j] << ' ';
		}
		cerr << '\n';
		map<string, string> tmp = symbolTable[fcnStack[i]].second;
		for (auto it = tmp.begin(); it != tmp.end(); it++) {
			cerr << it->first << ' ' << it->second << '\n';
		}
		cerr << '\n';
	}
}

string typeof(parseTree &);

int checkArgs(parseTree &tree, vector<string> params) {
	vector<string> argus;
	while (tree.rule == "arglist expr COMMA arglist") {
		argus.push_back(typeof(tree.children[0]));
		tree = tree.children[2];
	}
	argus.push_back(typeof(tree.children[0]));
	if (argus.size() != params.size()) return 0;
	for (int i = 0; i < params.size(); i++) {
		if (argus[i] != params[i]) return 0;
	}
	return 1;
}

string typeof(parseTree &tree) {
	if (errorFlag) return "ERROR";
	if (tree.rule == "expr term" || tree.rule == "term factor") {
		return typeof(tree.children[0]);
	}
	else if (tree.rule == "expr expr PLUS term") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == "int" && tmp2 == "int") return "int";
		else if (tmp1 == "int*" && tmp2 == "int*") {
			cerr << "ERROR: invalid additon!" << '\n';
			errorFlag = 1;
			return "ERROR";
		}
		else return "int*";
	}
	else if (tree.rule == "expr expr MINUS term") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == tmp2) return "int";
		else if (tmp1 == "int" && tmp2 == "int*") {
			cerr << "ERROR: invalid substraction!" << '\n';
			errorFlag = 1;
			return "ERROR";
		}
		else return "int*";
	}
	else if (tree.rule == "lvalue STAR factor" || tree.rule == "factor STAR factor") {
		if (typeof(tree.children[1]) != "int*") {
			cerr << "ERROR: Invalid Derefernce (not pointer)!" << '\n';
			errorFlag = 1;
			return "ERROR";
		}
		else return "int";
	}
	else if (tree.rule == "lvalue LPAREN lvalue RPAREN" || tree.rule == "factor LPAREN expr RPAREN") {
		return typeof(tree.children[1]);
	}
	else if (tree.rule == "term term STAR factor" || tree.rule == "term term SLASH factor" || tree.rule == "term term PCT factor") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == "int" && tmp2 == "int") return "int";
		else {
			cerr << "ERROR: Invalid Multiplication or Division!" << '\n';
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "factor AMP lvalue") {
		if (typeof(tree.children[1]) == "int") return "int*";
		else {
			cerr << "ERROR: Cannot get pointer of pointer!" << '\n';
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "factor NUM") return "int";
	else if (tree.rule == "factor NULL") return "int*";
	else if (tree.rule == "factor ID" || tree.rule == "lvalue ID") return symbolTable[currP].second[tree.children[0].tokens[1]];
	else if (tree.rule == "factor ID LPAREN RPAREN") return "int";
	else if (tree.rule == "factor ID LPAREN arglist RPAREN") {
		int valid = checkArgs(tree.children[2], symbolTable[tree.children[0].tokens[1]].first);
		if (valid) return "int";
	}
	else if (tree.rule == "factor NEW INT LBRACK expr RBRACK") {
		if (typeof(tree.children[3]) == "int") return "int*";
		else {
			cerr << "ERROR: allocation failed!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "test expr EQ expr" || tree.rule == "test expr NE expr" || tree.rule == "test expr LT expr"
		|| tree.rule == "test expr GT expr" || tree.rule == "test expr LE expr" || tree.rule == "test expr GE expr") {
		string tmp = typeof(tree.children[0]);
		if (tmp == typeof(tree.children[2]) && (tmp == "int" || tmp == "int*")) return "well";
		else {
			cerr << "ERROR: bad comparison!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statements statements statement") {
		string tmp = typeof(tree.children[0]);
		if (tmp == typeof(tree.children[1]) && tmp == "well") return "well";
		else {
			cerr << "ERROR: bad statements!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statements" || tree.rule == "dcls") return "well";
	else if (tree.rule == "statement lvalue BECOMES expr SEMI") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == tmp2 && (tmp1 == "int" || tmp1 == "int*")) return "well";
		else {
			cerr << "ERROR: bad assignment operation!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
		if (typeof(tree.children[2]) == "int") return "well";
		else {
			cerr << "ERROR: bad print operation!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statement DELETE LBRACK RBRACK expr SEMI") {
		if (typeof(tree.children[3]) == "int*") return "well";
		else {
			cerr << "ERROR: bad deallocate operation!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
		if (typeof(tree.children[2]) == "well" && typeof(tree.children[5]) == "well") return "well";
		else {
			cerr << "ERROR: bad while loop!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
		if (typeof(tree.children[2]) == "well" && typeof(tree.children[5]) == "well" && typeof(tree.children[9]) == "well") return "well";
		else {
			cerr << "ERROR: bad if statement!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "dcls dcls dcl BECOMES NUM SEMI") {
		if (typeof(tree.children[0]) == "well" && tree.children[1].children[0].tokens.size() == 2) return "well";
		else {
			cerr << "ERROR: bad dcl of int!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "dcls dcls dcl BECOMES NULL SEMI") {
		if (typeof(tree.children[0]) == "well" && tree.children[1].children[0].tokens.size() == 3) return "well";
		else {
			cerr << "ERROR: bad dcl of pointer!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		if (typeof(tree.children[5]) == "int" && typeof(tree.children[8]) == "well"
			&& typeof(tree.children[9]) == "well" && typeof(tree.children[11]) == "int") return "well";
		else {
			cerr << "ERROR: bad wain!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		if (typeof(tree.children[6]) == "well" && typeof(tree.children[7]) == "well" && typeof(tree.children[9]) == "int") return "well";
		else {
			cerr << "ERROR: bad helper fcn!";
			errorFlag = 1;
			return "ERROR";
		}
	}
	else if (tree.rule == "dcl type ID") {
		if (tree.children[0].tokens.size() == 2) return "int";
		else return "int*";
	}
	else {
		for (int i = 0; i < tree.children.size(); i++) {
			if (tree.children[i].rule == "procedures procedure procedures") currP = tree.children[i].children[0].children[1].tokens[1];
			else if (tree.children[i].rule == "procedures main") currP = "wain";
			typeof(tree.children[i]);
		}
		return "n/a";
	}
}

int main() {
	parseTree Tree;
	trav(cin, Tree);
	checkFcn(Tree);
	typeof(Tree);
	// if(!errorFlag) printTable();
}

