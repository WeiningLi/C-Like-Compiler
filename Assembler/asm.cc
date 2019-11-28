#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"
#include <map>
using namespace std;

int main() {
	std::string line;
	int errorflag = 0;
	int memoCount = 0;
	int memoCount2 = 0;
	int lineCount = 0;
	vector<string> secondPass;
	map<string, int> symbolTable;
	try {
		while (getline(std::cin, line)) {
			lineCount++;
			secondPass.push_back(line);
			std::vector<Token> tokenLine = scan(line);
			for (auto i = 0; i < tokenLine.size(); i++) { // first pass

				if (tokenLine[i].getKind() == Token::Kind::LABEL) {
					for (int j = 0; j < i; j++) {
						if (tokenLine[j].getKind() != Token::Kind::LABEL) {
							errorflag = 1;
							cerr << "ERROR! label not in line beginning" << '\n';
							break;
						}
					}
					string name = tokenLine[i].getLexeme();
					name.pop_back();
					if (symbolTable.count(name) == 0) {
						symbolTable.insert(pair<string, int>(name, memoCount));
					}
					else {
						errorflag = 1;
						cerr << "ERROR! repeat label name" << '\n';
						break;
					}
				}

				else if (tokenLine[i].getKind() == Token::Kind::WORD) {
					i++;
					if (tokenLine[i].getKind() == Token::Kind::INT || tokenLine[i].getKind() == Token::Kind::HEXINT || 
						tokenLine[i].getKind() == Token::Kind::ID) {
						if (i + 1 != tokenLine.size()) {
							errorflag = 1;
							cerr << "ERROR! more stuff after instruction end" << '\n';
							break;
						}
						int64_t instr = tokenLine[i].toLong();
						if (instr > 4294967295 || instr < -2147483648) {
							errorflag = 1;
							cerr << "ERROR! out of range" << '\n';
							break;
						}
						else memoCount += 4;
					}
					else {
						errorflag = 1;
						cerr << "ERROR! unrecognized part after .word" << '\n';
						break;
					}
				}

				else if (tokenLine[i].getKind() == Token::Kind::ID) {
					string cmd = tokenLine[i].getLexeme();
					i++;
					if (cmd == "jr" || cmd == "jalr") {

						if (tokenLine[i].getKind() == Token::Kind::REG) {
							if (i + 1 != tokenLine.size()) {
								errorflag = 1;
								cerr << "ERROR! more stuff after instruction end" << '\n';
								break;
							}
							string reg = tokenLine[i].getLexeme();
							if (stoi(reg.substr(1)) > 31 || stoi(reg.substr(1)) < 0) {
								errorflag = 1;
								cerr << "ERROR! reg index out of range" << '\n';
								break;
							}
							else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! unrecognized part after jump cmd" << '\n';
							break;
						}
					}

					else if (cmd == "sw" || cmd == "lw") {
						if (i + 6 == tokenLine.size() &&
							tokenLine[i].getKind() == Token::Kind::REG &&
							tokenLine[i + 1].getKind() == Token::Kind::COMMA &&
							(tokenLine[i + 2].getKind() == Token::Kind::INT || tokenLine[i + 2].getKind() == Token::Kind::HEXINT)&&
							tokenLine[i + 3].getKind() == Token::Kind::LPAREN &&
							tokenLine[i + 4].getKind() == Token::Kind::REG &&
							tokenLine[i + 5].getKind() == Token::Kind::RPAREN) {
							string reg1 = tokenLine[i].getLexeme();
							i += 2;
							int64_t val = tokenLine[i].toLong();
							i += 2;
							string reg2 = tokenLine[i].getLexeme();
							i++;
							if (stoi(reg1.substr(1)) > 31 || stoi(reg1.substr(1)) < 0 ||
								stoi(reg2.substr(1)) > 31 || stoi(reg2.substr(1)) < 0 ||
								((tokenLine[i - 3].getKind() == Token::Kind::INT && (val < -32768 || val > 32767)) ||
								(tokenLine[i - 3].getKind() == Token::Kind::HEXINT && (val < 0 || val > 65535)))) {
								errorflag = 1;
								cerr << "ERROR! reg index/input value out of range" << '\n';
								break;
							}
							else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! incorrect # of parameters after cmd or unrecognized part after branch cmd" << '\n';
							break;
						}
					}

					else if (cmd == "mult" || cmd == "multu" || cmd == "div" || cmd == "divu") {

						if (i + 3 == tokenLine.size() &&
							tokenLine[i].getKind() == Token::Kind::REG &&
							tokenLine[i + 1].getKind() == Token::Kind::COMMA &&
							tokenLine[i + 2].getKind() == Token::Kind::REG) {
							string reg1 = tokenLine[i].getLexeme();
							i += 2;
							string reg2 = tokenLine[i].getLexeme();
								if (stoi(reg1.substr(1)) > 31 || stoi(reg1.substr(1)) < 0 ||
									stoi(reg2.substr(1)) > 31 || stoi(reg2.substr(1)) < 0) {
									errorflag = 1;
									cerr << "ERROR! reg index/input value out of range" << '\n';
									break;
								}
								else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! incorrect # of parameters after cmd or unrecognized part after branch cmd" << '\n';
							break;
						}
					}

					else if (cmd == "lis" || cmd == "mflo" || cmd == "mfhi") {

						if (i + 1 != tokenLine.size()) {
							errorflag = 1;
							cerr << "ERROR! more stuff after instruction end" << '\n';
							break;
						}

						if (tokenLine[i].getKind() == Token::Kind::REG) {
							string reg = tokenLine[i].getLexeme();
							if (stoi(reg.substr(1)) > 31 || stoi(reg.substr(1)) < 0) {
								errorflag = 1;
								cerr << "ERROR! reg index out of range" << '\n';
								break;
							}
							else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! unrecognized part after lis/mf cmd" << '\n';
							break;
						}
					}

					else if (cmd == "beq" || cmd == "bne") {

						if (i + 5 == tokenLine.size() &&
							tokenLine[i].getKind() == Token::Kind::REG &&
							tokenLine[i + 1].getKind() == Token::Kind::COMMA &&
							tokenLine[i + 2].getKind() == Token::Kind::REG &&
							tokenLine[i + 3].getKind() == Token::Kind::COMMA &&
							(tokenLine[i + 4].getKind() == Token::Kind::ID ||
							tokenLine[i + 4].getKind() == Token::Kind::INT || tokenLine[i + 4].getKind() == Token::Kind::HEXINT)) {
							string reg1 = tokenLine[i].getLexeme();
							i += 2;
							string reg2 = tokenLine[i].getLexeme();
							i += 2;
							if (tokenLine[i].getKind() != Token::Kind::ID) {
								int64_t val = tokenLine[i].toLong();
								if (stoi(reg1.substr(1)) > 31 || stoi(reg1.substr(1)) < 0 ||
									stoi(reg2.substr(1)) > 31 || stoi(reg2.substr(1)) < 0 ||
									((tokenLine[i].getKind() == Token::Kind::INT && (val < -32768 || val > 32767)) ||
									(tokenLine[i].getKind() == Token::Kind::HEXINT && (val < 0 || val > 65535)))) {
									errorflag = 1;
									cerr << "ERROR! reg index/input value out of range" << '\n';
									break;
								}
								else memoCount += 4;
							}
							else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! incorrect # of parameters after cmd or unrecognized part after branch cmd" << '\n';
							break;
						}
					}

					else if (cmd == "add" || cmd == "sub" || cmd == "slt" || cmd == "sltu") {

						if (i + 5 == tokenLine.size() &&
							tokenLine[i].getKind() == Token::Kind::REG && 
							tokenLine[i + 1].getKind() == Token::Kind::COMMA &&
							tokenLine[i + 2].getKind() == Token::Kind::REG &&
							tokenLine[i + 3].getKind() == Token::Kind::COMMA &&
							tokenLine[i + 4].getKind() == Token::Kind::REG) {
							string reg1 = tokenLine[i].getLexeme();
							i += 2;
							string reg2 = tokenLine[i].getLexeme();
							i += 2;
							string reg3 = tokenLine[i].getLexeme();
							if (stoi(reg1.substr(1)) > 31 || stoi(reg1.substr(1)) < 0 ||
								stoi(reg2.substr(1)) > 31 || stoi(reg2.substr(1)) < 0 || 
								stoi(reg3.substr(1)) > 31 || stoi(reg3.substr(1)) < 0) {
								errorflag = 1;
								cerr << "ERROR! reg index out of range" << '\n';
								break;
							}
							else memoCount += 4;
						}
						else {
							errorflag = 1;
							cerr << "ERROR! incorrect # of parameters after cmd or unrecognized part after cmd" << '\n';
							break;
						}
					}

					else {
						errorflag = 1;
						cerr << "ERROR! unrecognized cmd" << '\n';
						break;
					}
				}

				else {
					errorflag = 1;
					cerr << "ERROR! unrecognized label" << '\n';
					break;
				}
			}
		}

		for (int i = 0; i < lineCount; i++) { // second pass
			vector<Token> tokenLine = scan(secondPass[i]);
			for (auto i = 0; i < tokenLine.size(); i++) {

				if (tokenLine[i].getKind() == Token::Kind::ID) {
					string cmd = tokenLine[i].getLexeme();
					i++;
					int instr;
					if (cmd == "add" || cmd == "sub" || cmd == "slt" || cmd == "sltu") {
						string reg1 = tokenLine[i].getLexeme();
						int regNum1 = stoi(reg1.substr(1));
						i += 2;
						string reg2 = tokenLine[i].getLexeme();
						int regNum2 = stoi(reg2.substr(1));
						i += 2;
						string reg3 = tokenLine[i].getLexeme();
						int regNum3 = stoi(reg3.substr(1));
						if (cmd == "add") instr = (0 << 26) | (regNum2 << 21) | (regNum3 << 16) | (regNum1 << 11) | (0 << 8) | (32 | 0x00);
						else if (cmd == "sub") instr = (0 << 26) | (regNum2 << 21) | (regNum3 << 16) | (regNum1 << 11) | (0 << 8) | (34 | 0x00);
						else if (cmd == "slt") instr = (0 << 26) | (regNum2 << 21) | (regNum3 << 16) | (regNum1 << 11) | (0 << 8) | (42 | 0x00);
						else if (cmd == "sltu") instr = (0 << 26) | (regNum2 << 21) | (regNum3 << 16) | (regNum1 << 11) | (0 << 8) | (43 | 0x00);
					}

					else if (cmd == "mult" || cmd == "multu" || cmd == "div" || cmd == "divu") {
						string reg1 = tokenLine[i].getLexeme();
						int regNum1 = stoi(reg1.substr(1));
						i += 2;
						string reg2 = tokenLine[i].getLexeme();
						int regNum2 = stoi(reg2.substr(1));
						if (cmd == "mult") instr = (0 << 26) | (regNum1 << 21) | (regNum2 << 16) | (24 | 0x00);
						else if (cmd == "multu") instr = (0 << 26) | (regNum1 << 21) | (regNum2 << 16) | (25 | 0x00);
						else if (cmd == "div") instr = (0 << 26) | (regNum1 << 21) | (regNum2 << 16) | (26 | 0x00);
						else if (cmd == "divu") instr = (0 << 26) | (regNum1 << 21) | (regNum2 << 16) | (27 | 0x00);
					}

					else if (cmd == "jr" || cmd == "jalr") {
						string reg = tokenLine[i].getLexeme();
						int regNum = stoi(reg.substr(1));
						if (cmd == "jr") instr = (0 << 26) | (regNum << 21) | (0 << 16) | (8 | 0x0000);
						else if (cmd == "jalr") instr = (0 << 26) | (regNum << 21) | (0 << 16) | (9 | 0x0000);
					}

					else if (cmd == "lis" || cmd == "mflo" || cmd == "mfhi") {
						string reg = tokenLine[i].getLexeme();
						int regNum = stoi(reg.substr(1));
						if (cmd == "lis") instr = (0 << 16) | (regNum << 11) | (20 | 0x0000);
						else if (cmd == "mflo") instr = (0 << 16) | (regNum << 11) | (18 | 0x0000);
						else if (cmd == "mfhi") instr = (0 << 16) | (regNum << 11) | (16 | 0x0000);
					}

					else if (cmd == "sw" || cmd == "lw") {
						string reg1 = tokenLine[i].getLexeme();
						int regNum1 = stoi(reg1.substr(1));
						i += 2;
						int64_t val = tokenLine[i].toLong();
						i += 2;
						string reg2 = tokenLine[i].getLexeme();
						int regNum2 = stoi(reg2.substr(1));
						i++;
							if (cmd == "sw") {
								if (val < 0) instr = (43 << 26) | (regNum2 << 21) | (regNum1 << 16) | (val & 0xffff);
								else instr = (43 << 26) | (regNum2 << 21) | (regNum1 << 16) | (val | 0x0000);
							}
							else if (cmd == "lw") {
								if (val < 0) instr = (35 << 26) | (regNum2 << 21) | (regNum1 << 16) | (val & 0xffff);
								else instr = (35 << 26) | (regNum2 << 21) | (regNum1 << 16) | (val | 0x0000);
							}
					}

					else if (cmd == "bne" || cmd == "beq") {
						string reg1 = tokenLine[i].getLexeme();
						int regNum1 = stoi(reg1.substr(1));
						i += 2;
						string reg2 = tokenLine[i].getLexeme();
						int regNum2 = stoi(reg2.substr(1));
						i += 2;
						if (tokenLine[i].getKind() == Token::Kind::INT || tokenLine[i].getKind() == Token::Kind::HEXINT) {
							int64_t val = tokenLine[i].toLong();
							if (cmd == "beq") {
								if (val < 0) instr = (4 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val & 0xffff);
								else instr = (4 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val | 0x0000);
							}
							else if (cmd == "bne") {
								if (val < 0) instr = (5 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val & 0xffff);
								else instr = (5 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val | 0x0000);
							}
						}
						else { // label
							if (symbolTable.count(tokenLine[i].getLexeme()) == 0) {
								errorflag = 1;
								cerr << "ERROR! unrecognized ID for label" << '\n';
								break;
							}
							int val = symbolTable[tokenLine[i].getLexeme()];
							val = (val - memoCount2 - 4) / 4;
							if (val < -32768 || val > 32767) {
								errorflag = 1;
								cerr << "ERROR! reg index/input value out of range" << '\n';
								break;
							}
							if (cmd == "beq") {
								if (val < 0) instr = (4 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val & 0xffff);
								else instr = (4 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val | 0x0000);
							}
							if (cmd == "bne") {
								if (val < 0) instr = (5 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val & 0xffff);
								else instr = (5 << 26) | (regNum1 << 21) | (regNum2 << 16) | (val | 0x0000);
							}
						}
					}

					for (int i = 24; i > 0; i -= 8) {
						char c = instr >> i;
						cout << c;
					}
					char c = instr;
					cout << c;
					memoCount2+=4;
					break;
				}

				else if (tokenLine[i].getKind() == Token::Kind::WORD) {
					i++;
					if (tokenLine[i].getKind() == Token::Kind::INT || tokenLine[i].getKind() == Token::Kind::HEXINT) {
						int64_t instr = tokenLine[i].toLong();
						if (instr >= 2147483648) {
							instr = instr - 2147483648 * 2;
						}
						for (int i = 24; i > 0; i -= 8) {
							char c = instr >> i;
							cout << c;
						}
						char c = instr;
						cout << c;
						memoCount2+=4;
						break;
					}
					else if (tokenLine[i].getKind() == Token::Kind::ID) {
						if (symbolTable.count(tokenLine[i].getLexeme()) == 0) {
							errorflag = 1;
							cerr << "ERROR! unrecognized ID after .word" << '\n';
							break;
						}
						int instr = symbolTable[tokenLine[i].getLexeme()];
						for (int i = 24; i > 0; i -= 8) {
							char c = instr >> i;
							cout << c;
						}
						char c = instr;
						cout << c;
						memoCount2+=4;
						break;
					}
				}
			}
		}

		if (errorflag == 0) {
			for (auto const& item : symbolTable) {
				cerr << item.first << " " << item.second << endl;
			}
		}

	}
	catch (ScanningFailure &f) {
		std::cerr << f.what() << std::endl;

		return 1;
	}

	return 0;
}
