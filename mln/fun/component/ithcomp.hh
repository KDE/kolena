// Copyright (C) 2007, 2008, 2009 EPITA Research and Development Laboratory (LRDE)
//
// This file is part of Olena.
//
// Olena is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, version 2 of the License.
//
// Olena is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Olena.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may use this file as part of a free
// software project without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to produce
// an executable, this file does not by itself cause the resulting
// executable to be covered by the GNU General Public License.  This
// exception does not however invalidate any other reasons why the
// executable file might be covered by the GNU General Public License.

#ifndef MLN_FUN_COMPONENT_ITHCOMP_HH
# define MLN_FUN_COMPONENT_ITHCOMP_HH

/// \file
///
/// Meta function to retrieve/modify the i'th component with a binary
/// function f(i, vec).

# include <mln/fun/binary.hh>
# include <mln/fun/component/comp.hh>

namespace mln
{

  namespace fun
  {

    struct ithcomp : binary<ithcomp>
    {
    };

  } // end of namespace mln::fun

# ifndef MLN_INCLUDE_ONLY

  namespace trait
  {

    namespace next
    {

      template <typename I, typename T>
      struct set_binary_<mln::fun::ithcomp, mln::value::Integer, I, mln::Object, T>
      {
	typedef set_binary_ ret;
	typedef I argument1;
	typedef T argument2;
	typedef mln_trait_nunary(mln::fun::comp, T) comp_t;
	typedef mln_result(comp_t) result;

	static result read(const argument1& i, const argument2& v)
	{
	  return comp_t::read(i, v);
	}
      };

    } // end of namespace mln::trait::next

  } // end of namespace mln::trait

# endif // ! MLN_INCLUDE_ONLY

} // end of namespace mln

#endif // ! MLN_FUN_COMPONENT_ITHCOMP_HH
