#include "FrequencyControl.h"

#include "AudioBlockControl.h"

#include <sstream>

wxBEGIN_EVENT_TABLE(FrequencyControl, wxPanel)
wxEND_EVENT_TABLE()

FrequencyControl::FrequencyControl(wxPanel * parent, uint32_t freq_)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SIMPLE)
	, freq(freq_)
{
	this->SetBackgroundColour(wxColour(142, 142, 142));
	sizer = new wxBoxSizer(wxHORIZONTAL); 	
	this->SetSizer(sizer);

	std::ostringstream label_string;
	//set display of frequencies to the number of sigfigs we have
	label_string.precision(4);
	label_string << std::fixed;
	
	label_string << (static_cast<double>(freq) / (1000.0 * 1000.0));
	label_string << " MHz";
	
	freqLabel = label_string.str();
	
	wxStaticText * label = new wxStaticText(this, wxID_ANY, freqLabel.c_str());
	sizer->Add(label, wxSizerFlags(0).Left().Center().Border(wxALL, 10));
	sizer->Layout();
}

void FrequencyControl::addAudioBlock(AudioBlockControl * control, uint64_t timestamp)
{
	audioBlockControls.push_back(control);
	sizer->Add(control, wxSizerFlags(0).Left().Border(wxALL, 10));
	sizer->Layout();	
}

void FrequencyControl::update(void)
{
	for (auto it : audioBlockControls)
	{
		it->updateWidth();
	}
	sizer->Layout();
}
