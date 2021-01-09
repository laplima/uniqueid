//
// Unique ID generator server
// Copyright (C) 2019-20 by LAPLJ.
// All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <colibry/NameServer.h>
#include <colibry/ORBManager.h>
#include "UniqueIDGenImpl.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

void usage(char* prog);

int main(int argc, char* argv[])
{
   	ORBManager orbm{argc, argv};

    try {

    	cout << "UniqueID Generator" << endl;

		const char* out_iorfile = nullptr;
		const char* ns_name = "uid";			// default name

		for (int i=1; i<argc; i++)
		    if (string(argv[i]) == "-o")
				out_iorfile = argv[++i];
		    else if (string(argv[i]) == "-n")
		    	ns_name = argv[++i];
		    else if (string{argv[i]} == "-i")
		    	usage(argv[0]);

    	orbm.activate_rootpoa();

		// Create the servant
		cout << "* Creating servant..." << flush;
		UniqueIDGenImpl uidgen_i([&orbm,ns_name]() {
			cout << "\tunbinding name \"" << ns_name << "\"..." << flush;
			NameServer{orbm}.unbind(ns_name);
		    cout << "OK" << endl;
		    orbm.shutdown();
		});
		cout << "OK" << endl;

		UniqueIDGen_var uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i);

		// Register in the NS
		try {
		    cout << "* Registering in the NS (\"" << ns_name << "\")..." << flush;
	    	NameServer ns{orbm};
		    ns.bind(ns_name,uidgen.in());
		    cout << "OK" << endl;
		} catch (CosNaming::NamingContext::AlreadyBound&) {
		    cerr << "NAME ALREADY BOUND!" << endl;
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
		orbm.run ();

	    cout << "* Shutting down orb..." << endl;
	    if (out_iorfile != nullptr) {
		    cout << "\tremoving file..." << flush;
		    unlink(out_iorfile); // remove ior file
		    cout << "OK" << endl;
	    }

    } catch (const CORBA::SystemException &e) {
	    cerr << "CORBA exception: " << e << endl;
    }
}

void usage(char* prog)
{
    cerr << "USAGE: " << prog << " -n <name> [-o <ior_file>]" << endl;
    exit(1);
}

char *dupstr(const char* orig)
{
	char* s = new char[strlen(orig)+1];
	strcpy(s,orig);
	return s;
}

