#ifndef UNIQUEIDGENI_H_
#define UNIQUEIDGENI_H_

#include "UniqueIDGenS.h"
#include <functional>
#include <colibry/Bag.h>

namespace UIDGen {

    class  UniqueIDGenImpl : public virtual POA_UIDGen::UniqueIDGen {
    public:

		UniqueIDGenImpl (const std::function<void()>& notify_shutdown);

		virtual ::UIDGen::ID_t getuid ();
		virtual void putback (::UIDGen::ID_t id);
		virtual void reset();
        virtual void shutdown();

    protected:

		colibry::Bag<ID_t> bag_;
		const std::function<void()> notify_shutdown_;
    };

}; // namespace

#endif /* UNIQUEIDGENI_H_  */
