#include "member.h"

Member::Member()
{
	color = WHITE;
	parent = -1;
	distance = -1;
	adjacent.clear();
}

Member::~Member()
{
}
