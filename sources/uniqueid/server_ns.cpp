//
// Unique ID generator server
// Copyright (C) 2019 by LAPLJ.
// All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <fstream>
#include <colibry/NameServer.h>
#include <colibry/ORBManager.h>
#include "UniqueIDGenImpl.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

NameServer* gNS = nullptr;

char* out_iorfile = nullptr;
char* ns_name = nullptr;

void termination_handler(int nsig);
void usage(char* prog);

int main(int argc, char* argv[])
{
    try {

    	cout << "UniqueID Generator" << endl;

    	ORBManager orbm{argc, argv};

		for (int i=1; i<argc; i++)
		    if (string(argv[i]) == "-o")
				out_iorfile = argv[++i];
		    else if (string(argv[i]) == "-n")
		    	ns_name = argv[++i];

		if (ns_name == nullptr)
		    usage(argv[0]);

		signal(SIGINT,termination_handler);
		signal(SIGTERM,termination_handler);

    	orbm.activate_rootpoa();

		// Create the servant
		cout << "* Creating servant..." << flush;
		UniqueIDGenImpl uidgen_i;
		cout << "OK" << endl;

		UniqueIDGen_var uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i);

		// Register in the NS
		// - get naming context
		try {
		    cout << "* Registering in the NS (\"" << ns_name << "\")..." << flush;
		    gNS = NameServer::Instance();
		    gNS->bind(ns_name,uidgen.in());
		    cout << "OK" << endl;
		} catch (CosNaming::NamingContext::AlreadyBound&) {
		    cerr << "NAME ALREADY BOUND!" << endl;
		    throw;
		} catch (CORBA::ORB::InvalidName&) {
		    cerr << " NS is not running." << endl;
		} catch (const CORBA::Exception&) {
			cerr << "CORBA exception while connecting to NS" << endl;
		}

		if (out_iorfile != nullptr) {
		    // Generating + saving IOR
		    cout << "* Saving reference (\"" << out_iorfile << "\")..."
		    	 << flush;
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
	    if (gNS != nullptr) {
			cout << "unbinding name..." << flush;
	        gNS->unbind(ns_name);
		    cout << "OK" << endl;
	    }

    } catch (CORBA::SystemException &) {
	    cerr << "CORBA exception." << endl;
    }
}


void termination_handler(int nsig)
{
    ORBManager::global->shutdown();
}

void usage(char* prog)
{
    cerr << "USAGE: " << prog << " -n <name> [-o <ior_file>]" << endl;
    exit(1);
}
