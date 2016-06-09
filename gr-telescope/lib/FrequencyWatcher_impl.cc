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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/block_registry.h>
#include <pmt/pmt.h>
#include "FrequencyWatcher_impl.h"

namespace gr 
{
  namespace telescope 
  {

    FrequencyWatcher::sptr
    FrequencyWatcher::make(const std::string &rtlsdr_alias)
    {
      return gnuradio::get_initial_sptr
        (new FrequencyWatcher_impl(rtlsdr_alias));
    }

    /*
     * The private constructor
     */
    FrequencyWatcher_impl::FrequencyWatcher_impl(const std::string &rtlsdr_alias)
      : gr::sync_block("FrequencyWatcher",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      basic_block_sptr blockFromRegistry;
      //this might fail if we can't find the block in the registry
      try
      {
        blockFromRegistry = global_block_registry.block_lookup(pmt::intern(rtlsdr_alias));
        //try to convert it to our source block
        dRtlsdr = boost::dynamic_pointer_cast<osmosdr::source>(blockFromRegistry);
      }
      catch (std::runtime_error)
      {
        std::cerr << "Cannot find the RTLSDR source block in the block registry\n";
      }

      if (!dRtlsdr)
      {
        std::cerr << "RTLSDR source block pointer is invalid\n";
      }
    }

    /*
     * Our virtual destructor.
     */
    FrequencyWatcher_impl::~FrequencyWatcher_impl()
    {
    }

    int
    FrequencyWatcher_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      for (int i = 0; i < noutput_items; ++i)
      {
        out[i] = in[i];
      }
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace telescope */
} /* namespace gr */

