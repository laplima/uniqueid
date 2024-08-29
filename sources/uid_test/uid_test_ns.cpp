#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <UniqueIDGenC.h>
#include <colibry/ORBManager.h>
#include <colibry/NameServer.h>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include <span>

using namespace std;
using namespace colibry;

class IDBag {
protected:
	std::vector<UIDGen::ID_t> ids;
public:
	void add(UIDGen::ID_t id) { ids.push_back(id); }
	void remove(UIDGen::ID_t id);
	void clear() { ids.clear(); }
	auto begin() { return ids.begin(); }
	auto end() { return ids.end(); }
	bool empty() const { return ids.empty(); }
	friend ostream& operator<<(ostream& os, const IDBag& idg);
};

class App {
public:
	enum class State { STARTING, RUNNING, QUITTING };
public:
	App(UIDGen::UniqueIDGen_ptr uid) : uid_{uid} {}
	void help();
	void run();
	[[nodiscard]] State state() const { return state_; }
private:
	UIDGen::UniqueIDGen_ptr uid_;
	State state_ = State::STARTING;
	IDBag idg_;
protected:
	void execute();
	void return_ids();
	static string input(const string& prompt);
};

// helpers

UIDGen::UniqueIDGen_ptr getref(const char* nsname, const char* iorfile);
vector<string> split(const string& input);

// ------------------------------------------------------------------

int main(int argc, char* argv[])
{
	const char* DFLT_NAME = "uid";

	try {

		fmt::println("* UniqueID Test client");

		ORBManager orbm{argc, argv};

		span args(argv,argc);

		const char* ns_name = nullptr;
		const char* iorfile = nullptr;

		for (int i=1; i<args.size(); ++i) {
			string arg{args[i]};
			if (arg == "-n")
				ns_name = args[++i];
			else if (arg == "-f")
				iorfile = args[++i];
		}

		if (ns_name == nullptr && iorfile == nullptr) {
			fmt::print(stderr,"\nUSAGE: {} [-n <NAME> | -f <IORFILE>]\n\n", args[0]);
			fmt::println("Using default name: \"{}\"", DFLT_NAME);
			ns_name = DFLT_NAME;
		}

		// get reference

		UIDGen::UniqueIDGen_var uid = UIDGen::UniqueIDGen::_nil();

		if (ns_name != nullptr) {
			NameServer ns{orbm};
			uid = ns.resolve<UIDGen::UniqueIDGen>(ns_name);
		} else
			uid = orbm.string_to_object<UIDGen::UniqueIDGen>(string{"file://"} + iorfile);

		if (CORBA::is_nil(uid.ptr())) {
			spdlog::error("CORBA object is nil");
			return 2;
		}

		// run application

		App app{uid.in()};
		app.help();
		app.run();

		fmt::println("Terminating");
		fflush(stdout);
		fmt::print("\n");

	} catch (CORBA::Exception& e) {
		spdlog::error("CORBA exception: {}", fmt::streamed(e));
	}
}

//
// APP
//

void App::help()
{
	fmt::print("\n");
	fmt::println(R"(g = get
p <N> = put back
u = getustr
r = reset
h = help
x = exit + return ids
<ENTER> = exit)");
	fmt::print("\n");
}

string App::input(const string& prompt)
{
	string rd;
	fmt::print("{}", prompt);
	getline(cin, rd);
	return rd;
}

void App::run()
{
	state_ = State::RUNNING;
	while (state() != State::QUITTING)
		execute();
}

void App::execute()
{
	string cmd = input("> ");

	if (cmd.empty()) {
		state_ = State::QUITTING;
		return;
	}

	auto tks = split(cmd);
	switch (tks[0][0]) {
		case 'h':
			help();
			break;
		case 'g': {
			auto v = uid_->getuid();
			fmt::println("    => [{}]", v);
			idg_.add(v);
			fmt::println("    {}", fmt::streamed(idg_)); }
			break;
		case 'p':
			if (tks.size() < 2)
				fmt::println(stderr, "    missing id");
			else {
				try {
					UIDGen::ID_t v = stoi(tks[1]);
					uid_->putback(v);
					fmt::println("    {} =>", v);
					idg_.remove(v);
				} catch (const UIDGen::InvalidID&) {
					fmt::println(stderr, "    Invalid ID");
				}
				fmt::println("    {}", fmt::streamed(idg_));
			}
			break;
		case 'r':
			if (tks.size() <2)
				fmt::println(stderr, "    missing password");
			else {
				if (uid_->reset(tks[1].c_str())) {
					idg_.clear();
					fmt::println("    uid reset");
				} else
				fmt::println(stderr, "    wrong password");
			}
			break;
		case 'u': {
			string us = uid_->getustr();
			fmt::println("   => \"{}\"", us); }
			break;
		case 'x':
			if (!idg_.empty()) {
				fmt::print("\tReturning ids: ");
				return_ids();
			}
			state_ = State::QUITTING;
			break;
		default:
			fmt::println(stderr, "   unknown command");
	}
}

void App::return_ids()
{
	std::for_each(idg_.begin(), idg_.end(), [this](auto& id) {
		fmt::print("{} ", id);
		this->uid_->putback(id);
	});
	fmt::print("\n");
	idg_.clear();
}

// helpers

vector<string> split(const string& input)
{
	regex re{"\\s+"};
	sregex_token_iterator
		first{input.begin(), input.end(), re, -1}, // -1 performns splitting
		last;
	return {first, last};
}

// IDBag

void IDBag::remove(UIDGen::ID_t id)
{
	auto p = find(ids.begin(), ids.end(), id);
	if (p != ids.end())
		ids.erase(p);
}

ostream& operator<<(ostream& os, const IDBag& idg)
{
	bool first = true;
	os << '[';
	for (const auto& i : idg.ids) {
		if (first) {
			first = false;
			os << i;
		} else
			os << ", " << i;
	}
	return (os << ']');
}
