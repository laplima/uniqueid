#include <iostream>
#include <UniqueIDGenC.h>
#include <colibry/ORBManager.h>
#include <colibry/NameServer.h>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

using namespace std;
using namespace colibry;

vector<string> split(const string& input)
{
	regex re{"\\s+"};
	sregex_token_iterator
		first{input.begin(), input.end(), re, -1}, // -1 performns splitting
		last;
	return {first, last};
}

class IDBag {
protected:
	std::vector<UIDGen::ID_t> ids;
public:
	void add(UIDGen::ID_t id) { ids.push_back(id); }
	void remove(UIDGen::ID_t id) {
		auto p = find(ids.begin(), ids.end(), id);
		if (p != ids.end())
			ids.erase(p);
	}
	void clear() { ids.clear(); }
	auto begin() { return ids.begin(); }
	auto end() { return ids.end(); }
	friend ostream& operator<<(ostream& os, const IDBag& idg) {
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
};

void return_ids(IDBag& idg, UIDGen::UniqueIDGen_ptr uid)
{
	std::for_each(idg.begin(), idg.end(), [&uid](auto& id) {
		uid->putback(id);
	});
	idg.clear();
}

int main(int argc, char* argv[])
{
	const char* DFLT_NAME = "uid";

	try {

		cout << "UniqueID Test client" << endl;
		cout << "(C) laplima" << endl;

		ORBManager orbm{argc, argv};

		const char* ns_name = nullptr;
		const char* iorfile = nullptr;

		for (int i=1; i<argc; ++i) {
			string arg{argv[i]};
			if (arg == "-n")
				ns_name = argv[++i];
			else if (arg == "-f")
				iorfile = argv[++i];
		}

		if (ns_name == nullptr && iorfile == nullptr) {
			cerr << "USAGE: " << argv[0] << " [-n <NAME> | -f <IORFILE>]" << endl;
			cout << "Using default name: \"" << DFLT_NAME << "\"" << endl;
			ns_name = DFLT_NAME;
		}

		UIDGen::UniqueIDGen_var uid = UIDGen::UniqueIDGen::_nil();

		if (ns_name != nullptr) {
			NameServer ns{orbm};
			uid = ns.resolve<UIDGen::UniqueIDGen>(ns_name);
		} else
			uid = orbm.string_to_object<UIDGen::UniqueIDGen>(string{"file://"} + iorfile);

		if (CORBA::is_nil(uid.ptr())) {
			cerr << "CORBA object is nil" << endl;
			return 2;
		}

		cout << "g = get" << endl
			 << "p <N> = put back" << endl
			 << "s = shutdown" << endl
			 << "r = reset" << endl
			 << "u = getustr" << endl
			 << "<ENTER> = exit + return ids" << endl
			 << "x = exit" << endl << endl;

		IDBag idg;
		bool returnids = true;
		string cmd;
		cout << "> ";
		getline(cin, cmd);
		while (!cmd.empty()) {
			auto tks = split(cmd);

			if (tks[0] == "g") {
				auto v = uid->getuid();
				cout << "    => " << v << endl;
				idg.add(v);
				cout << "    " << idg << endl;
			} else if (tks[0] == "p") {
				if (tks.size() < 2)
					cerr << "    missing id" << endl;
				else {
					try {
						UIDGen::ID_t v = stoi(tks[1]);
						uid->putback(v);
						cout << "    " << v << " =>" << endl;
						idg.remove(v);
					} catch (const UIDGen::InvalidID&) {
						cerr << "    Invalid ID" << endl;
					}
					cout << "    " << idg << endl;
				}
			} else if (tks[0] == "r") {
				uid->reset();
				idg.clear();
				cout << "    uid reset" << endl;
			} else if (tks[0] == "u") {
				string us = uid->getustr();
				cout << "   -> " << us << endl;
			} else if (tks[0] == "s")
				break;
			else if (tks[0] == "x") {
				returnids = false;
				break;
			} else
				cout << "   unknown command" << endl;

			cout << "> ";
			getline(cin, cmd);
		}

		if (returnids) {
			// return ids (before shutting down)
			cout << "returning ids" << endl;
			return_ids(idg,uid);
		}

		if (cmd == "s") {
			// shutdown
			uid->shutdown();
		}

		cout << "Terminating" << flush;
		cout << endl;

	} catch (CORBA::Exception& e) {
		cerr << "CORBA exception: " << e << endl;
	}
}
