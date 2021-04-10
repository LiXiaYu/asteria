// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "utils.hpp"
#include <time.h>  // ::timespec, ::clock_gettime(), ::localtime()
#include <unistd.h>  // ::write

namespace asteria {
namespace {

constexpr char s_lcchars[][8] =
  {
    "[NUL]", "[SOH]", "[STX]", "[ETX]", "[EOT]", "[ENQ]", "[ACK]", "[BEL]",
    "[BS]",  "\t",    "\n\t",  "[VT]",  "[FF]",  "",      "[SO]",  "[SI]",
    "[DLE]", "[DC1]", "[DC2]", "[DC3]", "[DC4]", "[NAK]", "[SYN]", "[ETB]",
    "[CAN]", "[EM]",  "[SUB]", "[ESC]", "[FS]",  "[GS]",  "[RS]",  "[US]",
  };

constexpr char s_escapes[][8] =
  {
    "\\0",   "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06", "\\a",
    "\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "\\x0E", "\\x0F",
    "\\x10", "\\x11", "\\x12", "\\x13", "\\x14", "\\x15", "\\x16", "\\x17",
    "\\x18", "\\x19", "\\Z",   "\\e",   "\\x1C", "\\x1D", "\\x1E", "\\x1F",
    " ",     "!",     "\\\"",  "#",     "$",     "%",     "&",     "\\'",
    "(",     ")",     "*",     "+",     ",",     "-",     ".",     "/",
    "0",     "1",     "2",     "3",     "4",     "5",     "6",     "7",
    "8",     "9",     ":",     ";",     "<",     "=",     ">",     "?",
    "@",     "A",     "B",     "C",     "D",     "E",     "F",     "G",
    "H",     "I",     "J",     "K",     "L",     "M",     "N",     "O",
    "P",     "Q",     "R",     "S",     "T",     "U",     "V",     "W",
    "X",     "Y",     "Z",     "[",     "\\\\",  "]",     "^",     "_",
    "`",     "a",     "b",     "c",     "d",     "e",     "f",     "g",
    "h",     "i",     "j",     "k",     "l",     "m",     "n",     "o",
    "p",     "q",     "r",     "s",     "t",     "u",     "v",     "w",
    "x",     "y",     "z",     "{",     "|",     "}",     "~",     "\\x7F",
    "\\x80", "\\x81", "\\x82", "\\x83", "\\x84", "\\x85", "\\x86", "\\x87",
    "\\x88", "\\x89", "\\x8A", "\\x8B", "\\x8C", "\\x8D", "\\x8E", "\\x8F",
    "\\x90", "\\x91", "\\x92", "\\x93", "\\x94", "\\x95", "\\x96", "\\x97",
    "\\x98", "\\x99", "\\x9A", "\\x9B", "\\x9C", "\\x9D", "\\x9E", "\\x9F",
    "\\xA0", "\\xA1", "\\xA2", "\\xA3", "\\xA4", "\\xA5", "\\xA6", "\\xA7",
    "\\xA8", "\\xA9", "\\xAA", "\\xAB", "\\xAC", "\\xAD", "\\xAE", "\\xAF",
    "\\xB0", "\\xB1", "\\xB2", "\\xB3", "\\xB4", "\\xB5", "\\xB6", "\\xB7",
    "\\xB8", "\\xB9", "\\xBA", "\\xBB", "\\xBC", "\\xBD", "\\xBE", "\\xBF",
    "\\xC0", "\\xC1", "\\xC2", "\\xC3", "\\xC4", "\\xC5", "\\xC6", "\\xC7",
    "\\xC8", "\\xC9", "\\xCA", "\\xCB", "\\xCC", "\\xCD", "\\xCE", "\\xCF",
    "\\xD0", "\\xD1", "\\xD2", "\\xD3", "\\xD4", "\\xD5", "\\xD6", "\\xD7",
    "\\xD8", "\\xD9", "\\xDA", "\\xDB", "\\xDC", "\\xDD", "\\xDE", "\\xDF",
    "\\xE0", "\\xE1", "\\xE2", "\\xE3", "\\xE4", "\\xE5", "\\xE6", "\\xE7",
    "\\xE8", "\\xE9", "\\xEA", "\\xEB", "\\xEC", "\\xED", "\\xEE", "\\xEF",
    "\\xF0", "\\xF1", "\\xF2", "\\xF3", "\\xF4", "\\xF5", "\\xF6", "\\xF7",
    "\\xF8", "\\xF9", "\\xFA", "\\xFB", "\\xFC", "\\xFD", "\\xFE", "\\xFF",
  };

// POSIX
constexpr const char*
do_result_of_strerror_r(int /*r*/, char* s) noexcept
  {
    return s;
  }

// GNU
constexpr const char*
do_result_of_strerror_r(char* r, char* /*s*/) noexcept
  {
    return r;
  }

inline const char*
do_xstrerror_r(int errnum, char *strerrbuf, size_t buflen) noexcept
  {
    return do_result_of_strerror_r(
               ::strerror_r(errnum, strerrbuf, buflen),
               strerrbuf);
  }

}  // namespace

namespace details_utils {

const uint8_t cctype_table[128] =
  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x21, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
    0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x12,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x12,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

tinyfmt&
operator<<(tinyfmt& fmt, const Quote_Wrapper& q)
  {
    // Insert the leading quote mark.
    fmt << '\"';

    // Quote all bytes from the source string.
    for(size_t i = 0;  i < q.len;  ++i) {
      size_t ch = q.str[i] & 0xFF;
      const auto& sq = s_escapes[ch];

      // Insert this quoted sequence.
      // Optimize the operation a little if it consists of only one character.
      if(ROCKET_EXPECT(sq[1] == 0))
        fmt << sq[0];
      else
        fmt << sq;
    }

    // Insert the trailing quote mark.
    fmt << '\"';
    return fmt;
  }

tinyfmt&
operator<<(tinyfmt& fmt, const Paragraph_Wrapper& q)
  {
    if(q.indent == 0) {
      // Write everything in a single line, separated by spaces.
      fmt << ' ';
    }
    else {
      // Terminate the current line.
      fmt << '\n';

      // Indent the next line accordingly.
      for(size_t i = 0;  i < q.hanging;  ++i)
        fmt << ' ';
    }
    return fmt;
  }

tinyfmt&
operator<<(tinyfmt& fmt, const Formatted_errno& e)
  {
    // Write the error number, followed by its description.
    char sbuf[256];
    return fmt << "error " << e.err << ": "
         << do_xstrerror_r(e.err, sbuf, sizeof(sbuf));
  }

}  // namespace details_utils

ptrdiff_t
write_log_to_stderr(const char* file, long line, cow_string&& msg) noexcept
  {
    cow_string log_text;
    log_text.reserve(1023);

    // Append the timestamp.
    ::timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    ::tm tr;
    ::localtime_r(&(ts.tv_sec), &tr);

    char temp[64];
    ::strftime(temp, sizeof(temp), "%F %R", &tr);
    log_text += temp;
    ::sprintf(temp, ":%2.9f", tr.tm_sec + double(ts.tv_nsec) / 1.0e9);
    log_text += temp;

    // Append the file name and line number, followed by a line feed.
    log_text += " @ ";
    log_text += file;
    ::sprintf(temp, ":%ld", line);
    log_text += temp;
    log_text += "\n\t";

    // Neutralize control characters. That is ['\x00','\x1F'] and '\x7F'.
    for(size_t i = 0;  i < msg.size();  ++i) {
      size_t ch = msg[i] & 0xFF;
      if(ch <= 0x1F)
        log_text += s_lcchars[ch];
      else if(ch == 0x7F)
        log_text += "[DEL]";
      else
        log_text += static_cast<char>(ch);
    }
    log_text += "\n\n";

    // Write the string now. If the operation fails, we don't retry.
    return ::write(STDERR_FILENO, log_text.data(), log_text.size());
  }

bool
utf8_encode(char*& pos, char32_t cp) noexcept
  {
    if(cp < 0x80) {
      // This character takes only one byte.
      *(pos++) = static_cast<char>(cp);
      return true;
    }

    if((0xD800 <= cp) && (cp < 0xE000))
      // Surrogates are reserved for UTF-16.
      return false;

    if(cp >= 0x110000)
      // Code point is too large.
      return false;

    // Encode bits into a byte.
    auto encode_one = [&](int sh, char32_t m)
      { *(pos++) = static_cast<char>((~m << 1) | ((cp >> sh) & m));  };

    // Encode the code point now. The result may be 2, 3 or 4 bytes.
    if(cp < 0x800) {
      encode_one( 6, 0x1F);
      encode_one( 0, 0x3F);
    }
    else if(cp < 0x10000) {
      encode_one(12, 0x0F);
      encode_one( 6, 0x3F);
      encode_one( 0, 0x3F);
    }
    else {
      encode_one(18, 0x07);
      encode_one(12, 0x3F);
      encode_one( 6, 0x3F);
      encode_one( 0, 0x3F);
    }
    return true;
  }

bool
utf8_encode(cow_string& text, char32_t cp)
  {
    // Encode the code point into this temporary buffer.
    char str[4];
    char* pos = str;
    if(!utf8_encode(pos, cp))
      return false;

    // Append all bytes encoded.
    text.append(str, pos);
    return true;
  }

bool
utf8_decode(char32_t& cp, const char*& pos, size_t avail) noexcept
  {
    if(avail == 0)
      return false;

    // Read the first byte.
    cp = *(pos++) & 0xFF;
    if(cp < 0x80)
      // This sequence contains only one byte.
      return true;

    if((cp < 0xC0) || (0xF8 <= cp))
      // This is not a leading character.
      return false;

    // Calculate the number of bytes in this code point.
    auto u8len = static_cast<size_t>(2 + (cp >= 0xE0) + (cp >= 0xF0));
    ROCKET_ASSERT(u8len >= 2);
    ROCKET_ASSERT(u8len <= 4);
    if(u8len > avail)
      // No enough characters have been provided.
      return false;

    // Unset bits that are not part of the payload.
    cp &= UINT32_C(0xFF) >> u8len;

    // Accumulate trailing code units.
    for(size_t i = 1;  i < u8len;  ++i) {
      char32_t cu = *(pos++) & 0xFF;
      if((cu < 0x80) || (0xC0 <= cu))
        // This trailing character is not valid.
        return false;
      cp = (cp << 6) | (cu & 0x3F);
    }

    if((0xD800 <= cp) && (cp < 0xE000))
      // Surrogates are reserved for UTF-16.
      return false;

    if(cp >= 0x110000)
      // Code point is too large.
      return false;

    // Re-encode it and check for overlong sequences.
    auto milen = static_cast<size_t>(1 + (cp >= 0x80) + (cp >= 0x800) + (cp >= 0x10000));
    if(milen != u8len)
      // Overlong sequences are not allowed.
      return false;

    return true;
  }

bool
utf8_decode(char32_t& cp, const cow_string& text, size_t& offset)
  {
    if(offset >= text.size())
      return false;

    // Decode bytes.
    const char* pos = text.data() + offset;
    if(!utf8_decode(cp, pos, text.size() - offset))
      return false;

    // Update the offset.
    offset = static_cast<size_t>(pos - text.data());
    return true;
  }

bool
utf16_encode(char16_t*& pos, char32_t cp) noexcept
  {
    if((0xD800 <= cp) && (cp < 0xE000))
      // Surrogates are reserved for UTF-16.
      return false;

    if(cp < 0x10000) {
      // This character takes only one code unit.
      *(pos++) = static_cast<char16_t>(cp);
      return true;
    }

    if(cp >= 0x110000)
      // Code point is too large.
      return false;

    // Write surrogates.
    *(pos++) = static_cast<char16_t>(0xD800 + ((cp - 0x10000) >> 10));
    *(pos++) = static_cast<char16_t>(0xDC00 + (cp & 0x3FF));
    return true;
  }

bool
utf16_encode(cow_u16string& text, char32_t cp)
  {
    // Encode the code point into this temporary buffer.
    char16_t str[2];
    char16_t* pos = str;
    if(!utf16_encode(pos, cp))
      return false;

    // Append all bytes encoded.
    text.append(str, pos);
    return true;
  }

bool
utf16_decode(char32_t& cp, const char16_t*& pos, size_t avail) noexcept
  {
    if(avail == 0)
      return false;

    // Read the first code unit.
    cp = *(pos++) & 0xFFFF;
    if((cp < 0xD800) || (0xE000 <= cp))
      // This sequence contains only one code unit.
      return true;

    if(cp >= 0xDC00)
      // A trailing surrogate is not allowed unless following a leading surrogate.
      return false;

    if(avail < 2)
      // No enough code units have been provided.
      return false;

    // Read the trailing surrogate.
    char16_t cu = *(pos++);
    if((cu < 0xDC00) || (0xE000 <= cu))
      // Only a trailing surrogate is allowed to follow a leading surrogate.
      return false;

    cp = 0x10000 + ((cp & 0x3FF) << 10) + (cu & 0x3FF);
    return true;
  }

bool
utf16_decode(char32_t& cp, const cow_u16string& text, size_t& offset)
  {
    if(offset >= text.size())
      return false;

    // Decode bytes.
    const char16_t* pos = text.data() + offset;
    if(!utf16_decode(cp, pos, text.size() - offset))
      return false;

    // Update the offset.
    offset = static_cast<size_t>(pos - text.data());
    return true;
  }

Wrapped_Index
wrap_index(int64_t index, size_t size) noexcept
  {
    ROCKET_ASSERT(size <= PTRDIFF_MAX);

    // The range of valid indices is (~size, size).
    Wrapped_Index w;
    auto ssize = static_cast<int64_t>(size);
    if(index >= 0) {
      // Append elements as needed.
      auto nappend = ::rocket::max(index, ssize - 1) - (ssize - 1);
      w.nprepend = 0;
      w.nappend = static_cast<uint64_t>(nappend);

      // `index` is truncated if it does not fit in `size_t`, but in this case it
      // shouldn't be used.
      w.rindex = static_cast<size_t>(index);
    }
    else {
      // Prepend elements as needed.
      auto nprepend = ::rocket::max(index - 1, ~ssize) - (index - 1);
      w.nprepend = static_cast<uint64_t>(nprepend);
      w.nappend = 0;

      // `index + ssize` cannot overflow when `index` is negative and `ssize` is not.
      w.rindex = static_cast<size_t>(index + ssize) + static_cast<size_t>(nprepend);
    }
    return w;
  }

uint64_t
generate_random_seed() noexcept
  {
    // Get the system time of very high resolution.
    ::timespec ts;
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t ireg = static_cast<uint64_t>(ts.tv_sec);
    ireg <<= 30;
    ireg |= static_cast<uint32_t>(ts.tv_nsec);

    // Hash it using FNV-1a to erase sensitive information.
    //   https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    // The source value is read in little-endian byte order.
    uint64_t seed = 0xCBF29CE484222325;
    for(size_t i = 0;  i < 8;  ++i) {
      seed = (seed ^ (ireg & 0xFF)) * 0x100000001B3;
      ireg >>= 8;
    }
    return seed;
  }

}  // namespace asteria
