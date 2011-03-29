// Copyright (C) 2010, 2011 EPITA Research and Development Laboratory
// (LRDE)
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

#ifndef SCRIBO_TOOLCHAIN_INTERNAL_TEXT_IN_DOC_FUNCTOR_HH
# define SCRIBO_TOOLCHAIN_INTERNAL_TEXT_IN_DOC_FUNCTOR_HH

# include <mln/io/ppm/save.hh>

# include <scribo/core/def/lbl_type.hh>

# include <scribo/primitive/extract/components.hh>
# include <scribo/primitive/extract/vertical_separators.hh>
# include <scribo/primitive/extract/separators_nonvisible.hh>

# include <scribo/primitive/remove/separators.hh>

# include <scribo/filter/object_links_bbox_h_ratio.hh>
# include <scribo/filter/objects_small.hh>

# include <scribo/primitive/group/from_single_link.hh>

# include <scribo/primitive/link/merge_double_link.hh>
# include <scribo/primitive/link/internal/dmax_width_and_height.hh>
# include <scribo/primitive/link/with_single_left_link_dmax_ratio.hh>
# include <scribo/primitive/link/with_single_right_link_dmax_ratio.hh>

# include <scribo/preprocessing/denoise_fg.hh>

# include <scribo/text/recognition.hh>
# include <scribo/text/merging.hh>

# include <scribo/make/debug_filename.hh>

# include <scribo/debug/save_bboxes_image.hh>
# include <scribo/debug/save_linked_bboxes_image.hh>
# include <scribo/debug/bboxes_enlarged_image.hh>
# include <scribo/debug/mean_and_base_lines_image.hh>
# include <scribo/debug/looks_like_a_text_line_image.hh>

# include <scribo/toolchain/internal/toolchain_functor.hh>


namespace scribo
{

  namespace toolchain
  {

    namespace internal
    {


      template <typename I>
      struct text_in_doc_functor
	: public Toolchain_Functor
      {
	typedef value::label<30> V;
	typedef mln_ch_value(I,V) L;

	text_in_doc_functor();

	virtual int nsteps() const;

	//===============
	// Core function
	//===============

	line_set<L> operator()(const Image<I>& input_);


	//=========
	// Options
	//=========
	bool enable_denoising;
	bool enable_line_seps;
	bool enable_whitespace_seps;
	bool enable_debug;

	//============
	// Parameters
	//============

	std::string ocr_language;


	// Results
	line_set<L> output;
      };


# ifndef MLN_INCLUDE_ONLY

      template <typename I>
      text_in_doc_functor<I>::text_in_doc_functor()
	: enable_denoising(true),
	  enable_line_seps(true),
	  enable_whitespace_seps(true),
	  enable_debug(false),
	  ocr_language("eng")
      {
      }


      //===============
      // Core function
      //===============

