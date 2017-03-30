#include "FrequencyControl.h"

#include "AudioBlockControl.h"

#include <sstream>
#include <iostream>

wxBEGIN_EVENT_TABLE(FrequencyControl, wxPanel)
wxEND_EVENT_TABLE()

FrequencyControl::FrequencyControl(wxPanel * parent, uint32_t freq_)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SIMPLE)
	, freq(freq_)
{
	this->SetBackgroundColour(wxColour(214, 214, 214));
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
	//sizer->Add(control, wxSizerFlags(0).Left().Border(wxALL, 10));
	//sizer->Layout();	
	update();
}

void FrequencyControl::setTimestampBounds(uint64_t lower, uint64_t upper)
{
	lowerTimestamp = lower;
	upperTimestamp = upper;
	update();
}

void FrequencyControl::update(void)
{
	//get our current width and height
	int width, height;
	GetSize(&width, &height);
	//we can calculate a "timestamp difference", e.g. how wide the timestamp bounds are
	int deltaTimestamp = static_cast<int>(upperTimestamp - lowerTimestamp);
	//calculate a conversion factor
	double timestampToWidth = static_cast<double>(width) / static_cast<double>(deltaTimestamp);

	for (auto it : audioBlockControls)
	{
		//we have to compute this block's position, based on its timestamp
		uint64_t blockLower = it->getLowerTimestamp();
		uint64_t blockUpper = it->getUpperTimestamp();
		
		int64_t relativeLower = (blockLower > lowerTimestamp) ? (blockLower - lowerTimestamp) : ((lowerTimestamp - blockLower) * -1);
		int64_t relativeUpper = (blockUpper > lowerTimestamp) ? (blockUpper - lowerTimestamp) : ((lowerTimestamp - blockUpper) * -1);
		
		int newX = static_cast<int>(relativeLower * timestampToWidth);
		int newEndX = static_cast<int>(relativeUpper * timestampToWidth);

		int newWidth = newEndX - newX;
		it->SetSize(newX, 0, newWidth, height, wxSIZE_ALLOW_MINUS_ONE | wxSIZE_FORCE_EVENT);	
	}
	this->Refresh();
}
