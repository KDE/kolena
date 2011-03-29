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

#ifndef SCRIBO_BINARIZATION_INTERNAL_COMPUTE_SAUVOLA_THRESHOLD_HH
# define SCRIBO_BINARIZATION_INTERNAL_COMPUTE_SAUVOLA_THRESHOLD_HH


/// \file
///
/// \brief Compute a threshold with Sauvola's binarization formula.

# include <algorithm>
# include <cmath>

# include <mln/core/image/image2d.hh>
# include <mln/value/int_u8.hh>

# include <scribo/binarization/internal/sauvola_debug.hh>


// Setup default Sauvola's formulae parameters values.
// These macros may be used in other variant of Sauvola's algorithm.
//
// Values are set according to the following reference: "Automatic
// Evaluation of Document Binarization Results", Badekas and al, 2005
//
// Badekas et al. said 0.34 was best.
# define SCRIBO_DEFAULT_SAUVOLA_K 0.34
//
// 128 is best for grayscale documents.
# define SCRIBO_DEFAULT_SAUVOLA_R 128


namespace scribo
{

  namespace binarization
  {

    namespace internal
    {

      using namespace mln;


      /*! \brief Compute a point wise threshold according Sauvola's
          binarization.

          \param[in] p A site.
          \param[in] simple An integral image of mean values.
          \param[in] squared An integral image of squared mean values.
          \param[in] win_width Window width.
          \param[in] k Control the threshold value in the local
                       window. The higher, the lower the threshold
                       form the local mean m(x, y).
          \param[in] R Maximum value of the standard deviation (128
                       for grayscale documents).

	  \return A threshold.
      */
      template <typename P, typename J>
      double
      compute_sauvola_threshold(const P& p,
				const J& simple,
				const J& squared,
				int win_width, double K, double R);

      /// \overload
      /// K is set to 0.34 and R to 128.
      //
      template <typename P, typename J>
      double
      compute_sauvola_threshold(const P& p,
				const J& simple,
				const J& squared,
				int win_width);



# ifndef MLN_INCLUDE_ONLY



      /*! \brief compute Sauvola's threshold applying directly the formula.

	\param[in] m_x_y Mean value.
	\param[in] s_x_y Standard deviation.
        \param[in] k Control the threshold value in the local
                     window. The higher, the lower the threshold
                     form the local mean m(x, y).
        \param[in] R Maximum value of the standard deviation (128
                     for grayscale documents).

	\return A threshold.
       */
      inline
      double
      sauvola_threshold_formula(const double m_x_y, const double s_x_y,
				const double K, const double R)
      {
	return m_x_y * (1.0 + K * ((s_x_y / R) - 1.0));
      }

      /// \overload
      /// K is set to 0.34 and R to 128.
      //
      inline
      double
      sauvola_threshold_formula(double m_x_y, double s_x_y)
      {
 	return sauvola_threshold_formula(m_x_y, s_x_y,
 					 SCRIBO_DEFAULT_SAUVOLA_K,
 					 SCRIBO_DEFAULT_SAUVOLA_R);
      }



