OGSS_Bool
UT_RAIDNPNoDecVolCtrl::smallStripeR0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 2;
	req._volumeAddress = 12;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 3) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::largeStripeR0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 10;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 4
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 4
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 2 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::fullStripeR0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 16;
	req._volumeAddress = 32;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 4) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 8
		|| sr.at (2) ._idxDevice != 2) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 8
		|| sr.at (3) ._idxDevice != 3) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::twoStripeR0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 20;
	req._volumeAddress = 2;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 6) return false;
	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 2
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 2) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 0
		|| sr.at (3) ._idxDevice != 3) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 0) return false;
	if (sr.at (5) ._size != 2 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::moreStripeR0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 32;
	req._volumeAddress = 8;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 8) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 2) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 3) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 0) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 4
		|| sr.at (3) ._idxDevice != 1) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 2) return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 3) return false;
	if (sr.at (6) ._size != 4 || sr.at (6) ._deviceAddress != 8
		|| sr.at (6) ._idxDevice != 0) return false;
	if (sr.at (7) ._size != 4 || sr.at (7) ._deviceAddress != 8
		|| sr.at (7) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::smallStripeW0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 2;
	req._volumeAddress = 12;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;

	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 3) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::largeStripeW0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 10;
	req._volumeAddress = 16;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 4
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 4
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 2 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::fullStripeW0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 16;
	req._volumeAddress = 32;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 4) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 8
		|| sr.at (2) ._idxDevice != 2) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 8
		|| sr.at (3) ._idxDevice != 3) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::twoStripeW0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 20;
	req._volumeAddress = 2;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 6) return false;
	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 2
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 2) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 0
		|| sr.at (3) ._idxDevice != 3) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 0) return false;
	if (sr.at (5) ._size != 2 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPNoDecVolCtrl::moreStripeW0PRequest () {
	RAIDNPNoDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._numParity = 0;

	req._size = 26;
	req._volumeAddress = 8;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 7) return false;
	
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 2) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 3) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 0) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 4
		|| sr.at (3) ._idxDevice != 1) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 2) return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 3) return false;
	if (sr.at (6) ._size != 2 || sr.at (6) ._deviceAddress != 8
		|| sr.at (6) ._idxDevice != 0) return false;

	return true;
}