#include "UniqueIDGenI.h"
#include "UniqueIDGenC.h"
#include <algorithm>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
#include <colibry/Logger.h>
#include <string_view>
#include <ranges>

using namespace std;
using namespace colibry;
using namespace UIDGen;

constexpr ID_t maxid = 10'000'000'000;
constexpr int uslen = 16;				// unique string length

UniqueIDGenImpl::UniqueIDGenImpl () : bag_{1, maxid}, ss_{uslen, '0', '9'}
{
}

::UIDGen::ID_t UniqueIDGenImpl::getuid ()
{
	spdlog::debug("get: {}", fmt::streamed(bag_));

	ID_t id{};
	try {
		id = bag_.get();
	} catch (const underflow_error&) {
		spdlog::error("No more IDs left");
	}
	return id;
}

void UniqueIDGenImpl::putback (::UIDGen::ID_t id)
{
	try {
		bag_.put_back(id);
		spdlog::debug("putback: {}", fmt::streamed(bag_));
   } catch (...) {
		spdlog::error("Put back id failed.");
		throw InvalidID{};
	}
}

char *UniqueIDGenImpl::getustr()
{
	auto ss = CORBA::string_dup(static_cast<string>(ss_).c_str());
	ss_.next();
	return ss;
}

bool UniqueIDGenImpl::reset(const char* passwd)
{
	if (string_view{passwd} == "uid"s) {
		bag_.reset();
		ss_.reset();
		spdlog::debug("reset: {}", fmt::streamed(bag_));
		return true;
	}
	return false;
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
	return ranges::all_of(ss_, [this](char c) { return (c == last_); });
	// for (const char& c : ss_)
	// 	if (c != last_)
	// 		return false;
	// return true;
}

std::string SeqString::next()
{
	for (auto i=ss_.size()-1; i>=0; --i) {
		++ss_[i];
		if (ss_[i] <= last_)
			return ss_;
		ss_[i] = first_;
	}
	if (is_last())
		reset();
	return ss_;
}
