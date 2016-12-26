#include "StargazerGUI.h"

#include <iostream>

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
{
	fileMenu = new wxMenu;
	helpMenu = new wxMenu;

	fileMenu->Append(wxID_EXIT);
	helpMenu->Append(wxID_ABOUT);

	topMenu = new wxMenuBar;
	topMenu->Append(fileMenu, "&File");
	topMenu->Append(helpMenu, "&Help");

	SetMenuBar(topMenu);
	CreateStatusBar();
	SetStatusText("wxWidgets test");
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("Telescope Stargazing Node\nCopyright 2016 Christopher Johnstone", "About Telescope", wxOK | wxICON_INFORMATION);
}
	
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()
