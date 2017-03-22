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

#include <sstream>
#include "FrequencyWatcher_impl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace gr 
{
  namespace telescope 
  {

    FrequencyWatcher::sptr
    FrequencyWatcher::make(const std::string &rtlsdr_alias, const std::string &frequencyList,
      uint32_t freqSwitchDuration, double frequencyOffset, bool isVerbose)
    {
      return gnuradio::get_initial_sptr
        (new FrequencyWatcher_impl(rtlsdr_alias, frequencyList, freqSwitchDuration, frequencyOffset, isVerbose));
    }

    /*
     * The private constructor
     */
    FrequencyWatcher_impl::FrequencyWatcher_impl(const std::string &_rtlsdr_alias, const std::string &frequencyList,
      uint32_t freqSwitchDuration, double _frequencyOffset, bool _isVerbose)
      : gr::sync_block("FrequencyWatcher",
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(short)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              frequencyOffset(_frequencyOffset), rtlsdr_alias(_rtlsdr_alias),
              changeFreqDuration(std::chrono::milliseconds(freqSwitchDuration)),
              isVerbose(_isVerbose), newFrequency(0), didChange(false), in_burst(false)
    {
      //set up the frequency key
      freq_key = pmt::string_to_symbol("freq");
	
      //set up the burst keys
      burst_key = pmt::string_to_symbol("burst");
      burst_false_value = pmt::PMT_F;
      burst_true_value = pmt::PMT_T; 

      //and set up the message port
      message_port_register_in(pmt::intern("FWCommands"));
      boost::bind(&FrequencyWatcher_impl::command_handler, this, _1);

      std::istringstream ss(frequencyList);
      double readDouble;

      while (ss >> readDouble)
      {
        frequencies.push_back(readDouble);

        if (ss.peek() == ',' || ss.peek() == ',')
        {
          ss.ignore();
        }
      }
      freqIt = frequencies.begin();

      if (isVerbose)
      {
        std::cout << "Read frequencies: ";
        for (unsigned int i = 0; i < frequencies.size(); ++i)
        {
          std::cout << frequencies[i];
          if (i != frequencies.size() - 1)
            std::cout << ",";
        }
        std::cout << "\n";

      }

     
    }

    bool FrequencyWatcher_impl::start()
    {
       //find the radio source block
      basic_block_sptr blockFromRegistry;
      //this might fail if we can't find the block in the registry
      try
      {
        blockFromRegistry = global_block_registry.block_lookup(pmt::intern(rtlsdr_alias));
        //try to convert it to our source block
        dRtlsdr = boost::dynamic_pointer_cast<osmosdr::source>(blockFromRegistry);

        if (isVerbose)
        {
          std::cout << "Successfully found the RTLSDR source block\n";
        }

        //connect into the noise daemon
        input_fd = open("noise_daemon_output", O_RDONLY);
        if (input_fd == -1)
        {
                close(input_fd);
                throw std::runtime_error("Can't connect to noise_daemon_output");
        }
        if (isVerbose)
        {
                std::cout << "Successfully connected to the noise daemon";
        }
      }
      catch (std::runtime_error)
      {
        std::cerr << "Cannot find the RTLSDR source block in the block registry\n";
        return false;
      }

      if (!dRtlsdr)
      {
        std::cerr << "RTLSDR source block pointer is invalid\n";
        return false;
      }

      return true;
    }

    void FrequencyWatcher_impl::command_handler(pmt::pmt_t message)
    {

    }

    void FrequencyWatcher_impl::setFrequency(double freq)
    {
      if (!dRtlsdr)
      {
        std::cerr << "Can't set frequency: invalid RTLSDR source block pointer\n";
        return;
      }
      if (isVerbose)
        std::cout << "Setting frequency to " << freq << " Hz\n";
      dRtlsdr->set_center_freq(freq + frequencyOffset);

      freqLock.lock();
      newFrequency = freq;
      didChange = true;
      freqLock.unlock();
    }

    double FrequencyWatcher_impl::getFrequency(void)
    {
      if (!dRtlsdr)
      {
        std::cerr << "Can't get frequency: invalid RTLSDR source block pointer\n";
        return 0.0;
      }
      return (dRtlsdr->get_center_freq() - frequencyOffset);
    }

    void FrequencyWatcher_impl::setAutoGain(bool automatic)
    {
      if (!dRtlsdr)
      {
        std::cerr << "Can't set gain mode: invalid RTLSDR source block poiner\n";
        return;
      }
      if (isVerbose)
        std::cout << "Setting gain mode to " << (automatic? "automatic" : "fixed") << "\n";
      dRtlsdr->set_gain_mode(automatic);
    }

    /*
     * Our virtual destructor.
     */
    FrequencyWatcher_impl::~FrequencyWatcher_impl()
    {
            //destroy our fd
            close(input_fd);
    }

    int
    FrequencyWatcher_impl::work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      const short *trigger = (const short*)input_items[1];
      gr_complex *out = (gr_complex *) output_items[0];

      //check if we need to change frequency.      
      freqLock.lock();
      auto current_time = std::chrono::steady_clock::now();
      if (!in_burst && current_time - lastFreqChangeTime > changeFreqDuration)
      {
            lastFreqChangeTime = current_time;
            setFrequency(*freqIt);
            
            if (++freqIt == frequencies.end())
            {
                  freqIt = frequencies.begin();
            }
      }

      //check to see if we need to emit a tag
      if (didChange)
      {
        add_item_tag(0, nitems_written(0), freq_key, pmt::from_double(newFrequency));
        didChange = false;
      }
      freqLock.unlock();

      //pass all the output items through
      memcpy(out, in, noutput_items * sizeof(gr_complex));
      
      for (int i = 0; i < noutput_items; ++i)
      {
        if (trigger[i] > 0)
        {
                if (in_burst == false)
                {
                        in_burst = true;
                        add_item_tag(0, nitems_written(0) + i, burst_key, burst_true_value);
                }
        } else {
                if (in_burst == true)
                {
                        in_burst = false;
                        add_item_tag(0, nitems_written(0) + i, burst_key, burst_false_value);
                }
        }
      } 

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace telescope */
} /* namespace gr */

