#include "HandlingResult.hpp"

HandlingResult::HandlingResult()
{
	this->code_ = (t_httpCode)0;
    this->isCGI_ = false;
    this->mode_ = C_KEEP_ALIVE;
}

HandlingResult::~HandlingResult()
{
}
