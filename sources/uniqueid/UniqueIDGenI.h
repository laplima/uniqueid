#ifndef UNIQUEIDGENI_H_
#define UNIQUEIDGENI_H_

#include "UniqueIDGenS.h"
#include <colibry/Bag.h>
#include <cstddef>
#include <string>

// ------------------------------------------------------

class SeqString {
public:
	SeqString(std::size_t sz, char first_char, char last_char);
	void reset();
	bool is_last();
	virtual std::string next();
	operator std::string() const { return ss_; }
protected:
	std::string ss_;
	char first_, last_;
};

// hex sequence
class HexSeq : public SeqString {
public:
	HexSeq(std::size_t sz);
	std::string next() override;
};

// ------------------------------------------------------

namespace UIDGen {

	class  UniqueIDGenImpl : public virtual POA_UIDGen::UniqueIDGen {
	public:

		UniqueIDGenImpl();

		::UIDGen::ID_t getuid () override;
		void putback (::UIDGen::ID_t id) override;
		char *getustr() override;
		bool reset(const char* passwd) override;

	protected:

		colibry::Bag<ID_t> bag_;
		const std::function<void()> notify_shutdown_;
		HexSeq hseq_;
	};

};

#endif
