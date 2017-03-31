#pragma once 

#include <wx/wx.h>

class TimelineControl : public wxWindow
{
	static const int height = 25;
public:
	TimelineControl(wxWindow * parent, uint64_t lower, uint64_t upper);
	
	void updateTimestamps(uint64_t lower, uint64_t upper);
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void render(wxDC& dc);
	void paintNow(void);

	wxDECLARE_EVENT_TABLE();

private:
	uint64_t lowerTimestamp, upperTimestamp;
};
