#include "UniqueIDGenImpl.h"
#include <iostream>

using namespace std;
using namespace colibry;
using namespace UIDGen;

UniqueIDGenImpl::UniqueIDGenImpl (const function<void()>& notify_shutdown)
    : bag_(1,10000), notify_shutdown_(notify_shutdown)
{
}

::UIDGen::ID_t UniqueIDGenImpl::getuid ()
{
    ID_t id;
    try {
		id = bag_.get();
    } catch (const underflow_error&) {
		cerr << "No more IDs left" << endl;
    }
    return id;
}

void UniqueIDGenImpl::putback (::UIDGen::ID_t id)
{
    try {
		bag_.put_back(id);
    } catch (...) {
		cerr << "Failed putting back id." << endl;
        throw InvalidID{};
    }
}

void UniqueIDGenImpl::reset()
{
    // todo
}

void UniqueIDGenImpl::shutdown()
{
    notify_shutdown_();
}
