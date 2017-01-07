#pragma once

#include <wx/event.h>

#include <noise/Fingerprint.h>
#include <noise/Message.h>

class ConnectionEvent : public wxCommandEvent
{
public:
	ConnectionEvent(wxEventType eventType, uint64_t system_, bool isConnected_);
	wxEvent* Clone() const {return new ConnectionEvent(*this); }
	uint64_t system;
	bool isConnected;
};

class FingerprintEvent : public wxCommandEvent
{
public:
	FingerprintEvent(wxEventType eventType, uint64_t system_, Fingerprint fingerprint_);
	wxEvent* Clone() const {return new FingerprintEvent(*this); }
	uint64_t system;
	Fingerprint fingerprint;
};

class MessageEvent : public wxCommandEvent
{
public:
	MessageEvent(wxEventType eventType, Message message_);
	wxEvent* Clone() const {return new MessageEvent(*this); }
	Message message;
};

wxDECLARE_EVENT(NOISE_CONNECTION_EVENT, ConnectionEvent);
wxDECLARE_EVENT(NOISE_FINGERPRINT_VERIFIED, FingerprintEvent);
wxDECLARE_EVENT(NOISE_MESSAGE, MessageEvent);

typedef void (wxEvtHandler::*NoiseConnectionEventFunction)(ConnectionEvent &);
typedef void (wxEvtHandler::*NoiseFingerprintEventFunction)(FingerprintEvent &);
typedef void (wxEvtHandler::*NoiseMessageEventFunction)(MessageEvent &);

#define NoiseConnectionEventHandler(func) wxEVENT_HANDLER_CAST(NoiseConnectionEventFunction, func)
#define NoiseFingerprintEventHandler(func) wxEVENT_HANDLER_CAST(NoiseFingerprintEventFunction, func)
#define NoiseMessageEventHandler(func) wxEVENT_HANDLER_CAST(NoiseMessageEventFunction, func)

#define EVT_NOISE_CONNECTION(id, func) wx__DECLARE_EVT1(NOISE_CONNECTION_EVENT, id, NoiseConnectionEventHandler(func))
#define EVT_NOISE_FINGERPRINT(id, func) wx__DECLARE_EVT1(NOISE_FINGERPRINT_VERIFIED, id, NoiseFingerprintEventHandler(func))
#define EVT_NOISE_MESSAGE(id, func) wx__DECLARE_EVT1(NOISE_MESSAGE, id, NoiseMessageEventHandler(func))
