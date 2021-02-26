// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "variable.hpp"
#include "../utils.hpp"

namespace asteria {

Variable::
~Variable()
  {
  }

Variable_Callback&
Variable::
enumerate_variables_descent(Variable_Callback& callback)
  const
  {
    return this->m_value.enumerate_variables(callback);
  }

}  // namespace asteria
