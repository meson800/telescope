#pragma once

#include <wx/wx.h>

#include <map>

class AudioBlockControl;

class MinimapControl : public wxWindow
{
	
	using AudioBlockMap = std::map<uint32_t, std::map<uint64_t, AudioBlockControl*>>;
public:
	MinimapControl(wxWindow* parent, const AudioBlockMap& blocks,
		uint64_t lowest, uint64_t lower, uint64_t upper, uint64_t highest);
	
	void updateTimestamps(uint64_t lowest, uint64_t lower, uint64_t upper, uint64_t highest);
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void render(wxDC& dc);
	void paintNow(void);

	wxDECLARE_EVENT_TABLE();

private:
	const AudioBlockMap& audioBlocks;
	uint64_t lowestTimestamp, lowerTimestamp, upperTimestamp, highestTimestamp;
};
