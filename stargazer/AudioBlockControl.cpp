#include "AudioBlockControl.h"

#include "TelescopeGlobals.h"

wxBEGIN_EVENT_TABLE(AudioBlockControl, wxWindow)
	EVT_PAINT(AudioBlockControl::paintEvent)
	EVT_LEFT_UP(AudioBlockControl::playAudio)
wxEND_EVENT_TABLE()

AudioBlockControl::AudioBlockControl(wxWindow* parent, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData, uint64_t timestamp)
	: wxWindow(parent, wxID_ANY)
	, lowerTimestamp(timestamp)
	, audioDevice(audioDevice_)
	, audioData(rawData)
{}

uint64_t AudioBlockControl::getLowerTimestamp(void)
{
	return lowerTimestamp;
}

uint64_t AudioBlockControl::getUpperTimestamp(void)
{
	return getLowerTimestamp() + ((audioData->second.size() / AUDIO_RATE) * 1000 / 4);//we have four bytes per sample
}	

void AudioBlockControl::paintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void AudioBlockControl::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void AudioBlockControl::render(wxDC& dc)
{
	dc.SetBrush(*wxBLUE_BRUSH);
	int width, height;
	GetSize(&width, &height);	
	dc.DrawRectangle(0, 0, width, height);
}

void AudioBlockControl::playAudio(wxMouseEvent& event)
{
	SDL_QueueAudio(audioDevice, audioData->second.data(), audioData->second.size());
}	
