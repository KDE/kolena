// Copyright (C) 2006, 2007, 2008, 2009 EPITA Research and Development Laboratory (LRDE)
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

#ifndef MLN_TRAIT_OP_TIMES_HH
# define MLN_TRAIT_OP_TIMES_HH

/*! \file
 *
 * \brief Declaration of the "binary multiplication" operator trait.
 */

# include <mln/trait/op/decl.hh>


# define mln_trait_op_times(L, R)  typename mln::trait::op::times< L , R >::ret
# define mln_trait_op_times_(L, R)          mln::trait::op::times< L , R >::ret



namespace mln
{

  namespace trait
  {

    namespace op
    {


      template <typename L, typename R>
      struct times : public solve_binary<times, L, R>
      {
      };


      // FIXME: Try to make the code below compile.

//       template <typename L, typename R>
//       struct times : mlc_equal< mln_exact(R), R >::check_t
//       {
// 	typedef solve_binary<mln::trait::op::times, L, R> solve_t;
// 	typedef typename solve_t::ret ret;
//       };

    } // end of namespace mln::trait::op

  } // end of namespace mln::trait

} // end of namespace mln


# include <mln/trait/solve.hh>


#endif // ! MLN_TRAIT_OP_TIMES_HH