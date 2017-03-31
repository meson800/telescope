#include "AudioBlockControl.h"

#include "TelescopeGlobals.h"

#include <algorithm>

wxBEGIN_EVENT_TABLE(AudioBlockControl, wxWindow)
	EVT_PAINT(AudioBlockControl::paintEvent)
	EVT_LEFT_UP(AudioBlockControl::playAudio)
wxEND_EVENT_TABLE()

AudioBlockControl::AudioBlockControl(wxWindow* parent, SDL_AudioDeviceID audioDevice_, AudioDataIterator rawData, uint64_t timestamp)
	: wxWindow(parent, wxID_ANY)
	, lowerTimestamp(timestamp)
	, audioDevice(audioDevice_)
	, audioData(rawData)
{
	SetMinSize(wxSize(-1, controlHeight));
}

uint64_t AudioBlockControl::getLowerTimestamp(void) const
{
	return lowerTimestamp;
}

uint64_t AudioBlockControl::getUpperTimestamp(void) const
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

void AudioBlockControl::updateWaveform(void)
{
	int width, height;
	GetSize(&width, &height);
	//we are going to generate a waveform
	//we do this by computing an "offset" of up to height * 2, for each pixel in width
	waveform.clear();	
	if (width == 0)
	{
		return;
	}
	uint64_t numSamples = audioData->second.size() / 4;
	float* sampleStart = reinterpret_cast<float*>(&(audioData->second[0]));
	int increment = numSamples / width;
	for (int i = 0; i < numSamples - increment; i += increment)
	{
		float maxValue = 0;
		for (int j = 0; j < increment; ++j)
		{
			//flip the float value
			float value = sampleStart[i + j];
			if (value > maxValue)
			{
				maxValue = value;
			}
		}
		waveform.push_back(static_cast<int>(maxValue * static_cast<float>(height / 2)));
	} 
}

void AudioBlockControl::render(wxDC& dc)
{
	dc.SetBrush(*wxBLUE_BRUSH);
	int width, height;
	GetSize(&width, &height);	
	if (GetSize() != oldSize)
	{
		updateWaveform();
		oldSize = GetSize();
	}
	dc.SetBrush(wxBrush(wxColour(192, 192, 192)));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0, 0, width, height);

	dc.SetPen(wxPen(wxColour(100, 100, 220)));
	for (int i = 0; i < waveform.size(); ++i)
	{
		dc.DrawLine(i, (height / 2) - waveform[i], i, (height / 2) + waveform[i]);
	}
}

void AudioBlockControl::playAudio(wxMouseEvent& event)
{
	SDL_QueueAudio(audioDevice, audioData->second.data(), audioData->second.size());
}	
