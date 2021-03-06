// Copyright (C) 2010 EPITA Research and Development Laboratory (LRDE)
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


#ifndef MLN_FUN_V2V_QT_RGB_TO_INT_U_HH
# define MLN_FUN_V2V_QT_RGB_TO_INT_U_HH

/// \file
///
/// Convert a qt::rgb32 value to an int_u8.

# include <mln/value/qt/rgb32.hh>
# include <mln/value/int_u8.hh>

namespace mln
{

  namespace fun
  {

    namespace v2v
    {

      template <unsigned n>
      struct qt_rgb_to_int_u : Function_v2v< qt_rgb_to_int_u<n> >
      {
	typedef value::int_u<n> result;

	result operator()(const value::qt::rgb32& c) const;
      };

# ifndef MLN_INCLUDE_ONLY

      template <unsigned n>
      typename qt_rgb_to_int_u<n>::result
      qt_rgb_to_int_u<n>::operator()(const value::qt::rgb32& c) const
      {
	return (c.red() + c.green() + c.blue()) / 3;
      }

# endif // ! MLN_INCLUDE_ONLY

    } // end of namespace mln::fun::v2v

  } // end of namespace mln::fun

} // end of namespace mln

#endif // ! MLN_FUN_V2V_QT_RGB_TO_INT_U_HH
