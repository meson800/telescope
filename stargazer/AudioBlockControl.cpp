#include "AudioBlockControl.h"

wxBEGIN_EVENT_TABLE(AudioBlockControl, wxWindow)
	EVT_PAINT(AudioBlockControl::paintEvent)
	EVT_LEFT_UP(AudioBlockControl::playAudio)
wxEND_EVENT_TABLE()

AudioBlockControl::AudioBlockControl(wxWindow* parent, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData)
	: wxWindow(parent, wxID_ANY)
	, audioDevice(audioDevice_)
	, audioData(rawData)
{
	updateWidth();
	SetMinSize(wxSize(width, controlHeight));
}

void AudioBlockControl::updateWidth()
{
	width = audioData->second.size() / 4400;
	SetMinSize(wxSize(width, controlHeight));
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
	dc.DrawRectangle(0, 0, width, controlHeight);
}

void AudioBlockControl::playAudio(wxMouseEvent& event)
{
	SDL_QueueAudio(audioDevice, audioData->second.data(), audioData->second.size());
}	
