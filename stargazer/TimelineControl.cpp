#include "TimelineControl.h"

wxBEGIN_EVENT_TABLE(TimelineControl, wxWindow)
	EVT_PAINT(TimelineControl::OnPaint)
	EVT_SIZE(TimelineControl::OnSize)
wxEND_EVENT_TABLE()

TimelineControl::TimelineControl(wxWindow* parent, uint64_t lower, uint64_t upper)
	: wxWindow(parent, wxID_ANY)
	, lowerTimestamp(lower)
	, upperTimestamp(upper)
{
	SetMinSize(wxSize(50, height));
}

void TimelineControl::updateTimestamps(uint64_t lower, uint64_t upper)
{
	lowerTimestamp = lower;
	upperTimestamp = upper;
	paintNow();
}

void TimelineControl::OnPaint(wxPaintEvent& event)
{
	paintNow();
}

void TimelineControl::paintNow(void)
{
	wxClientDC dc(this);
	render(dc);
}

void TimelineControl::render(wxDC& dc)
{
	wxFont font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
	dc.SetFont(font);
	int ourWidth, ourHeight;
	GetSize(&ourWidth, &ourHeight);
	
	double millisToPixels = static_cast<double>(ourWidth) / static_cast<double>((upperTimestamp - lowerTimestamp));
	for (uint64_t i = lowerTimestamp; i < upperTimestamp; i += 1000)
	{
		int x = static_cast<int>((i - lowerTimestamp) * millisToPixels);
		dc.DrawLine(x, 25, x, 15);
	}
}


void TimelineControl::OnSize(wxSizeEvent& event)
{
	Refresh();
}
