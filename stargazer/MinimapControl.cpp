#include "MinimapControl.h"

#include "AudioBlockControl.h"

wxBEGIN_EVENT_TABLE(MinimapControl, wxWindow)
	EVT_PAINT(MinimapControl::OnPaint)
	EVT_SIZE(MinimapControl::OnSize)
wxEND_EVENT_TABLE()

MinimapControl::MinimapControl(wxWindow* parent, const AudioBlockMap& blocks,
		uint64_t lowest, uint64_t lower, uint64_t upper, uint64_t highest)
	: wxWindow(parent, wxID_ANY)
	, audioBlocks(blocks)
{
	updateTimestamps(lowest,lower,upper,highest);
}

void MinimapControl::updateTimestamps(uint64_t lowest, uint64_t lower, uint64_t upper, uint64_t highest)
{
	lowestTimestamp = lowest;
	lowerTimestamp = lower;
	upperTimestamp = upper;
	highestTimestamp = highest;
	Refresh();
}

void MinimapControl::OnSize(wxSizeEvent& event)
{
	Refresh();
}

void MinimapControl::OnPaint(wxPaintEvent& event)
{
	paintNow();
}

void MinimapControl::paintNow(void)
{
	wxClientDC dc(this);
	render(dc);
}

void MinimapControl::render(wxDC& dc)
{
	dc.Clear();
	int width, height;
	GetSize(&width, &height);
	
	double timestampToPixels = static_cast<double>(width) / static_cast<double>(highestTimestamp - lowestTimestamp);
	

	//loop over each audio block, and draw it accordingly.
	dc.SetBrush(*wxBLUE_BRUSH);
	dc.SetPen(*wxTRANSPARENT_PEN);
	for (auto mapIterator : audioBlocks)
	{
		for (auto blockIt : mapIterator.second)
		{
			if (blockIt.second->getUpperTimestamp() - blockIt.second->getLowerTimestamp() > 50)
			{
				dc.DrawRectangle(
					static_cast<int>((blockIt.second->getLowerTimestamp() - lowestTimestamp) * timestampToPixels),
					static_cast<int>(.25 * height),
					static_cast<int>((blockIt.second->getUpperTimestamp() - blockIt.second->getLowerTimestamp()) * timestampToPixels),
					static_cast<int>(.5 * height));
			}	
		}
	}
	//now draw lines to indicate the current viewing window if we are displaying a scroll bar
	dc.SetPen(*wxBLACK_PEN);
	if (lowerTimestamp > lowestTimestamp
		|| upperTimestamp < highestTimestamp)
	{
		int lowerX = static_cast<int>((lowerTimestamp - lowestTimestamp) * timestampToPixels);
		int upperX = static_cast<int>((upperTimestamp - lowestTimestamp) * timestampToPixels);
		
		if (upperX >= width)
		{
			upperX = width - 1;
		}
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawLine(lowerX, .1 * height, upperX, .1 * height);
		dc.DrawLine(upperX, .1 * height, upperX, .9 * height);
		dc.DrawLine(lowerX, .9 * height, upperX, .9 * height);
		dc.DrawLine(lowerX, .1 * height, lowerX, .9 * height);
	}

	//draw a horizontal line through the control to represent the main timeline
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.DrawLine(0, height / 2, width, height / 2);
}
