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

#ifndef MLN_MORPHO_CLOSING_APPROX_STRUCTURAL_HH
# define MLN_MORPHO_CLOSING_APPROX_STRUCTURAL_HH

/// \file
///
/// Approximate of morphological structural closing.

# include <mln/morpho/approx/dilation.hh>
# include <mln/morpho/approx/erosion.hh>


namespace mln
{

  namespace morpho
  {

    namespace closing
    {

      namespace approx
      {

	/// Approximate of morphological structural closing.
	///
	/// This operator is e_{-B} o d_B.
	///
	template <typename I, typename W>
	mln_concrete(I)
	structural(const Image<I>& input, const Window<W>& win);



# ifndef MLN_INCLUDE_ONLY


	template <typename I, typename W>
	inline
	mln_concrete(I)
	structural(const Image<I>& input, const Window<W>& win)
	{
	  trace::entering("morpho::closing::approx::structural");

	  mln_precondition(exact(input).is_valid());
	  mln_precondition(! exact(win).is_empty());

	  using mln::morpho::approx::dilation;
	  using mln::morpho::approx::erosion;

	  mln_concrete(I) output = erosion(dilation(input,
						    win),
					   win::sym(win));

	  mln_postcondition(output >= input);

	  trace::exiting("morpho::closing::approx::structural");
	  return output;
	}


# endif // ! MLN_INCLUDE_ONLY

      } // end of namespace mln::morpho::closing::approx

    } // end of namespace mln::morpho::closing

  } // end of namespace mln::morpho

} // end of namespace mln


#endif // ! MLN_MORPHO_CLOSING_APPROX_STRUCTURAL_HH
