#ifndef UNIQUEIDGENI_H_
#define UNIQUEIDGENI_H_

#include "UniqueIDGenS.h"
#include <functional>
#include <colibry/Bag.h>
#include <string>

// ------------------------------------------------------

class SeqString {
public:
    SeqString(std::size_t sz, char first_char, char last_char);
    void reset();
    bool is_last();
    std::string next();
    operator std::string() const { return ss_; }
private:
    std::string ss_;
    char first_, last_;
};

// ------------------------------------------------------

namespace UIDGen {

    class  UniqueIDGenImpl : public virtual POA_UIDGen::UniqueIDGen {
    public:

		UniqueIDGenImpl(const std::function<void()>& notify_shutdown);

		virtual ::UIDGen::ID_t getuid ();
		virtual void putback (::UIDGen::ID_t id);
		virtual char *getustr();
		virtual void reset();
        virtual void shutdown();

    protected:

		colibry::Bag<ID_t> bag_;
		const std::function<void()> notify_shutdown_;
		SeqString ss_;
    };

};

#endif /* UNIQUEIDGENI_H_  */
