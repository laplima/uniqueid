#ifndef HEXSEQ_H
#define HEXSEQ_H

#include "SeqString.h"

// hex sequence
class HexSeq : public SeqString {
public:
	explicit HexSeq(std::size_t sz)  : SeqString{sz, '0', 'F'} {}
	HexSeq& operator++() override;
};

#endif
