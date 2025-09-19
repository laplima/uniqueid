#include "UID.h"
// #include <print>
#include <spdlog/spdlog.h>
#include "../printable.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

UIDGen::ID_t UID::getuid ()
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

void UID::putback (UIDGen::ID_t id)
{
	try {
		bag_.put_back(id);
		spdlog::debug("putback: {}", streamed(bag_));
   } catch (...) {
		spdlog::error("Put back id failed.");
		throw InvalidID{};
	}
}

std::string UID::getustr()
{
	auto ss = static_cast<string>(hseq_);
	hseq_.next();
	return ss;
}

bool UID::reset(const std::string& passwd)
{
	if (string_view{passwd} == "uid"s) {
		bag_.reset();
		hseq_.reset();
		spdlog::debug("reset: {}", streamed(bag_));
		return true;
	}
	return false;
}
