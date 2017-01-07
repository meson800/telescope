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

	//sdl variables
	SDL_AudioSpec wantedSpec, actualSpec;
	SDL_AudioDeviceID audioDevice;

	//private noise vars
	NoiseInterface * noiseInter;
	std::thread noiseNetworkingThread;
	bool hasStartedNoise;

	//sizers for the connected node panel
	wxWindow* connectionWindow;
	wxBoxSizer* connectionSizer;
	std::map<uint64_t, NodeControl *> connectedNodes;

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
