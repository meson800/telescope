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

#ifndef INCLUDED_TELESCOPE_FREQUENCYWATCHER_IMPL_H
#define INCLUDED_TELESCOPE_FREQUENCYWATCHER_IMPL_H

#include <mutex>
#include <vector>

#include <telescope/FrequencyWatcher.h>
#include <osmosdr/source.h>

namespace gr {
  namespace telescope {

    /*!
     * \brief Provides an interface to switch between multiple frequencies.
     * \ingroup block
     *
     * Allows for watching of multiple frequencies, pausing if one has a signal.
     * Inserts stream tags when the frequency is changed so downstream blocks can have this information.
     */
    class FrequencyWatcher_impl : public FrequencyWatcher
    {
     private:
      /*!
       * Mutex to prevent race conditions if this gets a frequency change message while processing signals
       */
      std::mutex freqLock;

      /*!
       * Variable storing the key for our stream tag
       */
      pmt::pmt_t freq_key;

      /*!
       * Stores a list of frequencies (doubles, in Hz) to watch
       */
      std::vector<double> frequencies;

      /*!
       * Variable that stores the frequency we jumped to.
       * Protect access to this with a mutex
       */
      double newFrequency;

      /*!
       * Variable that lets us know in the work function if we changed frequencies
       */
      bool didChange;

      /*!
       * If verbose output is enabled, commands are outputed to std::cout
       */
      bool isVerbose;

      /*!
       * Frequency offset set by the user.
       * This can be used to correct for a DC spike
       * when frequency is shifted down to 0 Hz
       */
      double frequencyOffset;

      /*!
       * Stores the name of the RTLSDR block alias until start can be called
       */
      std::string rtlsdr_alias;

      /*!
       * Changes the frequency our radio source is tuned to.
       * This transparently corrects using the frequency offset; just set what frequency is actually desired
       * Inserts a stream tag with the frequency change as soon as possible
       * \param freq Frequency in Hz to tune to
       */
      void setFrequency(double freq);

      /*!
       * Returns the frequency the radio soruce is tuned to, corrected for the frequency offset
       * \return Tuned frequency in Hz
       */
      double getFrequency(void);

      /*!
       * Sets if the radio source should use automatic RF gain
       * It can be useful to disable automatic gain when comparing antenna results to not skew results.
       * \param automatic True enables automatic gain, false disables automatic gain.
       */
      void setAutoGain(bool automatic);

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);
      osmosdr::source::sptr dRtlsdr;

     public:
      FrequencyWatcher_impl(const std::string &_rtlsdr_alias, const std::string &frequencyList,
        double _frequencyOffset, bool _isVerbose);
      ~FrequencyWatcher_impl();

      /*!
       * Message handler function for the input command port
       * \param message A PMT representing the incoming message
       */
       void command_handler(pmt::pmt_t message);

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

#endif /* INCLUDED_TELESCOPE_FREQUENCYWATCHER_IMPL_H */

