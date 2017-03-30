#pragma once

#include <wx/wx.h>
#include <SDL2/SDL.h>

#include <vector>
#include <map>

class AudioBlockControl : public wxWindow
{
	static const int controlHeight = 50;

	using AudioDataIterator = std::map<uint64_t, std::vector<unsigned char>>::iterator;
public:
	AudioBlockControl(wxWindow* parent, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData, uint64_t timestamp);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	uint64_t getLowerTimestamp(void);
	uint64_t getUpperTimestamp(void);

	void render(wxDC& dc);
	void playAudio(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	uint64_t lowerTimestamp;
	SDL_AudioDeviceID audioDevice;
	AudioDataIterator audioData;
};
