#include <iostream>
#include <UniqueIDGenC.h>
#include <colibry/ORBManager.h>
#include <colibry/NameServer.h>
#include <string>
#include <regex>
#include <vector>

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
			cerr << "USAGE: " << argv[0] << " [-n <NAME> | -f <IORFILE>" << endl;
			return 1;
		}

		UIDGen::UniqueIDGen_var uid = UIDGen::UniqueIDGen::_nil();

		if (ns_name != nullptr)
			uid = NameServer::Instance()->resolven<UIDGen::UniqueIDGen>(ns_name);
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

		string cmd;
		cout << "> ";
		getline(cin, cmd);
		while (!cmd.empty()) {
			auto tks = split(cmd);

			if (tks[0] == "g")
				cout << "\t" << uid->getuid() << endl;
			else if (tks[0] == "p") {
				if (tks.size() < 2)
					cerr << "\tmissing id" << endl;
				else {
					try {
						UIDGen::ID_t v = stoi(tks[1]);
						uid->putback(v);
						cout << "\t" << v << " put back" << endl;
					} catch (const UIDGen::InvalidID&) {
						cerr << "\tInvalid ID" << endl;
					}
				}
			} else if (tks[0] == "s") {
				uid->shutdown();
				break;
			}

			cout << "> ";
			getline(cin, cmd);
		}

		cout << "Terminating" << endl;

	} catch (CORBA::Exception& e) {
		cerr << "CORBA exception: " << e << endl;
	}
}
