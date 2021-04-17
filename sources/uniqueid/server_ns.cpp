//
// Unique ID generator server
// Copyright (C) 2019-21 by LAPLJ.
// All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cassert>
#include <thread>
#include <colibry/NameServer.h>
#include <colibry/ORBManager.h>
#include "UniqueIDGenImpl.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

void help(char* prog);

int main(int argc, char* argv[])
{
	try {

		cout << "UniqueID Generator" << endl;

		const char* out_iorfile = nullptr;
		const char* ns_name = "uid";			// default name

		for (int i=1; i<argc; i++) {
			string arg{argv[i]};
			if (arg == "-o")
				out_iorfile = argv[++i];
			else if (arg == "-n")
				ns_name = argv[++i];
			else if (arg == "-h")				// usage info
				help(argv[0]);
		}
		assert(ns_name != nullptr);

		ORBManager orbm{argc, argv};
		orbm.activate_rootpoa();

		// Create the servant
		cout << "* Creating servant..." << flush;
		UniqueIDGenImpl uidgen_i;
		cout << "OK" << endl;

		UniqueIDGen_var uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i);

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
		thread t{[&orbm]() { orbm.run(); }};

		cout << "  Type <ENTER> to quit" << endl;
		cin.get();

		// Clean-up IOR publications
		cout << "\tUbinding name \"" << ns_name << "\"..." << flush;
		NameServer{orbm}.unbind(ns_name);
		cout << "OK" << endl;

		if (out_iorfile != nullptr) {
			cout << "\tRemoving file..." << flush;
			unlink(out_iorfile); // remove ior file
			cout << "OK" << endl;
		}

		cout << "* Shutting down orb... " << flush;
		orbm.shutdown();
		t.join();
		cout << "OK" << endl;

	} catch (const CORBA::SystemException &e) {
		cerr << "CORBA exception: " << e << endl;
	}
}

void help(char* prog)
{
	cerr << "USAGE: " << prog << " -n <name> [-o <ior_file>] [-h]" << endl;
	exit(1);
}
