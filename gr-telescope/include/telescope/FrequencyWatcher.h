/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_TELESCOPE_FREQUENCYWATCHER_H
#define INCLUDED_TELESCOPE_FREQUENCYWATCHER_H

#include <telescope/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace telescope {

    /*!
     * \brief <+description of block+>
     * \ingroup telescope
     *
     */
    class TELESCOPE_API FrequencyWatcher : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<FrequencyWatcher> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of telescope::FrequencyWatcher.
       *
       * To avoid accidental use of raw pointers, telescope::FrequencyWatcher's
       * constructor is in a private implementation
       * class. telescope::FrequencyWatcher::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace telescope
} // namespace gr

#endif /* INCLUDED_TELESCOPE_FREQUENCYWATCHER_H */

