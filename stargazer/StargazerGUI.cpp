#include "StargazerGUI.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>

#include <SDL2/SDL.h>

#include <noise/Helpers.h>

#include "AudioBlockControl.h"
#include "FrequencyControl.h"

StargazerApp::StargazerApp()
	: guiFrame(nullptr)
{
	SDL_Init(SDL_INIT_AUDIO);
}


StargazerApp::~StargazerApp()
{
	SDL_Quit();
}

bool StargazerApp::OnInit()
{
	guiFrame = new MainFrame("Telescope", wxPoint(50, 50), wxSize(450, 350));
	guiFrame->Show(true);
	return true;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
	, hasStartedNoise(false)
{
	SDL_zero(wantedSpec);
	wantedSpec.freq = 48000;
	wantedSpec.format = AUDIO_F32LSB;
	wantedSpec.channels = 1;
	wantedSpec.samples = 4096;
	wantedSpec.callback = nullptr;
	audioDevice = SDL_OpenAudioDevice(NULL, 0, &wantedSpec, &actualSpec, 0);
	SDL_PauseAudioDevice(audioDevice, 0);
	//init noise
	
	fileMenu = new wxMenu;
	helpMenu = new wxMenu;

	fileMenu->Append(wxID_NETWORK, "&Connect");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);
	helpMenu->Append(wxID_ABOUT);

	topMenu = new wxMenuBar;
	topMenu->Append(fileMenu, "&Main");
	topMenu->Append(helpMenu, "&Help");

	SetMenuBar(topMenu);
	CreateStatusBar();
	SetStatusText("Noise has not been started");

	mainSizer = new wxBoxSizer(wxVERTICAL);
	mainPanel = new wxPanel(this, -1);
	mainPanel->SetSizer(mainSizer);	
	mainPanel->Show();

	//init the connection frame
	connectionSizer = new wxBoxSizer(wxHORIZONTAL);

	mainSizer->Add(connectionSizer, wxSizerFlags(0).Right().Border(wxALL, 10));

	//init the data frame
	dataSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(dataSizer, wxSizerFlags(0).Left().Right().Expand().Border(wxALL, 10));
}

MainFrame::~MainFrame()
{
	SDL_CloseAudioDevice(audioDevice);
	if (hasStartedNoise)
	{
		noiseInter->writeKeysToFile();
		noiseInter->stopNetworking();
		noiseNetworkingThread.join();
	}
	for (auto it = connectedNodes.begin(); it != connectedNodes.end(); ++it)
	{
		delete it->second;
	}
}

void MainFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("Telescope Stargazing Node\nCopyright 2016 Christopher Johnstone", "About Telescope", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnStartNoise(wxCommandEvent& event)
{
	std::thread initThread(&MainFrame::StartNoise, this);
	initThread.join();
}

void MainFrame::StartNoise()
{
	noiseInter = NoiseAPI::createNoiseInterface();
	
	noiseInter->loadKeysFromFile();
	if (noiseInter->numOurEncryptionKeys() == 0)
	{
		noiseInter->generateNewEncryptionKey();
	}
	std::stringstream status_builder;
	status_builder << "Started with encryption key " << noiseInter->getOurEncryptionKeyByIndex(0).toString() << "\n";
	SetStatusText(status_builder.str().c_str());
	noiseNetworkingThread = std::thread(&NoiseInterface::startNetworking, noiseInter, SERVER_PORT);

	noiseInter->addCallbackClass(this);

	hasStartedNoise = true;
}

void MainFrame::OnConnectionEvent(ConnectionEvent & event)
{
	if (event.isConnected)
	{
		NodeControl * newNode;
		if (connectedNodes.count(event.system))
		{
			newNode = connectedNodes[event.system];
		} else {
			newNode = new NodeControl(mainPanel, event.system);
			connectedNodes[event.system] = newNode;
		}
		connectionSizer->Add(newNode, wxSizerFlags(0).Left().Border(wxALL, 10));
		connectionSizer->Layout();
		mainSizer->Layout();
		newNode->Show();
	}
	else
	{
		if (connectedNodes.count(event.system))
		{
			connectionSizer->Detach(connectedNodes[event.system]);
			connectedNodes[event.system]->Hide();
			connectionSizer->Layout();
			mainSizer->Layout();
		}
	}


}

void MainFrame::OnFingerprintEvent(FingerprintEvent & event)
{
	if (connectedNodes.count(event.system))
	{
		connectedNodes[event.system]->verifyFingerprint(event.fingerprint.toString());
	}
}

void MainFrame::OnMessageEvent(MessageEvent & event)
{
	//extract the information on timestamp, frequency, and chunk ID
	const unsigned char* message_start = const_cast<const unsigned char*>(event.message.message.data());
	uint64_t millis_since_epoch = Helpers::bytesToUINT64(message_start);
	
	uint64_t freq = Helpers::bytesToUINT(message_start + 8);
	uint64_t chunk_id = Helpers::bytesToUINT(message_start + 12);

	std::cout << "Raw timestamp in millis is " << millis_since_epoch << "\n";
	std::chrono::time_point<std::chrono::system_clock> timestamp =
		std::chrono::system_clock::time_point(std::chrono::milliseconds(millis_since_epoch));
	auto time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);

	//because GCC is a pile of trash, we can't use std::put_time, because it's not implemented....
	char timestamp_str [100];
	strftime(timestamp_str, sizeof(timestamp_str), "%F %T %p", std::localtime(&time_t_timestamp));

	std::cout << "Recieved Noise Message at timestamp "
		<< timestamp_str << " and frequency " << freq << " Hz, chunk ID = " << chunk_id << "\n";

	bool newAudioBlock = (recievedAudio[freq].count(millis_since_epoch) == 0);
	std::vector<unsigned char> audio_data = std::vector<unsigned char>(event.message.message.begin() + 16, event.message.message.end());
	//add to the recieved audio section
	std::vector<unsigned char>& destVec = (recievedAudio[freq])[millis_since_epoch];
	destVec.insert(destVec.end(), audio_data.begin(), audio_data.end());

	//and create a new control for this recieved message
	if (newAudioBlock)
	{
		//see if we need to create a FrequencyControl to hold this
		if (frequencyControls.count(freq) == 0)
		{
			FrequencyControl * newFreqControl = new FrequencyControl(mainPanel, freq);
			frequencyControls[freq] = newFreqControl;
			dataSizer->Add(newFreqControl, wxSizerFlags(0).Left().Border(wxALL, 10));
			dataSizer->Layout();
		}

		AudioBlockControl * newControl = new AudioBlockControl(frequencyControls[freq], audioDevice,
			recievedAudio[freq].find(millis_since_epoch));
		(recievedAudioControls[freq])[millis_since_epoch] = newControl;
		frequencyControls[freq]->addAudioBlock(newControl, millis_since_epoch);
		dataSizer->Layout();
		mainSizer->Layout();
		newControl->Show();
	} else {
		//just update the width of the block
		(recievedAudioControls[freq])[millis_since_epoch]->updateWidth();
		(recievedAudioControls[freq])[millis_since_epoch]->Refresh();
		frequencyControls[freq]->update();
		dataSizer->Layout();
		mainSizer->Layout();
	}
	
	//and play the audio itself!
	SDL_QueueAudio(audioDevice, audio_data.data(), audio_data.size());
}

void MainFrame::MessageRecieved(const Message& message)
{
	MessageEvent * event = new MessageEvent(NOISE_MESSAGE, message);
	QueueEvent(event);
}

void MainFrame::NodeConnected(uint64_t system)
{
	ConnectionEvent * event = new ConnectionEvent(NOISE_CONNECTION_EVENT, system, true);
	QueueEvent(event);
}

void MainFrame::NodeDisconnected(uint64_t system)
{
	ConnectionEvent * event = new ConnectionEvent(NOISE_CONNECTION_EVENT, system, false);
	QueueEvent(event);
}

void MainFrame::FingerprintVerified(uint64_t system, const Fingerprint& fingerprint)
{
	FingerprintEvent * event = new FingerprintEvent(NOISE_FINGERPRINT_VERIFIED, system, fingerprint);
	QueueEvent(event);
}

	
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
	EVT_MENU(wxID_NETWORK, MainFrame::OnStartNoise)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_NOISE_CONNECTION(wxID_ANY, MainFrame::OnConnectionEvent)
	EVT_NOISE_FINGERPRINT(wxID_ANY, MainFrame::OnFingerprintEvent)
	EVT_NOISE_MESSAGE(wxID_ANY, MainFrame::OnMessageEvent)
wxEND_EVENT_TABLE()
