#include "UniqueIDGenI.h"
//#include "UniqueIDGenC.h"

#include "UID.h"

using namespace std;
using namespace UIDGen;

ID_t UniqueIDGen_i::getuid ()
{
	return uid_->getuid();
}

void UniqueIDGen_i::putback (ID_t id)
{
	uid_->putback(id);
}

char *UniqueIDGen_i::getustr()
{
	auto s = uid_->getustr();
	return CORBA::string_dup(s.c_str());
}

bool UniqueIDGen_i::reset(const char* passwd)
{
	return uid_->reset(passwd);
}
