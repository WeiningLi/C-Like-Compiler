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

map<string, pair<vector<string>, map<string, string>>> symbolTable1; // used for type checking

struct parseTree {
	string rule;
	vector<string> tokens;
	vector<parseTree> children;
};

string currP = "wain";

void checkDcl1(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		string type, id;
		type = "int*";
		if (tree.children[0].tokens.size() == 2) type = "int";
		id = tree.children[1].tokens[1];
		symbolTable1[fcn].second[id] = type;
		return;
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkDcl1(tree.children[i], fcn);
	}
}

void checkParams1(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		string type;
		type = "int*";
		if (tree.children[0].tokens.size() == 2) type = "int";
		symbolTable1[fcn].first.push_back(type);
		return;
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkParams1(tree.children[i], fcn);
	}
}

void checkFcn1(parseTree& tree) {
	if (tree.rule == "procedures procedure procedures") {
		pair<vector<string>, map<string, string>> tmp;
		symbolTable1[tree.children[0].children[1].tokens[1]] = tmp;
		currP = tree.children[0].children[1].tokens[1];
		checkDcl1(tree.children[0], tree.children[0].children[1].tokens[1]);
		checkParams1(tree.children[0].children[3], tree.children[0].children[1].tokens[1]);
		checkFcn1(tree.children[1]);
	}
	else if (tree.rule == "procedures main") {
		currP = "wain";
		checkDcl1(tree.children[0], "wain");
		checkParams1(tree.children[0].children[3], "wain");
		checkParams1(tree.children[0].children[5], "wain");
	}
	else {
		for (int i = 0; i < tree.children.size(); i++) {
			checkFcn1(tree.children[i]);
		}
	}
}

map<string, map<string, pair<string, int>>> symbolTable;
//  fcn name    para N       type    pos

map<string, int> fcnStack;
int pos;
int controlNum = 0;