      template <typename I>
      line_set<typename text_in_doc_functor<I>::L>
      text_in_doc_functor<I>::operator()(const Image<I>& input)
      {
	// Remove separators
	mln_ch_value(I,bool)
	  separators,
	  input_cleaned = exact(input);

	if (enable_line_seps)
	{
	  on_new_progress_label("Find vertical separators...");

	  // Vertical separators
	  separators = primitive::extract::vertical_separators(input, 81);

	  on_progress();

	  on_new_progress_label("Remove separators...");

	  input_cleaned = primitive::remove::separators(input, separators);

	  on_progress();
	}

	mln_ch_value(I,bool) whitespaces;
	if (enable_whitespace_seps)
	{
	  // Whitespace separators
	  on_new_progress_label("Find whitespace separators...");

	  whitespaces = primitive::extract::separators_nonvisible(input);

	  on_progress();
	}

	if (enable_debug)
	{
	  if (enable_whitespace_seps)
	    mln::io::pbm::save(whitespaces,
			       scribo::make::debug_filename("whitespaces.pbm"));

	  if (enable_line_seps)
	  {
	    mln::io::pbm::save(separators,
			       scribo::make::debug_filename("vseparators.pbm"));

	    mln::io::pbm::save(input_cleaned,
			       scribo::make::debug_filename("input_wo_vseparators.pbm"));
	  }
	}

	// Denoise
	if (enable_denoising)
	{
	  on_new_progress_label("Denoise...");

	  input_cleaned = preprocessing::denoise_fg(input_cleaned, c8(), 3);

	  if (enable_debug)
	    mln::io::pbm::save(input_cleaned,
			       scribo::make::debug_filename("denoised.pbm"));

	  on_progress();
	}

	/// Finding components.
	on_new_progress_label("Finding components...");

	V ncomponents;
	component_set<L>
	  components = scribo::primitive::extract::components(input_cleaned, c8(),
							      ncomponents);

	on_progress();

	/// Set separator components.
	if (enable_line_seps)
	  components.add_separators(separators);
	if (enable_whitespace_seps)
	  components.add_separators(whitespaces);

	if (enable_debug)
	  mln::io::pbm::save(components.separators(),
			     scribo::make::debug_filename("all_separators.pbm"));


	on_new_progress_label("Filtering components");

	components = scribo::filter::components_small(components, 3);

	on_progress();


	/// Linking potential objects
	on_new_progress_label("Linking objects...");

	object_links<L> left_link
	  = primitive::link::with_single_left_link_dmax_ratio(components,
							      primitive::link::internal::dmax_width_and_height(1),
							      anchor::MassCenter);
	object_links<L> right_link
	  = primitive::link::with_single_right_link_dmax_ratio(components,
							       primitive::link::internal::dmax_width_and_height(1),
							       anchor::MassCenter);

	if (enable_debug)
	{
	  debug::save_linked_bboxes_image(input, left_link, right_link,
					  literal::blue,
					  literal::cyan,
					  literal::yellow,
					  literal::green,
					  anchor::MassCenter,
					  scribo::make::debug_filename("object_links.ppm"));
	}


	// Validating left and right links.
	object_links<L>
	  merged_links = primitive::link::merge_double_link(left_link,
							    right_link);

	on_progress();


	on_new_progress_label("Filtering objects");

	// Remove links if bboxes have too different sizes.
	object_links<L> hratio_filtered_links
	  = filter::object_links_bbox_h_ratio(merged_links, 2.5f);


	if (enable_debug)
	{
	  mln_ch_value(I,value::rgb8)
	    hratio_decision_image = scribo::debug::decision_image(input,
								  merged_links,
								  hratio_filtered_links,
								  anchor::MassCenter);
	  io::ppm::save(hratio_decision_image,
			scribo::make::debug_filename("hratio_links_decision_image.ppm"));
	}

	on_progress();


	on_new_progress_label("Rebuilding lines");

	object_groups<L>
	  groups = primitive::group::from_single_link(hratio_filtered_links);



	// Construct a line set.
	line_set<L>
	  lines = scribo::make::line_set(groups);


	//===== DEBUG =====

	if (enable_debug)
	{

	  // Bboxes image.
	  scribo::debug::save_bboxes_image(input, lines,
					   scribo::make::debug_filename("step1_bboxes.ppm"));

	  // Bboxes enlarged
	  mln::io::ppm::save(scribo::debug::bboxes_enlarged_image(input, lines),
			     scribo::make::debug_filename("step1_bboxes_enlarged.ppm"));

	  // Looks like a text line
	  mln::io::ppm::save(scribo::debug::looks_like_a_text_line_image(input, lines),
			     scribo::make::debug_filename("step1_looks_like_a_text_line.ppm"));


	  // mean and base lines.
	  mln::io::ppm::save(scribo::debug::mean_and_base_lines_image(input, lines),
			     scribo::make::debug_filename("step1_x_height.ppm"));

	}
	//===== END OF DEBUG =====



	lines = scribo::text::merging(lines);



	//===== DEBUG =====

	if (enable_debug)
	{

	  // mean and base lines.
	  mln::io::ppm::save(scribo::debug::mean_and_base_lines_image(input, lines),
			     scribo::make::debug_filename("step2_x_height.ppm"));

	  // Looks like a text line
	  mln::io::ppm::save(scribo::debug::looks_like_a_text_line_image(input, lines),
			     scribo::make::debug_filename("step2_looks_like_a_text_line.ppm"));

	  // Bboxes image.
	  scribo::debug::save_bboxes_image(input, lines,
					   scribo::make::debug_filename("step2_bboxes.ppm"));


	}



	if (enable_debug)
	{
	  std::ofstream file(scribo::make::debug_filename("step2_bboxes_100p.txt").c_str());

	  for_all_lines(l, lines)
	    if (lines(l).tag() != line::Merged
		&& lines(l).tag() != line::Ignored
		&& lines(l).tag() != line::Pathological)
	    {
	      file << lines(l).bbox().pmin().row() << " "
		   << lines(l).bbox().pmin().col() << " "
		   << lines(l).bbox().pmax().row() << " "
		   << lines(l).bbox().pmax().col() << " "
		   << lines(l).card()              << " "
		   << lines(l).baseline()          << " "
		   << lines(l).x_height()          << " "
		   << lines(l).meanline()          << " "
		   << lines(l).d_height()          << " "
		   << lines(l).a_height()          << " "
		   << lines(l).char_space()        << " "
		   << lines(l).char_width()        << std::endl;

	    }

	  file.close();
	}


	//===== END OF DEBUG =====

	on_progress();

	on_new_progress_label("Recognizing text");

	scribo::text::recognition(lines, ocr_language.c_str());

	on_progress();

	output = lines;
	return output;
      }



      template<typename I>
      int
      text_in_doc_functor<I>::nsteps() const
      {
	return 6 + enable_denoising + enable_line_seps
	  + enable_whitespace_seps;
      }


# endif // ! MLN_INCLUDE_ONLY


    } // end of namespace scribo::toolchain::internal

  } // end of namespace scribo::toolchain

} // end of namespace scribo

#endif // ! SCRIBO_TOOLCHAIN_INTERNAL_TEXT_IN_DOC_FUNCTOR_HH
