#pragma once

#include <thread>
#include <map>

#include <wx/wxprec.h>
#include <wx/wx.h>

#include <SDL2/SDL.h>

#include <noise/NoiseCallbacks.h>
#include <noise/NoiseAPI.h>
#include <noise/NoiseInterface.h>

#include "NoiseEvents.h"
#include "NodeControl.h"

class AudioBlockControl;
class FrequencyControl;
class TimelineControl;
class MinimapControl;

class MainFrame : public wxFrame, public NoiseAPI::NoiseCallbacks
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame();

	//noise callback overrides
	void MessageRecieved(const Message& message) override;
	void NodeConnected(uint64_t system) override;
	void NodeDisconnected(uint64_t system) override;
	void FingerprintVerified(uint64_t system, const Fingerprint& fingerprint) override;


private:
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnStartNoise(wxCommandEvent& event);
	void OnConnectionEvent(ConnectionEvent& event);
	void OnFingerprintEvent(FingerprintEvent& event);
	void OnMessageEvent(MessageEvent& event);
	void OnScroll(wxScrollEvent& event);

	void OnLiveCheckbox(wxCommandEvent& event);
	void OnAutoscrollCheckbox(wxCommandEvent& event);

	void UpdateDataPanel(void);
	void UpdateScrollbar(void);

	void StartNoise();

	//sdl variables
	SDL_AudioSpec wantedSpec, actualSpec;
	SDL_AudioDeviceID audioDevice;

	//private noise vars
	NoiseInterface * noiseInter;
	std::thread noiseNetworkingThread;
	bool hasStartedNoise;

	//sizers for the whole display
	wxBoxSizer* mainSizer;
	wxPanel* mainPanel;

	//sizers for the connected node panel
	wxBoxSizer* connectionSizer;
	std::map<uint64_t, NodeControl *> connectedNodes;

	//sizers for the data panel
	wxPanel* dataPanel;
	wxFlexGridSizer* dataSizer;
	//Each frequency gets its own sizer
	std::map<uint32_t, FrequencyControl *> frequencyControls;
	//scrollbar to show this
	wxBoxSizer* scrollbarSizer;
	wxScrollBar* dataScroll;

	//map that stores audio blocks recieved
	//the first key is frequency, the second is (raw) timestamp
	std::map<uint32_t, std::map<uint64_t, AudioBlockControl*>> recievedAudioControls;
	std::map<uint32_t, std::map<uint64_t, std::vector<unsigned char>>> recievedAudio;

	//store the current lowest and largest timestamp values
	uint64_t lowestTimestamp;
	uint64_t highestTimestamp;
	
	uint64_t currentLowerTimestamp;
	uint64_t currentUpperTimestamp;
	bool firstEventSeen;

	//variable for the timeline control
	TimelineControl * timeline;
	
	//variable for the minimap control
	MinimapControl * minimap;

	//checkboxes for settings
	wxCheckBox * liveCheckbox;
	wxCheckBox * autoscrollCheckbox;
	bool isLiveFeed;
	bool isAutoscroll;
	
	enum ids
	{	LiveCheckboxID = 172,
		AutoscrollCheckboxID
	};

	wxMenu *fileMenu;
	wxMenu *helpMenu;
	wxMenuBar *topMenu;

	wxDECLARE_EVENT_TABLE();
};

class StargazerApp : public wxApp
{
public:
	StargazerApp();
	~StargazerApp();
	virtual bool OnInit();
private:
	MainFrame *guiFrame;
};
