#include <wx/wx.h>

#include <string>

class NodeControl : public wxWindow
{
	static const int controlWidth = 50;
	static const int controlHeight = 50;

public:
	NodeControl(wxFrame* parent, uint64_t node_);

	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	void render(wxDC& dc);
	void verifyFingerprint(std::string fingerprint);

	wxDECLARE_EVENT_TABLE();

private:
	uint64_t node;
	std::string tooltip;
	std::string verified_fingerprint;
	bool isVerified = false;

};
