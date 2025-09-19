#ifndef SEQSTRING_H
#define SEQSTRING_H

#include <string>

class SeqString {
public:
	SeqString(std::size_t sz, char first_char, char last_char);
	virtual ~SeqString() = default;

	virtual SeqString& operator++();	// may be overriden in subclasses

	void reset();	// start over
	[[nodiscard]] bool is_last();
	std::string next() { ++(*this); return ss_; }
	operator std::string() const { return ss_; }
protected:
	std::string ss_;
	char first_, last_;
};

#endif
