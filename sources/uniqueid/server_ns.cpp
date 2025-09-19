//
// Unique ID generator server
// Copyright (C) 2019-21 by LAPLJ.
// All rights reserved.
//

// #include <exception>
#include <print>
#include <format>
#include <iostream>
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
#include "../printable.h"
#include "UID.h"

using namespace std;
using namespace colibry;
using namespace UIDGen;

template <typename... T>
void fprint(std::format_string<T...> fmt, T&&... args)
{
	print(fmt, std::forward<T>(args)...);
	fflush(stdout);
}

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

		println("* UniqueID Generator");

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
		fprint("* Creating servant...");
		UID uidgen_i;
		println("OK");

		auto uidgen = orbm.activate_object<UniqueIDGen>(uidgen_i.in());

		// Register object in the NS
		try {
			fprint("* Registering in the NS (\"{}\")...", ns_name);
			NameServer ns{orbm};
			ns.bind(ns_name,uidgen.in());
			println("OK");
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
			fprint("* Saving reference (\"{}\")...\n", out_iorfile);
			orbm.save_ior(out_iorfile, uidgen.in());
			println("OK");
		}

		println("* Running event loop.");

		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		try {
			orbm.run();
		} catch (const Interrupted&) {
			print("{}", cursor_back());
			spdlog::info("Interrupted");
		}

		print("* Unbinding \"{}\"...", ns_name);
		fflush(stdout);
		NameServer{orbm}.unbind(ns_name);
		println("OK");
		orbm.shutdown();

		if (out_iorfile != nullptr) {
			println("* Removing file...");
			fflush(stdout);
			unlink(out_iorfile); // remove ior file
			println("OK");
		}

	} catch (const CORBA::SystemException &e) {
		spdlog::error("CORBA exception: {}", streamed(e));
	}
}

void help(char* prog)
{
	spdlog::error( "USAGE: {} -n <name> [-o <ior_file>] [-h]\n", prog);
	exit(1);
}
