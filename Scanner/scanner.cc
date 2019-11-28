#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <utility>
#include <set>
#include <array>
#include <iostream>
#include "scanner.h"



Token::Token(Token::Kind kind, std::string lexeme):
  kind(kind), lexeme(std::move(lexeme)) {}

  Token:: Kind Token::getKind() const { return kind; }
const std::string &Token::getLexeme() const { return lexeme; }


int64_t Token::toLong() const {
	std::istringstream iss;
	int64_t result;

	if (kind == NUM) {
		iss.str(lexeme);
	}

	iss >> result;
	return result;
}

ScanningFailure::ScanningFailure(std::string message):
  message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }

/* Represents a DFA (which you will see formally in class later)
 * to handle the scanning
 * process. You should not need to interact with this directly:
 * it is handled through the starter code.
 */
class AsmDFA {
  public:
    enum State {
      // States that are also kinds
      ID = 0,
	  NUM,
	  SINGLEEND,
      EQ,
	  EQEND,
      WHITESPACE,
	  COMMENT,
	  SLASH,

      // States that are not also kinds
      FAIL,
      START,
      EX,
	  LARGEST_STATE = EX
    };

  private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
      switch(s) {
        case ID:			return Token::ID;
        case NUM:			return Token::NUM;
        case SINGLEEND:     return Token::SINGLEEND;
        case EQ:			return Token::EQ;
		case EQEND:			return Token::EQEND;
		case SLASH:			return Token::SLASH;
		case COMMENT:		return Token::COMMENT;
        case WHITESPACE:	return Token::WHITESPACE;
        default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
      }
    }


  public:
    /* Tokenizes an input string according to the SMM algorithm.
     * You will learn the SMM algorithm in class around the time of Assignment 6.
     */
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;

      State state = start();
      std::string munchedInput;

      // We can't use a range-based for loop effectively here
      // since the iterator doesn't always increment.
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {

        State oldState = state;
        state = transition(state, *inputPosn);

        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;

          ++inputPosn;
        }

        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));

            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input:("
                                 + munchedInput  + ")");
          }
        }
      }

      return result;
    }

    /* Initializes the accepting states for the DFA.
     */
    AsmDFA() {
      acceptingStates = {ID, NUM, SINGLEEND, EQ, EQEND, SLASH, COMMENT, WHITESPACE};
      //Non-accepting states are DOT, MINUS, ZEROX, DOLLARS, START

      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }

      registerTransition(START, isalpha, ID);
      registerTransition(START, "0", SINGLEEND);
      registerTransition(START, "123456789", NUM);
	  registerTransition(START, "(){}+-*%,;[]&", SINGLEEND);
      registerTransition(START, "=><", EQ);
      registerTransition(START, "!", EX);
      registerTransition(START, "/", SLASH);
	  registerTransition(START, isspace, WHITESPACE);
      registerTransition(ID, isalnum, ID);
	  registerTransition(NUM, isdigit, NUM);
	  registerTransition(EX, "=", EQEND);
	  registerTransition(EQ, "=", EQEND);
	  registerTransition(SLASH, "/", COMMENT);
      registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
      registerTransition(WHITESPACE, isspace, WHITESPACE);
    }
	
    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};

std::vector<Token> scan(const std::string &input) {
  static AsmDFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

  std::vector<Token> newTokens;
  if (tokens.size() > 0) {
	  Token tmp = tokens[0];
	  int start = 0;
	  for (auto &token : tokens) {
		  if (start != 0) {
			  if ((token.getKind() == 1 || token.getLexeme() == "0")
				  && tmp.getKind() == 2 && tmp.getLexeme() == "0") {
				  throw ScanningFailure("ERROR: 0 lead NUM");
			  }
			  else if (token.getKind() == 0 && (tmp.getKind() == 1 || tmp.getLexeme() == "0")) throw ScanningFailure("ERROR: NUM followed by ID");
			  else if ((tmp.getKind() == 3 || tmp.getKind() == 4) && (token.getKind() == 3 || token.getKind() == 4)) {
				  throw ScanningFailure("ERROR: Consective comparing operations");
			  }
			  else if (tmp.getKind() == 1 && (tmp.toLong() > 2147483647 || tmp.toLong() < -2147483648)) {
				  throw ScanningFailure("ERROR: NUM out of range");
			  }
		  }
		  if (start == 0) start++;
		  tmp = token;
		  if (token.getKind() != Token::WHITESPACE && token.getKind() != Token::COMMENT) {
			  newTokens.push_back(token);
			  tmp = token;
		  }
	  }
  }
  return newTokens;
}
