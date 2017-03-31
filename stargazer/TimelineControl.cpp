#include "TimelineControl.h"

#include <chrono>

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
	dc.Clear();
	int ourWidth, ourHeight;
	GetSize(&ourWidth, &ourHeight);

	//round to nearest minute
	std::chrono::milliseconds lowerRounded = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::milliseconds(lowerTimestamp));
	std::chrono::time_point<std::chrono::system_clock> timestamp =
		std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(lowerTimestamp)));
	auto time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);


	//std::cout << "Recieved Noise Message at timestamp "
	//	<< timestamp_str << " and frequency " << freq << " Hz, chunk ID = " << chunk_id << "\n";
	
	double millisToPixels = static_cast<double>(ourWidth) / static_cast<double>((upperTimestamp - lowerTimestamp));
	uint64_t currentLabel = lowerRounded.count();
	std::chrono::milliseconds currentTime = lowerRounded;
	while (currentLabel < upperTimestamp)
	{
		//Draw the label with the timestamp
		std::chrono::time_point<std::chrono::system_clock> currentTimestamp =
			std::chrono::system_clock::time_point(currentTime);
		auto time_t_timestamp = std::chrono::system_clock::to_time_t(currentTimestamp);
		char timestamp_str [100];
		strftime(timestamp_str, sizeof(timestamp_str), "%k:%M:%S", std::localtime(&time_t_timestamp));
		
		wxSize textSize = dc.GetTextExtent(timestamp_str);
		if (currentLabel > lowerTimestamp)
		{
			int textX = static_cast<int>((currentLabel - lowerTimestamp) * millisToPixels) - textSize.GetWidth() / 2;
			if (textX > 0)
			{
				dc.DrawText(timestamp_str, textX, 0);
			}
		}

		//Draw line ticks for every second
		for (int i = 0; i < 15; ++i)
		{
			int x = static_cast<int>((currentLabel + (1000 * i) - lowerTimestamp) * millisToPixels);
			int height = (i == 0) ? 10 : 5;
			if (x > 0)
			{
				dc.DrawLine(x, 25, x, 25 - height);
			}
		}
		currentTime += std::chrono::seconds(15);
		currentLabel = currentTime.count();
	}
}


void TimelineControl::OnSize(wxSizeEvent& event)
{
	Refresh();
}
