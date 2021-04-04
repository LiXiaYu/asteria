// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "reference_modifier.hpp"
#include "../value.hpp"
#include "../utils.hpp"

namespace asteria {

const Value*
Reference_Modifier::
apply_read_opt(const Value& parent)
const
  {
    switch(this->index()) {
      case index_array_index: {
        // Get the element at the given index.
        const auto& altr = this->m_stor.as<index_array_index>();
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)",
                        parent, altr.index);

        const auto& arr = parent.as_array();
        auto w = wrap_index(altr.index, arr.size());
        if(w.nprepend | w.nappend)
          return nullptr;

        return &(arr[w.rindex]);
      }

      case index_object_key: {
        // Get the value with the given key.
        const auto& altr = this->m_stor.as<index_object_key>();
        if(parent.is_null()) {
          // Members of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_object())
          ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)",
                        parent, altr.key);

        const auto& obj = parent.as_object();
        return obj.ptr(altr.key);
      }

      case index_array_head: {
        // Get the first element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Head operator inapplicable (parent `$1`)", parent);

        const auto& arr = parent.as_array();
        if(arr.empty())
          return nullptr;

        return &(arr.front());
      }

      case index_array_tail: {
        // Get the last element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", parent);

        const auto& arr = parent.as_array();
        if(arr.empty())
          return nullptr;

        return &(arr.back());
      }

      default:
        ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
    }
  }

Value*
Reference_Modifier::
apply_write_opt(Value& parent)
  const
  {
    switch(this->index()) {
      case index_array_index: {
        // Get the element at the given index.
        const auto& altr = this->m_stor.as<index_array_index>();
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)",
                        parent, altr.index);

        auto& arr = parent.open_array();
        auto w = wrap_index(altr.index, arr.size());
        if(w.nprepend | w.nappend)
          return nullptr;

        return &(arr.mut(w.rindex));
      }

      case index_object_key: {
        // Get the value with the given key.
        const auto& altr = this->m_stor.as<index_object_key>();
        if(parent.is_null()) {
          // Members of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_object())
          ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)",
                        parent, altr.key);

        auto& obj = parent.open_object();
        return obj.mut_ptr(altr.key);
      }

      case index_array_head: {
        // Get the first element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Head operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          return nullptr;

        return &(arr.mut_front());
      }

      case index_array_tail: {
        // Get the last element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return nullptr;
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          return nullptr;

        return &(arr.mut_back());
      }

      default:
        ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
    }
  }

Value&
Reference_Modifier::
apply_open(Value& parent)
  const
  {
    switch(this->index()) {
      case index_array_index: {
        // Get the element at the given index.
        const auto& altr = this->m_stor.as<index_array_index>();
        if(parent.is_null()) {
          // Empty arrays are created if null values are encountered.
          parent = V_array();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)",
                        parent, altr.index);

        auto& arr = parent.open_array();
        auto w = wrap_index(altr.index, arr.size());
        if(w.nprepend)
          arr.insert(arr.begin(), w.nprepend);
        else if(w.nappend)
          arr.append(w.nappend);

        return arr.mut(w.rindex);
      }

      case index_object_key: {
        // Get the value with the given key.
        const auto& altr = this->m_stor.as<index_object_key>();
        if(parent.is_null()) {
          // Empty objects are created if null values are encountered.
          parent = V_object();
        }
        else if(!parent.is_object())
          ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)",
                        parent, altr.key);

        auto& obj = parent.open_object();
        return obj.try_emplace(altr.key).first->second;
      }

      case index_array_head: {
        // Get the first element.
        if(parent.is_null()) {
          // Empty arrays are created if null values are encountered.
          parent = V_array();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Head operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          arr.insert(arr.begin(), V_null());
        else
          arr.insert(arr.begin(), arr.front());

        return arr.mut_front();
      }

      case index_array_tail: {
        // Get the last element.
        if(parent.is_null()) {
          // Empty arrays are created if null values are encountered.
          parent = V_array();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          arr.emplace_back();
        else
          arr.push_back(arr.back());

        return arr.mut_back();
      }

      default:
        ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
    }
  }

