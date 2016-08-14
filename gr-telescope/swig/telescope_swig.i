/* -*- c++ -*- */

#define TELESCOPE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "telescope_swig_doc.i"

%{
#include "telescope/FrequencyWatcher.h"
#include "telescope/NoiseSink.h"
%}


%include "telescope/FrequencyWatcher.h"
GR_SWIG_BLOCK_MAGIC2(telescope, FrequencyWatcher);
%include "telescope/NoiseSink.h"
GR_SWIG_BLOCK_MAGIC2(telescope, NoiseSink);
