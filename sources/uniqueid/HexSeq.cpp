#include "HexSeq.h"

HexSeq& HexSeq::operator++()
{
	if (is_last())
		reset();
	else {
		for (auto i=ss_.size()-1; i>=0; --i) {
			if (ss_[i] == '9')
				ss_[i] = 'A';
			else
				++ss_[i];
			if (ss_[i] <= last_)
				break;
			ss_[i] = first_;
		}
	}
	return *this;
}