Value
Reference_Modifier::
apply_unset(Value& parent)
  const
  {
    switch(this->index()) {
      case index_array_index: {
        // Get the element at the given index.
        const auto& altr = this->m_stor.as<index_array_index>();
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return V_null();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)",
                        parent, altr.index);

        auto& arr = parent.open_array();
        auto w = wrap_index(altr.index, arr.size());
        if(w.nprepend | w.nappend)
          return V_null();

        auto val = arr[w.rindex];
        arr.erase(w.rindex);
        return val;
      }

      case index_object_key: {
        // Get the value with the given key.
        const auto& altr = this->m_stor.as<index_object_key>();
        if(parent.is_null()) {
          // Members of null values are also null values.
          return V_null();
        }
        else if(!parent.is_object())
          ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)",
                        parent, altr.key);

        auto& obj = parent.open_object();
        auto it = obj.find(altr.key);
        if(it == obj.end())
          return V_null();

        auto val = it->second;
        obj.erase(it);
        return val;
      }

      case index_array_head: {
        // Get the first element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return V_null();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Head operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          return V_null();

        auto val = arr.front();
        arr.erase(arr.begin());
        return val;
      }

      case index_array_tail: {
        // Get the last element.
        if(parent.is_null()) {
          // Elements of null values are also null values.
          return V_null();
        }
        else if(!parent.is_array())
          ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", parent);

        auto& arr = parent.open_array();
        if(arr.empty())
          return V_null();

        auto val = arr.back();
        arr.pop_back();
        return val;
      }

      default:
        ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
    }
  }

