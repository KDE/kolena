// Copyright (C) 2011 EPITA Research and Development Laboratory (LRDE)
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

#ifndef SCRIBO_CORE_PARAGRAPH_INFO_HH
# define SCRIBO_CORE_PARAGRAPH_INFO_HH

# include <scribo/core/line_info.hh>
# include <scribo/core/line_links.hh>
# include <mln/util/array.hh>
# include <mln/accu/shape/bbox.hh>

namespace scribo
{

  /*! \brief Paragraph structure information.

   */
  template <typename L>
  class paragraph_info
  {
  public:
    paragraph_info();
    paragraph_info(const line_links<L>& llinks);

    void add_line(const line_info<L>& line);

    const mln::box2d& bbox() const;

    const line_info<L>& line(line_id_t id) const;

    const mln::util::array<line_id_t>& line_ids() const;

    bool is_valid() const;

  private:
    mln::util::array<line_id_t> line_ids_;
    mln::accu::shape::bbox<mln_site(L)> bbox_;
    line_links<L> llinks_;
  };


# ifndef MLN_INCLUDE_ONLY


  template <typename L>
  paragraph_info<L>::paragraph_info()
  {
  }

  template <typename L>
  paragraph_info<L>::paragraph_info(const line_links<L>& llinks)
    : llinks_(llinks)
  {
  }

  template <typename L>
  void
  paragraph_info<L>::add_line(const line_info<L>& line)
  {
    line_ids_.append(line.id());
    bbox_.take(line.bbox());
  }

  template <typename L>
  const mln::box2d&
  paragraph_info<L>::bbox() const
  {
    return bbox_.to_result();
  }

  template <typename L>
  const line_info<L>&
  paragraph_info<L>::line(line_id_t id) const
  {
    mln_precondition(is_valid());
    return llinks_.lines()(id);
  }

  template <typename L>
  const mln::util::array<line_id_t>&
  paragraph_info<L>::line_ids() const
  {
    return line_ids_;
  }

  template <typename L>
  bool
  paragraph_info<L>::is_valid() const
  {
    return llinks_.is_valid();
  }


# endif // ! MLN_INCLUDE_ONLY

} // end of namespace scribo

#endif // ! SCRIBO_CORE_PARAGRAPH_INFO_HH
