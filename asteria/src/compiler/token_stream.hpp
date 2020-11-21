// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_COMPILER_TOKEN_STREAM_HPP_
#define ASTERIA_COMPILER_TOKEN_STREAM_HPP_

#include "../fwd.hpp"
#include "../recursion_sentry.hpp"
#include "token.hpp"

namespace asteria {

class Token_Stream
  {
  private:
    Compiler_Options m_opts;
    Recursion_Sentry m_sentry;
    cow_vector<Token> m_rtoks;  // Tokens are stored in reverse order.

  public:
    explicit constexpr
    Token_Stream(const Compiler_Options& opts)
      noexcept
      : m_opts(opts)
      { }

    ASTERIA_NONCOPYABLE_DESTRUCTOR(Token_Stream);

  public:
    // This provides stack overflow protection.
    Recursion_Sentry
    copy_recursion_sentry()
      const
      { return this->m_sentry;  }

    const void*
    get_recursion_base()
      const noexcept
      { return this->m_sentry.get_base();  }

    Token_Stream&
    set_recursion_base(const void* base)
      noexcept
      { return this->m_sentry.set_base(base), *this;  }

    // These are accessors and modifiers of options for parsing.
    const Compiler_Options&
    get_options()
      const noexcept
      { return this->m_opts;  }

    Compiler_Options&
    open_options()
      noexcept
      { return this->m_opts;  }

    Token_Stream&
    set_options(const Compiler_Options& opts)
      noexcept
      { return this->m_opts = opts, *this;  }

    // These are accessors and modifiers of tokens in this stream.
    bool
    empty()
      const noexcept
      { return this->m_rtoks.empty();  }

    size_t
    size()
      const noexcept
      { return this->m_rtoks.size();  }

    const Token*
    peek_opt(size_t offset = 0)
      const noexcept
      { return this->m_rtoks.ptr(this->m_rtoks.size() + ~offset);  }

    Token_Stream&
    shift(size_t count = 1)
      noexcept
      { return this->m_rtoks.pop_back(count), *this;  }

    Source_Location
    next_sloc()
      const noexcept
      {
        if(this->m_rtoks.empty())
          return Source_Location(sref("[end of stream]"), -1, 0);
        else
          return this->m_rtoks.back().sloc();
      }

    size_t
    next_length()
      const noexcept
      {
        if(this->m_rtoks.empty())
          return 0;
        else
          return this->m_rtoks.back().length();
      }

    Token_Stream&
    clear()
      noexcept
      {
        this->m_rtoks.clear();
        return *this;
      }

    // This function parses characters from the input stream and fills tokens into `*this`.
    // The contents of `*this` are destroyed prior to any further operation.
    // This function throws a `Parser_Error` upon failure.
    Token_Stream&
    reload(const cow_string& file, int line, tinybuf& cbuf);
  };

}  // namespace asteria

#endif
