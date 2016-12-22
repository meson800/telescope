#pragma once

#include <wx/wxprec.h>
#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame();

private:
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

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
