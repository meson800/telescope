#include "StargazerGUI.h"

#include <iostream>
#include <sstream>

StargazerApp::StargazerApp()
	: guiFrame(nullptr)
{}

StargazerApp::~StargazerApp()
{
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

	//init the connection frame
	connectionWindow = new wxWindow(this, wxID_ANY, wxPoint(0,0), wxSize(400, 100));
	connectionSizer = new wxBoxSizer(wxHORIZONTAL);
	connectionWindow->SetSizer(connectionSizer);
	connectionWindow->Show();
}

MainFrame::~MainFrame()
{
	if (hasStartedNoise)
	{
		noiseInter->writeKeysToFile();
		noiseInter->stopNetworking();
		noiseNetworkingThread.join();
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
	std::cout << "Node " << event.system << (event.isConnected ? " connected" : " disconnected") << "\n";
	if (event.isConnected)
	{
		NodeControl * newNode = new NodeControl(this, event.system);
		connectionSizer->Add(newNode, wxSizerFlags(0).Left().Border(wxALL, 10));
		connectionSizer->Layout();
		connectedNodes[event.system] = newNode;
	}

}

void MainFrame::OnFingerprintEvent(FingerprintEvent & event)
{
	if (connectedNodes.count(event.system))
	{
		connectedNodes[event.system]->verifyFingerprint(event.fingerprint.toString());
	}
	std::cout << "Verfied node " << event.system << " as owning key " << event.fingerprint.toString() << "\n";
}

void MainFrame::OnMessageEvent(MessageEvent & event)
{
	std::cout << "Recieved Noise Message\n";
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