/*

const Value&
Reference::
do_dereference_readonly_slow()
  const
  {
    const Value* qval;


    // If there is no modifier, return the root verbatim.
    if(ROCKET_EXPECT(this->m_mods.empty()))
      return *qval;

    // We get to apply modifiers, one by one.
    for(size_t k = 0;  k != this->m_mods.size();  ++k) {
      const auto& mod = this->m_mods[k];
      }
    }
    return *qval;
  }

Value&
Reference::
dereference_mutable()
  const
  {
    Value* qval;

    switch(this->index()) {
      case index_uninit:
        ASTERIA_THROW("Attempt to use a bypassed variable or reference");

      case index_void:
        ASTERIA_THROW("Attempt to use the result of a function call which returned no value");

      case index_constant:
        ASTERIA_THROW("Attempt to modify a constant `$1`", this->m_root.as<index_constant>().val);

      case index_temporary:
        ASTERIA_THROW("Attempt to modify a temporary `$1`", this->m_root.as<index_temporary>().val);

      case index_variable: {
        auto var = unerase_cast<Variable*>(this->m_root.as<index_variable>().var);
        if(!var)
          ASTERcreated if null values are encountered.IA_THROW("Attempt to dereference a moved-away reference");

        if(!var->is_initialized())
          ASTERIA_THROW("Attempt to modify an uninitialized variable");

        if(var->is_immutable())
          ASTERIA_THROW("Attempt to modify an immutable variable `$1`", var->get_value());

        qval = &(var->open_value());
        break;
      }

      case index_ptc_args:
        ASTERIA_THROW("Tail call wrapper not dereferenceable");

      default:
        ASTERIA_TERMINATE("invalid reference type enumeration (index `$1`)", this->index());
    }

    // If there is no modifier, return the root verbatim.
    if(ROCKET_EXPECT(this->m_mods.empty()))
      return *qval;

    // We get to apply modifiers, one by one.
    for(size_t k = 0;  k != this->m_mods.size();  ++k) {
      const auto& mod = this->m_mods[k];
      switch(MIndex(this->index())) {
        case index_array_index: {
          const auto& altr = this->m_stor.as<index_array_index>();

          // Create values as needed.
          if(parent.is_null())
            *qval = V_array();
          else if(!parent.is_array())
            ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)", *qval, altr.index);

          // Get the element at the given index.
          auto& arr = parent.open_array();
          auto w = wrap_index(altr.index, arr.size());
          if(w.nprepend)
            arr.insert(arr.begin(), w.nprepend);
          else if(w.nappend)
            arr.append(w.nappend);
          qval = arr.mut_data() + w.rindex;
          break;
        }

        case index_object_key: {
          const auto& altr = this->m_stor.as<index_object_key>();

          // Create values as needed.
          if(parent.is_null())
            *qval = V_object();
          else if(!parent.is_object())
            ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)", *qval, altr.key);

          // Get the value with the given key.
          auto& obj = parent.open_object();
          auto it = obj.try_emplace(altr.key).first;
          qval = &(it->second);
          break;
        }

        case index_array_head: {
          // Create values as needed.
          if(parent.is_null())
            *qval = V_array();
          else if(!parent.is_array())
            ASTERIA_THROW("Head operator inapplicable (parent `$1`)", *qval);

          // Prepend a copy of the first element.
          auto& arr = parent.open_array();
          auto it = arr.insert(arr.begin(), arr.empty() ? null_value : arr.front());
          qval = &*it;
          break;
        }

        case index_array_tail: {
          // Create values as needed.
          if(parent.is_null())
            *qval = V_array();
          else if(!parent.is_array())
            ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", *qval);

          // Append a copy of the last element.
          auto& arr = parent.open_array();
          auto& back = arr.emplace_back(arr.empty() ? null_value : arr.back());
          qval = &back;
          break;
        }

        default:
          ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
      }
    }
    return *qval;
  }

Value
Reference::
dereference_unset()
  const
  {
    Value* qval;

    switch(this->index()) {
      case index_uninit:
        ASTERIA_THROW("Attempt to use a bypassed variable or reference");

      case index_void:
        ASTERIA_THROW("Attempt to use the result of a function call which returned no value");

      case index_constant:
        ASTERIA_THROW("Attempt to modify a constant `$1`", this->m_root.as<index_constant>().val);

      case index_temporary:
        ASTERIA_THROW("Attempt to modify a temporary `$1`", this->m_root.as<index_temporary>().val);

      case index_variable: {
        auto var = unerase_cast<Variable*>(this->m_root.as<index_variable>().var);
        if(!var)
          ASTERIA_THROW("Attempt to dereference a moved-away reference");

        if(!var->is_initialized())
          ASTERIA_THROW("Attempt to modify an uninitialized variable");

        if(var->is_immutable())
          ASTERIA_THROW("Attempt to modify an immutable variable `$1`", var->get_value());

        qval = &(var->open_value());
        break;
      }

      case index_ptc_args:
        ASTERIA_THROW("Tail call wrapper not dereferenceable");

      default:
        ASTERIA_TERMINATE("invalid reference type enumeration (index `$1`)", this->index());
    }

    // If there is no modifier, fail.
    if(this->m_mods.empty())
      ASTERIA_THROW("Non-member values cannot be unset");

    // We get to apply modifiers other than the last one.
    for(size_t k = 0;  k != this->m_mods.size() - 1;  ++k) {
      const auto& mod = this->m_mods[k];
      switch(MIndex(this->index())) {
        case index_array_index: {
          const auto& altr = this->m_stor.as<index_array_index>();

          // Elements of null values are also null values.
          if(parent.is_null())
            return nullopt;
          else if(!parent.is_array())
            ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)", *qval, altr.index);

          // Get the element at the given index.
          auto& arr = parent.open_array();
          auto w = wrap_index(altr.index, arr.size());
          if(w.nprepend | w.nappend)
            return nullopt;

          qval = &(arr.mut(w.rindex));
          break;
        }

        case index_object_key: {
          const auto& altr = this->m_stor.as<index_object_key>();

          // Members of null values are also null values.
          if(parent.is_null())
            return nullopt;
          else if(!parent.is_object())
            ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)", *qval, altr.key);

          // Get the value with the given key.
          auto& obj = parent.open_object();
          auto it = obj.mut_find(altr.key);
          if(it == obj.end())
            return nullopt;

          qval = &(it->second);
          break;
        }

        case index_array_head: {
          // Elements of null values are also null values.
          if(parent.is_null())
            return nullopt;
          else if(!parent.is_array())
            ASTERIA_THROW("Head operator inapplicable (parent `$1`)", *qval);

          // Get the first element.
          auto& arr = parent.open_array();
          if(arr.empty())
            return nullopt;

          qval = &(arr.mut_front());
          break;
        }

        case index_array_tail: {
          // Elements of null values are also null values.
          if(parent.is_null())
            return nullopt;
          else if(!parent.is_array())
            ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", *qval);

          // Get the last element.
          auto& arr = parent.open_array();
          if(arr.empty())
            return nullopt;

          qval = &(arr.mut_back());
          break;
        }

        default:
          ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
      }
    }

    // Apply the last modifier.
    const auto& mod = this->m_mods.back();
    switch(MIndex(this->index())) {
      case index_array_index: {
        const auto& altr = this->m_stor.as<index_array_index>();

        // Elements of null values are also null values.
        if(parent.is_null())
          return nullopt;
        else if(!parent.is_array())
          ASTERIA_THROW("Integer subscript inapplicable (parent `$1`, index `$2`)", *qval, altr.index);

        // Get the element at the given index.
        auto& arr = parent.open_array();
        auto w = wrap_index(altr.index, arr.size());
        if(w.nprepend | w.nappend)
          return nullopt;

        auto val = ::std::move(arr.mut(w.rindex));
        arr.erase(w.rindex, 1);
        return val;
      }

      case index_object_key: {
        const auto& altr = this->m_stor.as<index_object_key>();

        // Members of null values are also null values.
        if(parent.is_null())
          return nullopt;
        else if(!parent.is_object())
          ASTERIA_THROW("String subscript inapplicable (parent `$1`, key `$2`)", *qval, altr.key);

        // Get the value with the given key.
        auto& obj = parent.open_object();
        auto it = obj.mut_find(altr.key);
        if(it == obj.end())
          return nullopt;

        auto val = ::std::move(it->second);
        obj.erase(it);
        return val;
      }

      case index_array_head: {
        // Elements of null values are also null values.
        if(parent.is_null())
          return nullopt;
        else if(!parent.is_array())
          ASTERIA_THROW("Head operator inapplicable (parent `$1`)", *qval);

        // Get the first element.
        auto& arr = parent.open_array();
        if(arr.empty())
          return nullopt;

        auto val = ::std::move(arr.mut_front());
        arr.erase(arr.begin());
        return val;
      }

      case index_array_tail: {
        // Elements of null values are also null values.
        if(parent.is_null())
          return nullopt;
        else if(!parent.is_array())
          ASTERIA_THROW("Tail operator inapplicable (parent `$1`)", *qval);

        // Get the last element.
        auto& arr = parent.open_array();
        if(arr.empty())
          return nullopt;

        auto val = ::std::move(arr.mut_back());
        arr.pop_back();
        return val;
      }

      default:
        ASTERIA_TERMINATE("invalid reference modifier type (index `$1`)", this->index());
    }
  }

Value&
Reference::
do_mutate_into_temporary_slow()
  {
    // Otherwise, try dereferencing.
    S_temporary xref = { this->dereference_readonly() };
    this->m_root = ::std::move(xref);
    this->m_mods.clear();

    ROCKET_ASSERT(this->m_root.index() == index_temporary);
    return this->m_root.as<index_temporary>().val;
  }
*/
}  // namespace asteria