      template <typename P, typename J>
      double
      compute_sauvola_threshold(const P& p,
				const J& simple,
				const J& squared,
				int win_width, double K, double R)
      {
	mln_precondition(simple.nrows() == squared.nrows());
	mln_precondition(simple.ncols() == squared.ncols());

	// Window half width.
	int w_2 = win_width >> 1;

	int row_min = std::max(0, p.row() - w_2 - 1);
	int col_min = std::max(0, p.col() - w_2 - 1);

	int row_max = std::min(static_cast<int>(simple.nrows()) - 1,
			       p.row() + w_2);
	int col_max = std::min(static_cast<int>(simple.ncols()) - 1,
			       p.col() + w_2);


	double wh = (row_max - row_min) * (col_max - col_min);

	// Mean.
	double m_x_y_tmp = (simple.at_(row_max, col_max)
			    + simple.at_(row_min, col_min)
			    - simple.at_(row_max, col_min)
			    - simple.at_(row_min, col_max));

	double m_x_y = m_x_y_tmp / wh;

#  ifdef SCRIBO_SAUVOLA_DEBUG
	// Store local mean
	debug_mean(p) = m_x_y * mean_debug_factor;
#  endif // ! SCRIBO_SAUVOLA_DEBUG

	// Standard deviation.
	double s_x_y_tmp = (squared.at_(row_max, col_max)
			    + squared.at_(row_min, col_min)
			    - squared.at_(row_max, col_min)
			    - squared.at_(row_min, col_max));

	double s_x_y = std::sqrt((s_x_y_tmp - (m_x_y_tmp * m_x_y_tmp) / wh) / (wh - 1.f));

#  ifdef SCRIBO_SAUVOLA_DEBUG
	// Store local standard deviation
	debug_stddev(p) = s_x_y * stddev_debug_factor;
#  endif // ! SCRIBO_SAUVOLA_DEBUG

	// Thresholding.
	double t_x_y = sauvola_threshold_formula(m_x_y, s_x_y, K, R);

#  ifdef SCRIBO_SAUVOLA_DEBUG
	double alpha = K * (1 - s_x_y / R);
	debug_alpham(p) = alpha * m_x_y * alpham_debug_factor;
	debug_alphacond(p) = (s_x_y < (alpha * m_x_y / 2.));
#  endif // !SCRIBO_SAUVOLA_DEBUG

	return t_x_y;
      }


      template <typename P, typename J>
      double
      compute_sauvola_threshold_single_image(const P& p,
					     const J& integral,
					     int win_width,
					     double K, double R)
      {
	// Window half width.
	int w_2 = win_width >> 1;

	int row_min = std::max(0, p.row() - w_2);
	int col_min = std::max(0, p.col() - w_2);

	int row_max = std::min(static_cast<int>(integral.nrows()) - 1,
			       p.row() + w_2);
	int col_max = std::min(static_cast<int>(integral.ncols()) - 1,
			       p.col() + w_2);


	double wh = (row_max - row_min + 1) * (col_max - col_min + 1);

	// Mean.
	double m_x_y_tmp = (integral.at_(row_max, col_max).first()
			    + integral.at_(row_min, col_min).first()
			    - integral.at_(row_max, col_min).first()
			    - integral.at_(row_min, col_max).first());

	double m_x_y = m_x_y_tmp / wh;

#  ifdef SCRIBO_SAUVOLA_DEBUG
	// Store local mean
	debug_mean(p) = m_x_y * mean_debug_factor;
#  endif // ! SCRIBO_SAUVOLA_DEBUG

	// Standard deviation.
	double s_x_y_tmp = (integral.at_(row_max, col_max).second()
			    + integral.at_(row_min, col_min).second()
			    - integral.at_(row_max, col_min).second()
			    - integral.at_(row_min, col_max).second());

	double s_x_y = std::sqrt((s_x_y_tmp - (m_x_y_tmp * m_x_y_tmp) / wh) / (wh - 1.f));

#  ifdef SCRIBO_SAUVOLA_DEBUG
	// Store local standard deviation
	debug_stddev(p) = s_x_y * stddev_debug_factor;
#  endif // !SCRIBO_SAUVOLA_DEBUG

	// Thresholding.
	double t_x_y = sauvola_threshold_formula(m_x_y, s_x_y, K, R);

#  ifdef SCRIBO_SAUVOLA_DEBUG
	double alpha = K * (1 - s_x_y / R);
	debug_alpham(p) = alpha * m_x_y * alpham_debug_factor;
	debug_alphacond(p) = (s_x_y < (alpha * m_x_y / 2.));
#  endif // !SCRIBO_SAUVOLA_DEBUG

	return t_x_y;
      }



      template <typename P, typename J>
      double
      compute_sauvola_threshold(const P& p,
				const J& simple,
				const J& squared,
				int win_width)
      {
	return compute_sauvola_threshold(p, simple, squared, win_width,
					 SCRIBO_DEFAULT_SAUVOLA_K,
					 SCRIBO_DEFAULT_SAUVOLA_R);
      }


#endif // ! MLN_INCLUDE_ONLY

    } // end of namespace scribo::binarization::internal

  } // end of namespace scribo::binarization

} // end of namespace scribo

#endif // ! SCRIBO_BINARIZATION_INTERNAL_COMPUTE_SAUVOLA_THRESHOLD_HH
