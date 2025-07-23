#pragma once
#include <string>

enum TokenType
{
	TOKEN_WORD,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_SEMICOLON,
	TOKENS_AMOUNT
};

struct Token
{
	TokenType type;
	std::string value;
};