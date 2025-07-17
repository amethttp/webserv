#include "HandlingResult.hpp"

HandlingResult::HandlingResult()
{
    this->isCGI_ = false;
    this->mode_ = C_KEEP_ALIVE;
}

HandlingResult::~HandlingResult()
{
}
