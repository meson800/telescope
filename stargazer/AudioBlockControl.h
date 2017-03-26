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
	AudioBlockControl(wxWindow* parent, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	void updateWidth();
	void render(wxDC& dc);
	void playAudio(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	uint32_t width;
	SDL_AudioDeviceID audioDevice;
	AudioDataIterator audioData;
};
