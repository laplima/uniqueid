#ifndef UNIQUEIDGENI_H_
#define UNIQUEIDGENI_H_

#include "UniqueIDGenS.h"
#include <colibry/Bag.h>
#include <colibry/ORBManager.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace UIDGen {

    class  UniqueIDGenImpl : public virtual POA_UIDGen::UniqueIDGen {
    public:

		UniqueIDGenImpl (colibry::ORBManager& om);	// do not own om
		virtual ~UniqueIDGenImpl ();

		virtual ::UIDGen::ID_t getuid ();
		virtual void putback (::UIDGen::ID_t id);
		virtual void reset();
        virtual void shutdown();

    protected:

		colibry::Bag<ID_t> m_bag;
		colibry::ORBManager& om_;
    };

}; // namespace


#endif /* UNIQUEIDGENI_H_  */
