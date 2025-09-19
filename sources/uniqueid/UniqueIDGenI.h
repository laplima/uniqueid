#ifndef UNIQUEIDGENI_H_
#define UNIQUEIDGENI_H_

#include "UniqueIDGenS.h"
#include <colibry/Bag.h>
#include <cstddef>
#include <string>

class UID;

namespace UIDGen {

	class  UniqueIDGen_i : public virtual POA_UIDGen::UniqueIDGen {
	public:

		UniqueIDGen_i(UID* uid) : uid_{uid} {}

		::UIDGen::ID_t getuid () override;
		void putback (::UIDGen::ID_t id) override;
		char *getustr() override;
		bool reset(const char* passwd) override;

	protected:

		UID* uid_;
	};

};

#endif
