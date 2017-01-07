#include "NoiseEvents.h"

ConnectionEvent::ConnectionEvent(wxEventType eventType, uint64_t system_, bool isConnected_)
	: wxCommandEvent(eventType)
	, system(system_)
	, isConnected(isConnected_)
{}

FingerprintEvent::FingerprintEvent(wxEventType eventType, uint64_t system_, Fingerprint fingerprint_)
	: wxCommandEvent(eventType)
	, system(system_)
	, fingerprint(fingerprint_)
{}

MessageEvent::MessageEvent(wxEventType eventType, Message message_)
	: wxCommandEvent(eventType)
	, message(message_)
{}

wxDEFINE_EVENT(NOISE_CONNECTION_EVENT, ConnectionEvent);
wxDEFINE_EVENT(NOISE_FINGERPRINT_VERIFIED, FingerprintEvent);
wxDEFINE_EVENT(NOISE_MESSAGE, MessageEvent);
