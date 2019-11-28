#ifndef CS241_SCANNER_H
#define CS241_SCANNER_H
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <ostream>

// This file contains helpers for asm.cc and should not need to be modified by

class Token;

/* Scans a single line of input and produces a list of tokens.
 *
 * Scan returns tokens with the following kinds:
 * ID: identifiers and keywords.
 * LABEL: labels (identifiers ending in a colon).
 * WORD: the special ".word" keyword.
 * COMMA: a comma.
 * LPAREN: a left parenthesis.
 * RPAREN: a right parenthesis.
 * INT: a signed or unsigned 32-bit integer written in decimal.
 * HEXINT: an unsigned 32-bit integer written in hexadecimal.
 * REG: a register between $0 and $31.
 */

std::vector<Token> scan(const std::string &input);

/* A scanned token produced by the scanner.
 * The "kind" tells us what kind of token it is
 * while the "lexeme" tells us exactly what text
 * the programmer typed. For example, the token
 * "abc" might have kind "ID" and lexeme "abc".
 *
 * While you can create tokens with any kind and
 * lexeme, the list of kinds produced by the
 * starter code can be found in the documentation
 * for scan above.
 */
class Token {
  public:
    enum Kind {
      ID = 0,
      NUM,
	  SINGLEEND,
      EQ,
	  EQEND,
	  SLASH,
	  COMMENT,
      WHITESPACE
    };

  private:
    Kind kind;
    std::string lexeme;

  public:
    Token(Kind kind, std::string lexeme);

    Kind getKind() const;
    const std::string &getLexeme() const;

	int64_t toLong() const;

};

/* An exception class thrown when an error is encountered while scanning.
 */
class ScanningFailure {
    std::string message;

  public:
    ScanningFailure(std::string message);

    // Returns the message associated with the exception.
    const std::string &what() const;
};

#endif
