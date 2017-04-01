#pragma once

#include <wx/wx.h>
#include <SDL2/SDL.h>

#include <vector>
#include <map>

class AudioBlockControl : public wxWindow
{
	static const int controlHeight = 100;

	using AudioDataIterator = std::map<uint64_t, std::vector<unsigned char>>::iterator;
public:
	AudioBlockControl(wxWindow* parent, wxWindow* redraw, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData, uint64_t timestamp);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	uint64_t getLowerTimestamp(void) const;
	uint64_t getUpperTimestamp(void) const;

	bool hasBeenPlayed(void) const;
	void setPlayed(void);

	void render(wxDC& dc);
	void playAudio(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	void updateWaveform(void);

	wxWindow * redrawWindow;

	uint64_t lowerTimestamp;
	SDL_AudioDeviceID audioDevice;
	AudioDataIterator audioData;

	std::vector<int> waveform;
	wxSize oldSize;

	bool playStatus;
};
