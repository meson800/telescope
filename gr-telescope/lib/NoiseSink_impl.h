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

#ifndef INCLUDED_TELESCOPE_NOISESINK_IMPL_H
#define INCLUDED_TELESCOPE_NOISESINK_IMPL_H

#include <telescope/NoiseSink.h>
#include <noise/NoiseAPI.h>
#include <noise/Fingerprint.h>

namespace gr {
  namespace telescope {

    class NoiseSink_impl : public NoiseSink
    {
     private:
     /*!
      * Variable that stores our file descriptor for connection with the Noise daemon
      */
      int output_fd;

     /*!
      * Variable that stores the current frequency we've read
      */
      double cur_freq;

     /*!
      * Stores if we are currently reading a burst
      */
      bool is_in_burst;

     /*!
      * Stores the key for recognizing a time tag
      */
      pmt::pmt_t time_key;

     /*!
      * Stores the key for recognizing a burst tag
      */
      pmt::pmt_t burst_key;

     /*!
      * Stores the key for recognizing a frequency tag
      */
      pmt::pmt_t freq_key;

     /*!
      * Stores the sample rate that we are running at
      * (This is used for timestamp)
      */
      double sample_rate;

     /*!
      * Stores the time value of a sample
      */
      double timestamp;

     /*!
      * Stores the sample number that matches with the timestamp
      */
      uint64_t timestamp_sample;

     /*!
      * Stores if we've seen a valid time tag yet. It's undefined if we don't
      */
      bool is_timestamp_valid;

     /*!
      * Stores the maximum size of the accumulator before it sends a message
      */
      uint64_t max_accum_size;

     /*!
      * Buffer that stores samples before sending it through noise
      */
      std::vector<float> accum;

     /*!
      * Stores the initial timestamp of the burst
      */
      uint64_t burst_timestamp;

     /*!
      * Stores the current sub-message number for this burst
      */
      uint32_t burst_chunk_number;

     /*!
      * Stores the frequency of this burst
      */
      uint32_t burst_frequency;

     /*!
      * Stores the fingerprint that we should send data to (the stargazing node)
      */
      Fingerprint dest_fingerprint;

     /*!
      * Inits the accumulator and clears it without sending a message if any bytes were in the accumulator
      */
      void init_accumulator(double _timestamp, double _frequency);

     /*!
      * Adds bytes to our accumulator, and sends a message through Noise if we have exceeded our message length
      */
      void add_bytes_to_accumulator(const float * arr, uint64_t num_samples);

     /*!
      * Finalizes the accumulator and sends a partial message if any bytes were in the accumulator
      */
      void finalize_accumulator();

     /*!
      * Sends the current state of the accumulator as a message through Noise
      */
      void send_accumulator();

     public:
      NoiseSink_impl(const std::string& stargazing_key_, double sample_rate_, uint64_t max_accum_samples_);
      ~NoiseSink_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      /*!
       * Override this function to do some initalization after everything else has been created
       * This lets us get the radio source pointer no matter where it appears in the GRC script
       */
      bool start() override;
    };

  } // namespace telescope
} // namespace gr

#endif /* INCLUDED_TELESCOPE_NOISESINK_IMPL_H */

