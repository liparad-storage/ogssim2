#include "controller/volumecontroller.hpp"

void
VolumeController::updateScheme (
	const Request						& event) {
	switch (event._type) {
		case RQT_EVFLT:
			_deviceState [event._idxDevice] ._failureDate = event._date;
			break;
		case RQT_EVRPL:
			_deviceState [event._idxDevice] ._renewalDate = event._date;
			break;
		default:;
	}
}
