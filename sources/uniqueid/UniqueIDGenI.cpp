#include "UniqueIDGenI.h"
#include "UniqueIDGenC.h"
#include <algorithm>
#include <cstddef>
#include <print>
#include <spdlog/spdlog.h>
// #include <colibry/Logger.h>
#include <string_view>
#include <ranges>
#include "../printable.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

constexpr ID_t maxid = 10'000'000'000;
constexpr int uslen = 16;				// unique string length

UniqueIDGenImpl::UniqueIDGenImpl () : bag_{1, maxid}, hseq_{uslen}
{
}

::UIDGen::ID_t UniqueIDGenImpl::getuid ()
{
	spdlog::debug("get: {}", streamed(bag_));

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
		spdlog::debug("putback: {}", streamed(bag_));
   } catch (...) {
		spdlog::error("Put back id failed.");
		throw InvalidID{};
	}
}

char *UniqueIDGenImpl::getustr()
{
	auto ss = CORBA::string_dup(static_cast<string>(hseq_).c_str());
	hseq_.next();
	return ss;
}

bool UniqueIDGenImpl::reset(const char* passwd)
{
	if (string_view{passwd} == "uid"s) {
		bag_.reset();
		hseq_.reset();
		spdlog::debug("reset: {}", streamed(bag_));
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

// ------------------------------------------------------

HexSeq::HexSeq(std::size_t sz)
	: SeqString(sz, '0', 'F')
{
}

std::string HexSeq::next()
{
	for (auto i=ss_.size()-1; i>=0; --i) {
		if (ss_[i] == '9')
			ss_[i] = 'A';
		else
			++ss_[i];
		if (ss_[i] <= last_)
			return ss_;
		ss_[i] = first_;
	}
	if (is_last())
		reset();
	return ss_;
}