string typeof(parseTree &tree) {
	if (tree.rule == "expr term" || tree.rule == "term factor") {
		return typeof(tree.children[0]);
	}
	else if (tree.rule == "expr expr PLUS term") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == "int" && tmp2 == "int") return "int";
		else return "int*";
	}
	else if (tree.rule == "expr expr MINUS term") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == tmp2) return "int";
		else return "int*";
	}
	else if (tree.rule == "lvalue STAR factor" || tree.rule == "factor STAR factor") {
		return "int";
	}
	else if (tree.rule == "lvalue LPAREN lvalue RPAREN" || tree.rule == "factor LPAREN expr RPAREN") {
		return typeof(tree.children[1]);
	}
	else if (tree.rule == "term term STAR factor" || tree.rule == "term term SLASH factor" || tree.rule == "term term PCT factor") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == "int" && tmp2 == "int") return "int";
	}
	else if (tree.rule == "factor AMP lvalue") {
		if (typeof(tree.children[1]) == "int") return "int*";
	}
	else if (tree.rule == "factor NUM") return "int";
	else if (tree.rule == "factor NULL") return "int*";
	else if (tree.rule == "factor ID" || tree.rule == "lvalue ID") return symbolTable1[currP].second[tree.children[0].tokens[1]];
	else if (tree.rule == "factor ID LPAREN RPAREN") return "int";
	else if (tree.rule == "factor ID LPAREN arglist RPAREN") return "int";
	else if (tree.rule == "factor NEW INT LBRACK expr RBRACK") {
		if (typeof(tree.children[3]) == "int") return "int*";
	}
	else if (tree.rule == "test expr EQ expr" || tree.rule == "test expr NE expr" || tree.rule == "test expr LT expr"
		|| tree.rule == "test expr GT expr" || tree.rule == "test expr LE expr" || tree.rule == "test expr GE expr") {
		string tmp = typeof(tree.children[0]);
		if (tmp == typeof(tree.children[2]) && (tmp == "int" || tmp == "int*")) return "well";
	}
	else if (tree.rule == "statements statements statement") {
		string tmp = typeof(tree.children[0]);
		if (tmp == typeof(tree.children[1]) && tmp == "well") return "well";
	}
	else if (tree.rule == "statements" || tree.rule == "dcls") return "well";
	else if (tree.rule == "statement lvalue BECOMES expr SEMI") {
		string tmp1 = typeof(tree.children[0]);
		string tmp2 = typeof(tree.children[2]);
		if (tmp1 == tmp2 && (tmp1 == "int" || tmp1 == "int*")) return "well";
	}
	else if (tree.rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
		if (typeof(tree.children[2]) == "int") return "well";
	}
	else if (tree.rule == "statement DELETE LBRACK RBRACK expr SEMI") {
		if (typeof(tree.children[3]) == "int*") return "well";
	}
	else if (tree.rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
		if (typeof(tree.children[2]) == "well" && typeof(tree.children[5]) == "well") return "well";
	}
	else if (tree.rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
		if (typeof(tree.children[2]) == "well" && typeof(tree.children[5]) == "well" && typeof(tree.children[9]) == "well") return "well";
	}
	else if (tree.rule == "dcls dcls dcl BECOMES NUM SEMI") {
		if (typeof(tree.children[0]) == "well" && tree.children[1].children[0].tokens.size() == 2) return "well";
	}
	else if (tree.rule == "dcls dcls dcl BECOMES NULL SEMI") {
		if (typeof(tree.children[0]) == "well" && tree.children[1].children[0].tokens.size() == 3) return "well";
	}
	else if (tree.rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		if (typeof(tree.children[5]) == "int" && typeof(tree.children[8]) == "well"
			&& typeof(tree.children[9]) == "well" && typeof(tree.children[11]) == "int") return "well";
	}
	else if (tree.rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		if (typeof(tree.children[6]) == "well" && typeof(tree.children[7]) == "well" && typeof(tree.children[9]) == "int") return "well";
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

map<string, int> paramsCount;

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

void push(string);
void get(string);

void checkParam(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		paramsCount[fcn] = paramsCount[fcn] + 4;
		return;
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkParam(tree.children[i], fcn);
	}
}

void checkDcl(parseTree& tree, string fcn) {
	if (tree.rule == "dcl type ID") {
		string type, id;
		type = "int*";
		if (tree.children[0].tokens.size() == 2) type = "int";
		id = tree.children[1].tokens[1];
		symbolTable[fcn][id] = make_pair(type, fcnStack[fcn]);
		if(currP == "wain" && fcnStack["wain"] == 0) {
			push("31");
			if (type == "int") {
				push("2");
				cout << "add $2, $0, $0" << '\n';
				cout << "lis $3" << '\n' << ".word init" << '\n' << "jalr $3" << '\n';
				get("2");
			}
			else cout << "lis $3" << '\n' << ".word init" << '\n' << "jalr $3" << '\n';
			get("31");
		}
		fcnStack[fcn] = fcnStack[fcn] - 4;
		return;
	}
	for (int i = 0; i < tree.children.size(); i++) {
		checkDcl(tree.children[i], fcn);
	}
}

void checkFcn(parseTree& tree) {
	if (tree.rule == "procedures procedure procedures") {
		fcnStack[tree.children[0].children[1].tokens[1]] = 0;
		paramsCount[tree.children[0].children[1].tokens[1]] = 0;
		map<string, pair<string, int>> tmp;
		symbolTable[tree.children[0].children[1].tokens[1]] = tmp;
		currP = tree.children[0].children[1].tokens[1];
		checkDcl(tree.children[0], tree.children[0].children[1].tokens[1]);
		checkFcn(tree.children[1]);
		checkParam(tree.children[0].children[3], tree.children[0].children[1].tokens[1]);
	}
	else if (tree.rule == "procedures main") {
		currP = "wain";
		fcnStack["wain"] = 0;
		map<string, pair<string, int>> tmp;
		symbolTable["wain"] = tmp;
		checkDcl(tree.children[0], "wain");
	}
	else {
		for (int i = 0; i < tree.children.size(); i++) {
			checkFcn(tree.children[i]);
		}
	}
}

