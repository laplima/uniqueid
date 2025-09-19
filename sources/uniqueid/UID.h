//
// High-level implementation (called by the actual servant)
//

#ifndef UID_H
#define UID_H

#include <string>
#include "UniqueIDGenI.h"
#include "HexSeq.h"

constexpr UIDGen::ID_t maxid = 10'000'000'000;
constexpr int uslen = 16;				// unique string length

// ------------------------------------------------------

class UID {
public:
	UID() : uidi_{this}, bag_{1, maxid}, hseq_{uslen} {}

	auto& in() { return uidi_; }

	UIDGen::ID_t getuid ();
	void putback (UIDGen::ID_t id);
	std::string getustr();
	bool reset(const std::string& passwd);

private:
	UIDGen::UniqueIDGen_i uidi_;
	colibry::Bag<UIDGen::ID_t> bag_;
	HexSeq hseq_;
};

#endif

