#include "NodeControl.h"

#include <sstream>

wxBEGIN_EVENT_TABLE(NodeControl, wxWindow)
	EVT_PAINT(NodeControl::paintEvent)
wxEND_EVENT_TABLE()

NodeControl::NodeControl(wxFrame* parent, uint64_t node_)
	: wxWindow(parent, wxID_ANY)
	, node(node_)
{
	SetMinSize(wxSize(controlWidth, controlHeight));
	std::stringstream tooltipBuilder;
	tooltipBuilder << "System " << node;
	SetToolTip(tooltipBuilder.str().c_str());
}

void NodeControl::paintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void NodeControl::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void NodeControl::render(wxDC& dc)
{
	dc.SetBrush((isVerified? *wxGREEN_BRUSH : *wxYELLOW_BRUSH));
	dc.SetPen(wxNullPen);
	dc.DrawCircle(controlWidth / 2, controlWidth / 2, (controlWidth / 2) - 2);
}

void NodeControl::verifyFingerprint(std::string fingerprint)
{
	verified_fingerprint = fingerprint;
	std::stringstream tooltipBuilder;
	tooltipBuilder << "System " << node << "\nFingerprint:" << verified_fingerprint;
	SetToolTip(tooltipBuilder.str().c_str());

	isVerified = true;
	paintNow();
}
	 
