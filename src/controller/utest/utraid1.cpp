UT_RAID1VolCtrl::UT_RAID1VolCtrl (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_RAID1VolCtrl> (MTP_VOLRAID1) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Middle device request",
				&UT_RAID1VolCtrl::middleDeviceRequest) );
			_tests.push_back (make_pair ("Start device request",
				&UT_RAID1VolCtrl::startDeviceRequest) );
			_tests.push_back (make_pair ("End device request",
				&UT_RAID1VolCtrl::endDeviceRequest) );
			_tests.push_back (make_pair ("Two device request",
				&UT_RAID1VolCtrl::twoDeviceRequest) );
			_tests.push_back (make_pair ("More device request",
				&UT_RAID1VolCtrl::moreDeviceRequest) );
			_tests.push_back (make_pair ("Read requests",
				&UT_RAID1VolCtrl::readRequests) );
			_tests.push_back (make_pair ("Write request",
				&UT_RAID1VolCtrl::writeRequest) );
		} else if (! elt.compare ("middleDev") )
			_tests.push_back (make_pair ("Middle device request",
				&UT_RAID1VolCtrl::middleDeviceRequest) );
		else if (! elt.compare ("startDev") )
			_tests.push_back (make_pair ("Start device request",
				&UT_RAID1VolCtrl::startDeviceRequest) );
		else if (! elt.compare ("endDev") )
			_tests.push_back (make_pair ("End device request",
				&UT_RAID1VolCtrl::endDeviceRequest) );
		else if (! elt.compare ("twoDev") )
			_tests.push_back (make_pair ("Two device request",
				&UT_RAID1VolCtrl::twoDeviceRequest) );
		else if (! elt.compare ("moreDev") )
			_tests.push_back (make_pair ("More device request",
				&UT_RAID1VolCtrl::moreDeviceRequest) );
		else if (! elt.compare ("readRequests") )
			_tests.push_back (make_pair ("Read requests",
				&UT_RAID1VolCtrl::readRequests) );
		else if (! elt.compare ("writeRequest") )
			_tests.push_back (make_pair ("Write request",
				&UT_RAID1VolCtrl::writeRequest) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!";
	}
}

UT_RAID1VolCtrl::~UT_RAID1VolCtrl () {  }

OGSS_Bool
UT_RAID1VolCtrl::middleDeviceRequest () {
	RAID1VolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 4;
	req._volumeAddress = 6;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 6
		|| sr.at (0) ._idxDevice != 0) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::startDeviceRequest () {
	RAID1VolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 12;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 12 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::endDeviceRequest () {
	RAID1VolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 6;
	req._volumeAddress = 42;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 6 || sr.at (0) ._deviceAddress != 10
		|| sr.at (0) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::twoDeviceRequest () {
	RAID1VolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 18;
	req._volumeAddress = 12;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 12
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 14 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::moreDeviceRequest () {
	RAID1VolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 24;
	req._volumeAddress = 12;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 12
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 16 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::readRequests () {
	RAID1VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 4;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 1) return false;

	sr.clear ();
	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 3) return false;

	return true;
}

OGSS_Bool
UT_RAID1VolCtrl::writeRequest () {
	RAID1VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 4;
	req._volumeAddress = 16;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 1) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 3) return false;

	return true;
}
