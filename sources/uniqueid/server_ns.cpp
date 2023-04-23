//
// Unique ID generator server
// Copyright (C) 2019-21 by LAPLJ.
// All rights reserved.
//

#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cassert>
#include <span>
#include <csignal>
#include <functional>
#include <colibry/NameServer.h>
#include <colibry/ORBManager.h>
#include <colibry/TextTools.h>
#include <sys/socket.h>
#include "UniqueIDGenI.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

void help(char* prog);

class Interrupted {};

void signal_handler(int ns)
{
	throw Interrupted{};
}

int main(int argc, char* argv[])
{
	span args(argv,argc);

	try {

		cout << "* UniqueID Generator" << endl;

		const char* out_iorfile = nullptr;
		const char* ns_name = "uid";			// default name

		bool get_iorfile = false;
		bool get_nsname = false;
		for ( const char* a : args.last(argc-1)) {
			if (a == "-h"s)				// usage info
				help(args[0]);
			else if (a == "-o"s)
				get_iorfile = true;
			else if (a == "-n"s)
				get_nsname = true;
			else if (get_iorfile) {
				out_iorfile = a;
				get_iorfile = false;
			} else if (get_nsname) {
				ns_name = a;
				get_nsname = false;
			}
		}
		assert(ns_name != nullptr);

		ORBManager orbm{argc, argv};
		orbm.activate_rootpoa();

		// Create the servant
		cout << "* Creating servant..." << flush;
		UniqueIDGenImpl uidgen_i;
		cout << "OK" << endl;

		auto uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i);

		// Register object in the NS
		try {
			cout << "* Registering in the NS (\"" << ns_name << "\")..." << flush;
			NameServer ns{orbm};
			ns.bind(ns_name,uidgen.in());
			cout << "OK" << endl;
		} catch (CosNaming::NamingContext::AlreadyBound&) {
			cerr << "NAME ALREADY BOUND!" << endl;
			if (out_iorfile == nullptr)
				throw;
		} catch (CORBA::ORB::InvalidName&) {
			cerr << " NS is not running." << endl;
			throw;
		} catch (const CORBA::Exception&) {
			cerr << "CORBA exception while connecting to NS" << endl;
			throw;
		}

		if (out_iorfile != nullptr) {
			// Generating + saving IOR
			cout << "* Saving reference (\"" << out_iorfile << "\")..." << flush;
			orbm.save_ior(out_iorfile, uidgen.in());
			cout << "OK" << endl;
		}

		cout << "* Running event loop." << endl;

		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		try {
			orbm.run();
		} catch (const Interrupted&) {
			cout << cursor_back() << "* Interrupted" << endl;
		}

		cout << "* Unbinding \"" << ns_name << "\"..." << flush;
		NameServer{orbm}.unbind(ns_name);
		cout << "OK" << endl;
		orbm.shutdown();

		if (out_iorfile != nullptr) {
			cout << "* Removing file..." << flush;
			unlink(out_iorfile); // remove ior file
			cout << "OK" << endl;
		}

	} catch (const CORBA::SystemException &e) {
		cerr << "CORBA exception: " << e << endl;
	}
}

void help(char* prog)
{
	cerr << "USAGE: " << prog << " -n <name> [-o <ior_file>] [-h]" << endl;
	exit(1);
}
