#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"
using namespace std;


int main() {
  std::string line;
  std::string kind;
  try {
    while (getline(std::cin, line)) {
      // For example, just print the scanned tokens
      std::vector<Token> tokenLine = scan(line);
      for (auto &token : tokenLine) {
		  if (token.getKind() == 0) {
			  if (token.getLexeme() == "wain") cout << "WAIN" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "int") cout << "INT" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "if") cout << "IF" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "else") cout << "ELSE" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "while") cout << "WHILE" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "println") cout << "PRINTLN" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "return") cout << "RETURN" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "NULL") cout << "null" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "new") cout << "NEW" << ' ' << token.getLexeme() << '\n';
			  else if (token.getLexeme() == "delete") cout << "DELETE" << ' ' << token.getLexeme() << '\n';
			  else cout << "ID" << ' ' << token.getLexeme() << '\n';
		  }
		  else if (token.getKind() == 1) cout << "NUM" << ' ' << token.getLexeme() << '\n';
		  else if (token.getKind() == 2) {
			  if (token.getLexeme() == "0") cout << "NUM";
			  else if (token.getLexeme() == "(") cout << "LPAREN";
			  else if (token.getLexeme() == ")") cout << "RPAREN";
			  else if (token.getLexeme() == "[") cout << "LBRACK";
			  else if (token.getLexeme() == "]") cout << "RBRACK";
			  else if (token.getLexeme() == "{") cout << "LBRACE";
			  else if (token.getLexeme() == "}") cout << "RBRACE";
			  else if (token.getLexeme() == "+") cout << "PLUS";
			  else if (token.getLexeme() == "-") cout << "MINUS";
			  else if (token.getLexeme() == "*") cout << "STAR";
			  else if (token.getLexeme() == "%") cout << "PCT";
			  else if (token.getLexeme() == ",") cout << "COMMA";
			  else if (token.getLexeme() == ";") cout << "SEMI";
			  else if (token.getLexeme() == "&") cout << "AMP";
			  cout << ' ' << token.getLexeme() << '\n';
		  }
		  else if (token.getKind() == 3) {
			  if (token.getLexeme() == "=") cout << "BECOMES";
			  else if (token.getLexeme() == ">") cout << "GT";
			  else if (token.getLexeme() == "<") cout << "LT";
			  cout << ' ' << token.getLexeme() << '\n';
		  }
		  else if (token.getKind() == 4) {
			  if (token.getLexeme() == "==") cout << "EQ";
			  else if (token.getLexeme() == ">=") cout << "GE";
			  else if (token.getLexeme() == "<=") cout << "LE";
			  else if (token.getLexeme() == "!=") cout << "NE";
			  cout << ' ' << token.getLexeme() << '\n';
		  }
		  else if (token.getKind() == 5) cout << "SLASH " << token.getLexeme() << '\n';
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;

    return 1;
  }

  return 0;
}
