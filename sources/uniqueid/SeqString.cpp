#include "SeqString.h"
#include <algorithm>

using namespace std;

SeqString::SeqString(std::size_t sz, char first_char, char last_char)
		: ss_(sz, first_char), first_{first_char}, last_{last_char}
{}

void SeqString::reset()
{
	for (char& c : ss_)
		c = first_;
}

bool SeqString::is_last()
{
	return ranges::all_of(ss_, [this](char c) { return (c == last_); });
}

SeqString& SeqString::operator++()
{
	if (is_last())
		reset(); // start over
	else {
		for (auto i=ss_.size()-1; i>=0; --i) {
			++ss_[i];
			if (ss_[i] <= last_)
				break;
			ss_[i] = first_;
		}
	}
	return *this;
}

