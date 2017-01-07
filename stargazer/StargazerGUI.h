#pragma once

#include <thread>

#include <wx/wxprec.h>
#include <wx/wx.h>

#include <noise/NoiseCallbacks.h>
#include <noise/NoiseAPI.h>
#include <noise/NoiseInterface.h>

#include "NoiseEvents.h"

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

	//private noise vars
	NoiseInterface * noiseInter;
	std::thread noiseNetworkingThread;
	bool hasStartedNoise;

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
