#include "UniqueIDGenImpl.h"
#include <colibry/ORBManager.h>
#include <iostream>

using namespace std;
using namespace colibry;
using namespace UIDGen;

UniqueIDGenImpl::UniqueIDGenImpl (ORBManager& om) : om_{om}, m_bag(1,10000)
{
}

UniqueIDGenImpl::~UniqueIDGenImpl (void)
{
}

::UIDGen::ID_t UniqueIDGenImpl::getuid ()
{
    ID_t id;
    try {
		id = m_bag.get();
    } catch (const underflow_error&) {
		cerr << "No more IDs left" << endl;
    }
    return id;
}

void UniqueIDGenImpl::putback (::UIDGen::ID_t id)
{
    try {
		m_bag.put_back(id);
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
    om_.shutdown();
}
