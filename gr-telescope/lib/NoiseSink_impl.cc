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
#include <noise/Helpers.h>
#include <stdexcept>
#include <chrono>
#include "NoiseSink_impl.h"
#include "TelescopeMessages.h"

#include <spawn.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace gr {
  namespace telescope {

    NoiseSink::sptr
    NoiseSink::make(const std::string & stargazing_key, double sample_rate, uint64_t max_accum_samples)
    {
      return gnuradio::get_initial_sptr
        (new NoiseSink_impl(stargazing_key, sample_rate, max_accum_samples));
    }

    /*
     * The private constructor
     */
    NoiseSink_impl::NoiseSink_impl(const std::string & stargazing_key_, double sample_rate_, uint64_t max_accum_samples_)
      : gr::sync_block("NoiseSink",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0, 0, 0))
      , is_in_burst(false)
      , time_key(pmt::string_to_symbol("rx_time"))
      , burst_key(pmt::string_to_symbol("burst"))
      , freq_key(pmt::string_to_symbol("freq"))
      , sample_rate(sample_rate_)
      , timestamp(0.0)
      , timestamp_sample(0)
      , is_timestamp_valid(false)
      , max_accum_size(max_accum_samples_)
      , dest_fingerprint(stargazing_key_)
    {
        char * argv[] = {const_cast<char*>("noise_daemon"), NULL};
        pid_t pid;
        int status = posix_spawnp(&pid, "noise_daemon", NULL, NULL, argv, environ);
        if (status != 0)
        {
                throw std::runtime_error("Couldn't start Noise daemon");
        }
        std::cout << "Started noise daemon with pid:" << pid << "\n";

        //reserve enough space in our accumulator
        accum.reserve(max_accum_size);
    }

    bool
    NoiseSink_impl::start()
    {
        //now init our output pipe
        output_fd = open("noise_daemon_input", O_WRONLY);
        if (output_fd == -1)
        {
                close(output_fd);
                throw std::runtime_error("Couldn't open noise_daemon_input");
        }
    }


    /*
     * Our virtual destructor.
     */
    NoiseSink_impl::~NoiseSink_impl()
    {
            close(output_fd);
    }

    void
    NoiseSink_impl::init_accumulator(double _timestamp, double _frequency)
    {
            std::chrono::time_point<std::chrono::system_clock> epoch_time;
            auto time_since_epoch = std::chrono::system_clock::now() - epoch_time;
            burst_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch).count();
            burst_frequency = _frequency;
            burst_chunk_number = 0;
            accum.clear();
    }

    void
    NoiseSink_impl::add_bytes_to_accumulator(const float * arr, uint64_t num_samples)
    {
            uint64_t cur_index = 0;
            while (cur_index < num_samples)
            {
                    uint64_t remaining_samples = max_accum_size - accum.size();
                    //send a message through the daemon if we need to
                    if (remaining_samples == 0)
                    {
                            send_accumulator();
                            accum.clear();
                    } else {
                            //find number of samples to insert
                            uint64_t samples_to_insert = ((num_samples - cur_index) < remaining_samples) ? (num_samples - cur_index) : remaining_samples;
                            accum.insert(accum.end(), arr + cur_index, arr + cur_index + samples_to_insert);
                            cur_index += samples_to_insert;
                    }
            }
    }

    void
    NoiseSink_impl::finalize_accumulator()
    {
            send_accumulator();
            accum.clear();
    }

    void
    NoiseSink_impl::send_accumulator()
    {
        //write the destination fingerprint
	std::vector<unsigned char> fingerprint_size = Helpers::uintToBytes(dest_fingerprint.data.size());
	Helpers::writeToFd(output_fd, fingerprint_size);	
	Helpers::writeToFd(output_fd, dest_fingerprint.data);

        //write the message data
	std::vector<unsigned char> message_size = Helpers::uintToBytes(accum.size() * sizeof(float)
		+ 8 + 4 + 4); //8 for the timestamp, 4 for frequency, 4 for the burst ID
	Helpers::writeToFd(output_fd, message_size);

        //write the initial timestamp info, the frequency, and the sub-burst number
	std::vector<unsigned char> timestamp = Helpers::uint64ToBytes(burst_timestamp);
	std::vector<unsigned char> freq = Helpers::uintToBytes(burst_frequency);
	std::vector<unsigned char> sub_burst_id = Helpers::uintToBytes(burst_chunk_number);
	Helpers::writeToFd(output_fd, timestamp);
	Helpers::writeToFd(output_fd, freq);
	Helpers::writeToFd(output_fd, sub_burst_id);
	


        //do this part manually as we have 
        uint64_t cur_index = 0;
        uint64_t length = accum.size() * sizeof(float);
        const unsigned char * buf = reinterpret_cast<unsigned char*>(accum.data());
        while (cur_index < length)
        {
               int bytes_written = write(output_fd, buf + cur_index, length - cur_index); 
                if (bytes_written == -1)
                {
                    throw std::runtime_error("Write threw an exception");
                }
                cur_index += bytes_written;
        }

	//increment our counter
	++burst_chunk_number;
    }

    int
    NoiseSink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];

      //get all the tags in this batch
      uint64_t cur_index = 0;
      uint64_t start_sample = nitems_read(0);
      uint64_t end_sample = start_sample + static_cast<uint64_t>(noutput_items);

      std::vector<tag_t> all_tags;
      get_tags_in_range(all_tags, 0, start_sample, end_sample);
      std::sort(all_tags.begin(), all_tags.end(), tag_t::offset_compare);
      //now we have all of our tags for this block in order. Let's process them!
      auto tag_it = all_tags.begin();
      while (tag_it != all_tags.end())
      {
              //see if we got a time tag! If so, update our time tag
              if (pmt::eqv(((*tag_it).key), time_key))
              {
                      uint64_t offset = (*tag_it).offset;
                      pmt::pmt_t time = (*tag_it).value;
                      uint64_t num_secs = pmt::to_uint64(pmt::tuple_ref(time, 0));
                      double fractional_secs = pmt::to_double(pmt::tuple_ref(time, 1));
                      double time_offset = (double)offset / sample_rate;
                      
                      timestamp = (double)num_secs + fractional_secs + time_offset;
                      timestamp_sample = offset; 
                      is_timestamp_valid = true;

                      std::cout << "Found a timestamp\n";
              } else if (pmt::eqv(((*tag_it).key), freq_key)) {
                      //set the last frequency seen so we can transfer it
                      cur_freq = pmt::to_double((*tag_it).value);
                      std::cout << "Changed frequency to " << cur_freq << "\n";
                      if (is_in_burst)
                      {
                              finalize_accumulator();
                              init_accumulator(timestamp +
                                (static_cast<double>((*tag_it).offset - timestamp_sample) / sample_rate)
                                , cur_freq);
                      }
              } else if (pmt::eqv(((*tag_it).key), burst_key)) {
                      if (pmt::is_true((*tag_it).value))
                      {
                              /*
                              if (!is_timestamp_valid)
                              {
                                      throw std::runtime_error("Started a burst before we have a valid timestamp");
                              }
                              */
                              std::cout << "Started a burst\n";
                              //yay! We started a burst
                              //if we were already in a burst, finalize it
                              if (is_in_burst)
                              {
                                      finalize_accumulator();
                              }
                              is_in_burst = true;
                              init_accumulator(timestamp +
                                (static_cast<double>((*tag_it).offset - timestamp_sample) / sample_rate)
                                , cur_freq);
                              cur_index = (*tag_it).offset - start_sample;
                      } else {
                              //add all samples from our last tagged point to now to get what we want
                              std::cout << "Found the end of a burst\n";
                              uint64_t end_index = (*tag_it).offset - start_sample;
                              add_bytes_to_accumulator(in + cur_index, end_index - cur_index);
                              finalize_accumulator();
                              is_in_burst = false;
                      }
              }
              ++tag_it;
      }
      //we're done, so let's just add everything remaining
      if (is_in_burst)
      {
              add_bytes_to_accumulator(in + cur_index, static_cast<uint64_t>(noutput_items) - cur_index);
      }
      return noutput_items;
    }

  } /* namespace telescope */
} /* namespace gr */

