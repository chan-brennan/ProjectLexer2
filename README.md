# Lexer for FIRST & FOLLOW Project
## Overview
This project implements a modified lexical analyzer (lexer) for a programming language, designed specifically for generating the FIRST and FOLLOW sets, which are commonly used in parser generation for context-free grammars (CFGs). The lexer reads input and breaks it down into tokens relevant for syntax analysis, such as ARROW, STAR, and HASH.

## Features
Token Identification: The lexer identifies specific tokens like arrows (->), stars (*), hashes (#), identifiers, and errors.
Internal Token List: The lexer pre-processes all tokens in the input and stores them in an internal list for easy retrieval during parsing.
Peek Functionality: The lexer supports peeking at upcoming tokens without consuming them, which is useful in parser lookahead scenarios.
## Key Components
Token Class: Defines the structure of a token with attributes like lexeme, token_type, and line_no.
LexicalAnalyzer Class: Contains the main logic for tokenization and managing the token list. Key methods include:
GetToken(): Retrieves tokens from the pre-processed token list.
GetTokenMain(): Processes and generates tokens from input.
peek(int howFar): Peeks ahead in the token list without advancing the current position.
SkipSpace(): Skips whitespace characters in the input.
ScanId(): Scans and identifies alphanumeric sequences as identifiers.
## Token Types
The lexer supports the following token types:
Symbols: ARROW (->), STAR (*), HASH (#)
Identifiers: Alphanumeric sequences starting with a letter.
Special Tokens: END_OF_FILE, ERROR
## How It Works
Token Preprocessing: When the lexer object is instantiated, the entire input is tokenized and stored in a list (tokenList). This allows efficient retrieval and lookahead during parsing.
Token Retrieval: The GetToken() method fetches tokens sequentially from the token list. It returns END_OF_FILE once all tokens have been processed.
Lookahead (Peeking): The peek() method allows the parser to look ahead a specified number of tokens without consuming them, enabling more advanced parsing strategies.
Error Handling: Invalid tokens are flagged as ERROR, allowing the parser to handle unexpected input.
## Usage
To integrate this lexer into a project, instantiate a LexicalAnalyzer object and use GetToken() to retrieve tokens. You can also use peek() for lookahead functionality.
