//
// Unique ID generator server
// Copyright (C) 2019-21 by LAPLJ.
// All rights reserved.
//

#include <exception>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
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
	// enable debug level messages
	// spdlog::set_level(spdlog::level::debug);

	span args(argv,argc);

	try {

		fmt::println("* UniqueID Generator");

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
		fmt::print("* Creating servant...");
		fflush(stdout);
		UniqueIDGenImpl uidgen_i;
		fmt::println("OK");

		auto uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i);

		// Register object in the NS
		try {
			fmt::print("* Registering in the NS (\"{}\")...", ns_name);
			fflush(stdout);
			NameServer ns{orbm};
			ns.bind(ns_name,uidgen.in());
			fmt::println("OK");
		} catch (CosNaming::NamingContext::AlreadyBound&) {
			spdlog::error("NAME ALREADY BOUND!\n");
			if (out_iorfile == nullptr)
				throw;
		} catch (CORBA::ORB::InvalidName&) {
			spdlog::error(" NS is not running.\n");
			throw;
		} catch (const CORBA::Exception&) {
			spdlog::error( "CORBA exception while connecting to NS\n");
			throw;
		}

		if (out_iorfile != nullptr) {
			// Generating + saving IOR
			fmt::println("* Saving reference (\"{}\")...", out_iorfile);
			fflush(stdout);
			orbm.save_ior(out_iorfile, uidgen.in());
			fmt::println("OK");
		}

		fmt::println("* Running event loop.");

		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		try {
			orbm.run();
		} catch (const Interrupted&) {
			fmt::print("{}", cursor_back());
			spdlog::info("Interrupted");
		}

		fmt::print("* Unbinding \"{}\"...", ns_name);
		fflush(stdout);
		NameServer{orbm}.unbind(ns_name);
		fmt::println("OK");
		orbm.shutdown();

		if (out_iorfile != nullptr) {
			fmt::println("* Removing file...");
			fflush(stdout);
			unlink(out_iorfile); // remove ior file
			fmt::println("OK");
		}

	} catch (const CORBA::SystemException &e) {
		spdlog::error("CORBA exception: {}", fmt::streamed(e));
	}
}

void help(char* prog)
{
	spdlog::error( "USAGE: {} -n <name> [-o <ior_file>] [-h]\n", prog);
	exit(1);
}
