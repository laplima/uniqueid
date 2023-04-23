#include "UniqueIDGenI.h"
#include <iostream>
#include <colibry/Logger.h>

using namespace std;
using namespace colibry;
using namespace UIDGen;

UniqueIDGenImpl::UniqueIDGenImpl () : bag_(1,10000), ss_{10, '0', '9'}
{
}

::UIDGen::ID_t UniqueIDGenImpl::getuid ()
{
#if DEBUG
	LOG(INFO) << "get: " << bag_ << endl;
#endif

	ID_t id{};
	try {
		id = bag_.get();
	} catch (const underflow_error&) {
		LOG(ERROR) << "No more IDs left";
	}
	return id;
}

void UniqueIDGenImpl::putback (::UIDGen::ID_t id)
{
	try {
		bag_.put_back(id);
#if DEBUG
		LOG(INFO) << "putback: " << bag_;
#endif
   } catch (...) {
		LOG(ERROR) << "Put back id failed.";
		throw InvalidID{};
	}
}

char *UniqueIDGenImpl::getustr()
{
	auto ss = CORBA::string_dup(static_cast<string>(ss_).c_str());
	ss_.next();
	return ss;
}

void UniqueIDGenImpl::reset()
{
	bag_.reset();
	ss_.reset();
#if DEBUG
	LOG(INFO) << "reset: " << bag_;
#endif
}

// ------------------------------------------------------

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
	for (const char& c : ss_)
		if (c != last_)
			return false;
	return true;
}

std::string SeqString::next()
{
	for (auto i=ss_.size()-1; i>=0; --i) {
		++ss_[i];
		if (ss_[i] <= last_)
			return ss_;
		else
			ss_[i] = first_;
	}
	if (is_last())
		reset();
	return ss_;
}
