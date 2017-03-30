#pragma once

#include <wx/wx.h>

#include <string>
#include <vector>

//forward declarations
class AudioBlockControl;

class FrequencyControl : public wxPanel
{
public:	
	FrequencyControl(wxPanel* parent, uint32_t freq_);
	//Add another recieved audio block to this frequency control
	void addAudioBlock(AudioBlockControl* control, uint64_t timestamp);
	
	//Re-lays out the box sizer, as needed (due to updates to the audio blocks)
	void update(void);
	
	//Tells this frequency control the timestamp bounds
	void setTimestampBounds(uint32_t lower, uint32_t upper);

	wxDECLARE_EVENT_TABLE();

private:
	//whatever frequency we're using
	uint32_t freq;
	//stores all of our audio block controls
	std::vector<AudioBlockControl*> audioBlockControls;
	//our main sizer
	wxBoxSizer * sizer;
	
	std::string freqLabel;

	//stores the current timestamp bounds that we're displaying
	uint32_t lowerTimestamp, upperTimestamp;
};
