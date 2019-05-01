
#include <bits/ensure.h>
#include <mlibc/charset.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

bool charset::is_ascii_superset() {
	// TODO: For locales that change the meaning of ASCII chars, this needs to be changed.
	return true;
}

bool charset::is_alpha(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_alpha() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_digit(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c >= '0' && c <= '9';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_digit() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_xdigit(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_xdigit() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_alnum(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_alnum() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_punct(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c == '!' || c == '"' || c == '#' || c == '$' || c == '%' || c == '&'
				|| c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ','
				|| c == '-' || c == '.' || c == '/'
				|| c == ':' || c == ';' || c == '<' || c == '=' || c == '>' || c == '?'
				|| c == '@'
				|| c == '[' || c == '\\' || c == ']' || c == '^' || c == '_' || c == '`'
				|| c == '{' || c == '|' || c == '}' || c == '~';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_punct() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_graph(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c >= 0x21 && c <= 0x7E;
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_graph() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_blank(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c == ' ' || c == '\t';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_blank() is not implemented"
				" for the full Unicode charset " << c << frg::endlog;
	return false;
}

bool charset::is_space(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_space() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_print(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return c >= 0x20 && c <= 0x7E;
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_print() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_lower(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return (c >= 'a' && c <= 'z');
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_print() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

bool charset::is_upper(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		return (c >= 'A' && c <= 'Z');
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::is_print() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return false;
}

codepoint charset::to_lower(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		if(c >= 'A' && c <= 'Z')
			return c - 'A' + 'a';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::to_lower() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return c;
}

codepoint charset::to_upper(codepoint c) {
	if(c <= 0x7F && is_ascii_superset())
		if(c >= 'a' && c <= 'z')
			return c - 'a' + 'A';
	if(c > 0x7F)
		mlibc::infoLogger() << "mlibc: charset::to_upper() is not implemented"
				" for the full Unicode charset" << frg::endlog;
	return c;
}

charset *current_charset() {
	static charset global_charset;
	return &global_charset;
}

} // namespace mlibc