void push(string index) { cout << "sw $" << index << ", -4($30)" << '\n' << "sub $30, $30, $4" << '\n'; }
void pop() { cout << "add $30, $30, $4" << '\n' << "lw $5, -4($30)" << '\n'; }
void get(string index) { cout << "add $30, $30, $4" << '\n' << "lw $"<< index << ", -4($30)" << '\n'; }

void code(parseTree& tree) {
	if(tree.rule == "start BOF procedures EOF" || tree.rule == "factor LPAREN expr RPAREN") {
		code(tree.children[1]);
	}
	else if (tree.rule == "expr term" || tree.rule == "term factor") code(tree.children[0]);
	else if (tree.rule == "procedures main") {
		cout << "wain:" << '\n';
		currP = "wain";
		code(tree.children[0]);
	}
	else if (tree.rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		cout << "lis $12" << '\n' << ".word " << to_string(fcnStack[currP] * -1) << '\n' << "sub $30, $30, $12" << '\n';
		cout << "sw $1, 0($29)" << '\n' << "sw $2, -4($29)" << '\n';
		code(tree.children[8]);
		code(tree.children[9]);
		code(tree.children[11]);
	}
	else if (tree.rule == "dcls dcls dcl BECOMES NUM SEMI") {
		code(tree.children[0]);
		cout << "lis $3" << '\n' << ".word " << tree.children[3].tokens[1] << '\n';
		pos = symbolTable[currP][tree.children[1].children[1].tokens[1]].second;
		cout << "sw $3, " << to_string(pos) << "($29)	; declare var" << '\n';
	}
	else if(tree.rule == "dcls dcls dcl BECOMES NULL SEMI"){
    	code(tree.children[0]);
		pos = symbolTable[currP][tree.children[1].children[1].tokens[1]].second;
    	cout << "sw $11, " << to_string(pos) << "($29)	; declare nullptr" << '\n';
    }
	else if (tree.rule == "statement lvalue BECOMES expr SEMI") {
		code(tree.children[2]);
		push("3");
		code(tree.children[0]);
		pop();
		cout << "sw $5, 0($3)	; lvalue assignment" << '\n';
	}
	else if (tree.rule == "lvalue ID") {
		cout << "lis $3" << '\n' << ".word " << to_string(symbolTable[currP][tree.children[0].tokens[1]].second) << '\n' << "add $3, $29, $3" << '\n';
	}
	else if (tree.rule == "lvalue LPAREN lvalue RPAREN") {
		code(tree.children[1]);
	}
	else if (tree.rule == "factor ID") {
		string id = tree.children[0].tokens[1];
		string pos = to_string(symbolTable[currP][id].second);
		cout << "lw $3, "<< pos << "($29)" << '\n';
	}
	else if (tree.rule == "expr expr PLUS term" || tree.rule == "expr expr MINUS term" || tree.rule == "term term STAR factor" 
	|| tree.rule == "term term SLASH factor" || tree.rule == "term term PCT factor") {
		code(tree.children[0]);
		push("3");
		code(tree.children[2]);
		pop();
		if (tree.rule == "expr expr PLUS term" && (typeof(tree.children[0]) == typeof(tree.children[2]))) { // int int
			cout << "add $3, $5, $3		; int int plus" << '\n';
		}
		else if (tree.rule == "expr expr PLUS term" && (typeof(tree.children[0]) == "int*")) { // int* int
			cout << "mult $3, $4" << '\n' << "mflo $3" << '\n' << "add $3, $5, $3		; int* int plus" << '\n';
		}
		else if (tree.rule == "expr expr PLUS term" && (typeof(tree.children[0]) == "int")) { // int int*
			cout << "mult $5, $4" << '\n' << "mflo $5" << '\n' << "add $3, $5, $3		; int int* plus" << '\n';
		}
		else if (tree.rule == "expr expr MINUS term" && (typeof(tree.children[0]) != typeof(tree.children[2]))) { // int* int
			cout << "mult $3, $4" << '\n' << "mflo $3" << '\n' << "sub $3, $5, $3		; int* int minus" << '\n';
		}
		else if (tree.rule == "expr expr MINUS term" && (typeof(tree.children[0]) == "int*")) { // int* int*
			cout << "sub $3, $5, $3" << '\n' << "div $3, $4" << '\n' << "mflo $3		; int* int* minus" << '\n';
		}
		else if (tree.rule == "expr expr MINUS term" && (typeof(tree.children[0]) == "int")) { // int int
			cout << "sub $3, $5, $3		; int int minus" << '\n';
		}
		else if (tree.rule == "term term STAR factor") {
			cout << "mult $3, $5;	mult op" << '\n';
			cout << "mflo $3" << '\n';
		}
		else if (tree.rule == "term term SLASH factor") {
			cout << "div $5, $3;	div op" << '\n';
			cout << "mflo $3" << '\n';
		}
		else {
			cout << "div $5, $3" << '\n';
			cout << "mfhi $3" << '\n';
		}
	}
	else if (tree.rule == "factor NUM") cout << "lis $3" << '\n' << ".word " << tree.children[0].tokens[1] << '\n';
	else if (tree.rule == "statements statements statement") {
		code(tree.children[0]);
		code(tree.children[1]);
	}
	else if (tree.rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
		push("31");
		push("1");
		code(tree.children[2]);
		cout << "add $1, $3, $0" << '\n' << "lis $10" << '\n' << ".word print" << '\n' << "jalr $10" << '\n';
		get("1");
		get("31");
	}
	else if (tree.rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
		controlNum++;
		int tmp = controlNum;
		cout << "begin" << to_string(tmp) << ": 	; loop begin" << '\n';
		code(tree.children[2]);
		cout << "beq $3, $0, " << "end" << to_string(tmp) << '\n';
		code(tree.children[5]);
		cout << "beq $0, $0, " << "begin" << to_string(tmp) << '\n';
		cout << "end" << to_string(tmp) << ": 	; loop end" << '\n';
	}
	else if (tree.rule == "test expr LT expr" || tree.rule == "test expr EQ expr" || tree.rule == "test expr NE expr"
	|| tree.rule == "test expr LE expr" || tree.rule == "test expr GE expr" || tree.rule == "test expr GT expr") {
		code(tree.children[0]);
		push("3");
		code(tree.children[2]);
		pop();
		string instr = "slt";
		if(typeof(tree.children[0]) == "int*") instr = "sltu";
		if (tree.tokens[2] == "LT") cout << instr <<" $3, $5, $3" << '\n';
		else if (tree.tokens[2] == "EQ") {
			cout << instr << " $6, $3, $5" << '\n';
			cout << instr << " $7, $5, $3" << '\n';
			cout << "add $3, $6, $7" << '\n';
			cout << "sub $3, $11, $3" << '\n';
		}
		else if (tree.tokens[2] == "NE") {
			cout << instr << " $6, $3, $5" << '\n';
			cout << instr << " $7, $5, $3" << '\n';
			cout << "add $3, $6, $7" << '\n';
		}
		else if (tree.tokens[2] == "LE") {
			cout << instr << " $3, $3, $5" << '\n';
			cout << "sub $3, $11, $3" << '\n';
		}
		else if (tree.tokens[2] == "GE") {
			cout << instr << " $3, $5, $3" << '\n';
			cout << "sub $3, $11, $3" << '\n';
		}
		else if (tree.tokens[2] == "GT") cout << instr << " $3, $3, $5" << '\n';
	}
	else if (tree.rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
		controlNum++;
		int tmp = controlNum;
		code(tree.children[2]);
		cout << "beq $3, $0, " << "else" << to_string(tmp) << '\n';
		code(tree.children[5]);
		cout << "beq $0, $0, " << "end" << to_string(tmp) << '\n';
		cout << "else" << to_string(tmp) << ": 	; loop end" << '\n';
		code(tree.children[9]);
		cout << "end" << to_string(tmp) << ": 	; loop end" << '\n';
	}
	else if (tree.rule == "factor STAR factor") {
		code(tree.children[1]);
		cout << "lw $3, 0($3)" << '\n';
	}
	else if (tree.rule == "factor NULL") cout << "add $3, $0, $11" << '\n';
	else if (tree.rule == "factor AMP lvalue" || tree.rule == "lvalue STAR factor") code(tree.children[1]);
	else if (tree.rule == "factor NEW INT LBRACK expr RBRACK") {
		code(tree.children[3]);
		push("1");
		cout << "sub $1, $3, $0" << '\n';
		push("31");
		cout << "lis $10" << '\n' << ".word new" << '\n' << "jalr $10" << '\n';
		cout << "bne $3, $0, 1" << '\n';
    	cout << "add $3, $0, $11" << '\n';
		get("31");
		get("1");
	}
	else if (tree.rule == "statement DELETE LBRACK RBRACK expr SEMI") {
		code(tree.children[3]);
		push("1");
		cout << "sub $1, $3, $0" << '\n';
		push("31");
		cout << "beq $11, $1, 3" << '\n';
		cout << "lis $10" << '\n' << ".word delete" << '\n' << "jalr $10" << '\n';
		get("31");
		get("1");
	}
	else if (tree.rule == "procedures procedure procedures") {
		code(tree.children[0]);
		code(tree.children[1]);
	}
	else if (tree.rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
		currP = tree.children[1].tokens[1];
		cout << "FCN" << currP << ":" << '\n';
		cout << "sub $29, $30, $4" << '\n';
		cout << "lis $12" << '\n' << ".word " << to_string(paramsCount[currP]) << '\n' << "add $29, $29, $12" << '\n';
		cout << "lis $12" << '\n' << ".word " << to_string(fcnStack[currP] * -1 - paramsCount[currP]) << '\n' << "sub $30, $30, $12" << '\n';
		code(tree.children[6]);
		code(tree.children[7]);
		code(tree.children[9]);
		cout << "add $30, $29, $4" << '\n';
		cout << "jr $31" << '\n';
	}
	else if (tree.rule == "factor ID LPAREN RPAREN") {
		push("31");
		push("29");
		cout << "lis $5" << '\n' << ".word " << "FCN" << tree.children[0].tokens[1] << '\n' << "jalr $5" << '\n';
		get("29");
		get("31");
	}
	else if (tree.rule == "arglist expr") {
		code(tree.children[0]);
		push("3");
	}
	else if (tree.rule == "arglist expr COMMA arglist") {
		code(tree.children[0]);
		push("3");
		code(tree.children[2]);
	}
	else if (tree.rule == "factor ID LPAREN arglist RPAREN") {
		push("31");
		push("29");
		code(tree.children[2]);
		cout << "lis $5" << '\n' << ".word " << "FCN" << tree.children[0].tokens[1] << '\n' << "jalr $5" << '\n';
		get("29");
		get("31");
	}
	else return;
}

int main() {
	cout << ".import print" << '\n' << ".import init" << '\n' << ".import new" << '\n' << ".import delete" << '\n'; 
	cout << "lis $4" << '\n' << ".word 4" << '\n' << "lis $11" << '\n' << ".word 1" << '\n' << "sub $29, $30, $4" << '\n';

	parseTree Tree;
	trav(cin, Tree);
	checkFcn1(Tree);

	currP = "wain";
	checkFcn(Tree);
	cout << "beq $0, $0, wain" << '\n';
	code(Tree);
	cout << "add $30, $29, $4" << '\n' << "jr $31" << '\n';
}
