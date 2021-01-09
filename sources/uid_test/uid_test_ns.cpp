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

int main(int argc, char* argv[])
{
	const char* DFLT_NAME = "uid";

	try {

		cout << "UniqueID client" << endl;

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
		}
		else
			uid = orbm.string_to_object<UIDGen::UniqueIDGen>(string{"file://"} + iorfile);

		if (CORBA::is_nil(uid.ptr())) {
			cerr << "CORBA object is nil" << endl;
			return 2;
		}

		cout << "g = get" << endl
			 << "p <N> = put back" << endl
			 << "s = shutdown" << endl
			 << "<ENTER> = exit" << endl << endl;

		vector<UIDGen::ID_t> ids;
		string cmd;
		cout << "> ";
		getline(cin, cmd);
		while (!cmd.empty()) {
			auto tks = split(cmd);

			if (tks[0] == "g") {
				auto v = uid->getuid();
				cout << "\t" << v << endl;
				ids.push_back(v);
			} else if (tks[0] == "p") {
				if (tks.size() < 2)
					cerr << "\tmissing id" << endl;
				else {
					try {
						UIDGen::ID_t v = stoi(tks[1]);
						uid->putback(v);
						cout << "\t" << v << " put back" << endl;
						ids.erase(find(ids.begin(), ids.end(), v));
					} catch (const UIDGen::InvalidID&) {
						cerr << "\tInvalid ID" << endl;
					}
				}
			} else if (tks[0] == "s")
				break;

			cout << "> ";
			getline(cin, cmd);
		}

		// return ids (before shutting down)
		cout << "returning: ";
		bool first = true;
		for (auto v : ids) {
			uid->putback(v);
			if (first) {
				cout << v;
				first = false;
			} else
				cout << ", " << v;
		}
		cout << endl;

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
