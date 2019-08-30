OGSS_Bool
UT_RAIDNPDatDecVolCtrl::smallStripeR2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

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
UT_RAIDNPDatDecVolCtrl::largeStripeR2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 10;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 4
		|| sr.at (0) ._idxDevice != 5) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 4
		|| sr.at (1) ._idxDevice != 0) return false;
	if (sr.at (2) ._size != 2 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::fullStripeR2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 16;
	req._volumeAddress = 32;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 4) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 4) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 5) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 8
		|| sr.at (2) ._idxDevice != 0) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 8
		|| sr.at (3) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::twoStripeR2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

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
		|| sr.at (4) ._idxDevice != 5) return false;
	if (sr.at (5) ._size != 2 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 0) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::moreStripeR2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

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
		|| sr.at (2) ._idxDevice != 5) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 4
		|| sr.at (3) ._idxDevice != 0) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 1) return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 2) return false;
	if (sr.at (6) ._size != 4 || sr.at (6) ._deviceAddress != 8
		|| sr.at (6) ._idxDevice != 4) return false;
	if (sr.at (7) ._size != 4 || sr.at (7) ._deviceAddress != 8
		|| sr.at (7) ._idxDevice != 5) return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::smallStripeW2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 2;
	req._volumeAddress = 12;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 6) return false;

	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 3 || sr.at (0) ._type != RQT_READ)
			return false;
	if (sr.at (1) ._size != 2 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 4 || sr.at (1) ._type != RQT_READ)
			return false;
	if (sr.at (2) ._size != 2 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 5 || sr.at (2) ._type != RQT_READ)
			return false;

	if (sr.at (3) ._size != 2 || sr.at (3) ._deviceAddress != 0
		|| sr.at (3) ._idxDevice != 3 || sr.at (3) ._type != RQT_WRITE)
			return false;
	if (sr.at (4) ._size != 2 || sr.at (4) ._deviceAddress != 0
		|| sr.at (4) ._idxDevice != 4 || sr.at (4) ._type != RQT_WRITE)
			return false;
	if (sr.at (5) ._size != 2 || sr.at (5) ._deviceAddress != 0
		|| sr.at (5) ._idxDevice != 5 || sr.at (5) ._type != RQT_WRITE)
			return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::largeStripeW2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 10;
	req._volumeAddress = 16;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 9) return false;

	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 6
		|| sr.at (0) ._idxDevice != 1 || sr.at (0) ._type != RQT_READ)
			return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 4
		|| sr.at (1) ._idxDevice != 2 || sr.at (1) ._type != RQT_READ)
			return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 3 || sr.at (2) ._type != RQT_READ)
			return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 4
		|| sr.at (3) ._idxDevice != 4 || sr.at (3) ._type != RQT_READ)
			return false;

	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 5 || sr.at (4) ._type != RQT_WRITE)
			return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 0 || sr.at (5) ._type != RQT_WRITE)
			return false;
	if (sr.at (6) ._size != 2 || sr.at (6) ._deviceAddress != 4
		|| sr.at (6) ._idxDevice != 1 || sr.at (6) ._type != RQT_WRITE)
			return false;
	if (sr.at (7) ._size != 4 || sr.at (7) ._deviceAddress != 4
		|| sr.at (7) ._idxDevice != 3 || sr.at (7) ._type != RQT_WRITE)
			return false;
	if (sr.at (8) ._size != 4 || sr.at (8) ._deviceAddress != 4
		|| sr.at (8) ._idxDevice != 4 || sr.at (8) ._type != RQT_WRITE)
			return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::fullStripeW2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 16;
	req._volumeAddress = 32;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 6) return false;

	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 4 || sr.at (0) ._type != RQT_WRITE)
			return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 5 || sr.at (1) ._type != RQT_WRITE)
			return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 8
		|| sr.at (2) ._idxDevice != 0 || sr.at (2) ._type != RQT_WRITE)
			return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 8
		|| sr.at (3) ._idxDevice != 1 || sr.at (3) ._type != RQT_WRITE)
			return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 8
		|| sr.at (4) ._idxDevice != 2 || sr.at (4) ._type != RQT_WRITE)
			return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 8
		|| sr.at (5) ._idxDevice != 3 || sr.at (5) ._type != RQT_WRITE)
			return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::twoStripeW2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 20;
	req._volumeAddress = 2;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 17) return false;

	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 0 || sr.at (0) ._type != RQT_READ)
			return false;
	if (sr.at (1) ._size != 2 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 4 || sr.at (1) ._type != RQT_READ)
			return false;
	if (sr.at (2) ._size != 2 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 5 || sr.at (2) ._type != RQT_READ)
			return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 4
		|| sr.at (3) ._idxDevice != 5 || sr.at (3) ._type != RQT_READ)
			return false;
	if (sr.at (4) ._size != 2 || sr.at (4) ._deviceAddress != 4
		|| sr.at (4) ._idxDevice != 0 || sr.at (4) ._type != RQT_READ)
			return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 4
		|| sr.at (5) ._idxDevice != 3 || sr.at (5) ._type != RQT_READ)
			return false;
	if (sr.at (6) ._size != 4 || sr.at (6) ._deviceAddress != 4
		|| sr.at (6) ._idxDevice != 4 || sr.at (6) ._type != RQT_READ)
			return false;

	if (sr.at (7) ._size != 2 || sr.at (7) ._deviceAddress != 2
		|| sr.at (7) ._idxDevice != 0 || sr.at (7) ._type != RQT_WRITE)
			return false;
	if (sr.at (8) ._size != 4 || sr.at (8) ._deviceAddress != 0
		|| sr.at (8) ._idxDevice != 1 || sr.at (8) ._type != RQT_WRITE)
			return false;
	if (sr.at (9) ._size != 4 || sr.at (9) ._deviceAddress != 0
		|| sr.at (9) ._idxDevice != 2 || sr.at (9) ._type != RQT_WRITE)
			return false;
	if (sr.at (10) ._size != 4 || sr.at (10) ._deviceAddress != 0
		|| sr.at (10) ._idxDevice != 3 || sr.at (10) ._type != RQT_WRITE)
			return false;
	if (sr.at (11) ._size != 4 || sr.at (11) ._deviceAddress != 0
		|| sr.at (11) ._idxDevice != 4 || sr.at (11) ._type != RQT_WRITE)
			return false;
	if (sr.at (12) ._size != 4 || sr.at (12) ._deviceAddress != 0
		|| sr.at (12) ._idxDevice != 5 || sr.at (12) ._type != RQT_WRITE)
			return false;
	if (sr.at (13) ._size != 4 || sr.at (13) ._deviceAddress != 4
		|| sr.at (13) ._idxDevice != 5 || sr.at (13) ._type != RQT_WRITE)
			return false;
	if (sr.at (14) ._size != 2 || sr.at (14) ._deviceAddress != 4
		|| sr.at (14) ._idxDevice != 0 || sr.at (14) ._type != RQT_WRITE)
			return false;
	if (sr.at (15) ._size != 4 || sr.at (15) ._deviceAddress != 4
		|| sr.at (15) ._idxDevice != 3 || sr.at (15) ._type != RQT_WRITE)
			return false;
	if (sr.at (16) ._size != 4 || sr.at (16) ._deviceAddress != 4
		|| sr.at (16) ._idxDevice != 4 || sr.at (16) ._type != RQT_WRITE)
			return false;

	return true;
}

OGSS_Bool
UT_RAIDNPDatDecVolCtrl::moreStripeW2PRequest () {
	RAIDNPDatDecVolCtrl		module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 6;
	module._numParity = 2;

	req._size = 26;
	req._volumeAddress = 8;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 20) return false;

	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 2 || sr.at (0) ._type != RQT_READ) 
			return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 3 || sr.at (1) ._type != RQT_READ)
			return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 4 || sr.at (2) ._type != RQT_READ)
			return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 0
		|| sr.at (3) ._idxDevice != 5 || sr.at (3) ._type != RQT_READ)
			return false;
	if (sr.at (4) ._size != 2 || sr.at (4) ._deviceAddress != 8
		|| sr.at (4) ._idxDevice != 4 || sr.at (4) ._type != RQT_READ)
			return false;
	if (sr.at (5) ._size != 2 || sr.at (5) ._deviceAddress != 8
		|| sr.at (5) ._idxDevice != 2 || sr.at (5) ._type != RQT_READ)
			return false;
	if (sr.at (6) ._size != 2 || sr.at (6) ._deviceAddress != 8
		|| sr.at (6) ._idxDevice != 3 || sr.at (6) ._type != RQT_READ)
			return false;

	if (sr.at (7) ._size != 4 || sr.at (7) ._deviceAddress != 0
		|| sr.at (7) ._idxDevice != 2 || sr.at (7) ._type != RQT_WRITE)
			return false;
	if (sr.at (8) ._size != 4 || sr.at (8) ._deviceAddress != 0
		|| sr.at (8) ._idxDevice != 3 || sr.at (8) ._type != RQT_WRITE)
			return false;
	if (sr.at (9) ._size != 4 || sr.at (9) ._deviceAddress != 0
		|| sr.at (9) ._idxDevice != 4 || sr.at (9) ._type != RQT_WRITE)
			return false;
	if (sr.at (10) ._size != 4 || sr.at (10) ._deviceAddress != 0
		|| sr.at (10) ._idxDevice != 5 || sr.at (10) ._type != RQT_WRITE)
			return false;
	if (sr.at (11) ._size != 4 || sr.at (11) ._deviceAddress != 4
		|| sr.at (11) ._idxDevice != 5 || sr.at (11) ._type != RQT_WRITE)
			return false;
	if (sr.at (12) ._size != 4 || sr.at (12) ._deviceAddress != 4
		|| sr.at (12) ._idxDevice != 0 || sr.at (12) ._type != RQT_WRITE)
			return false;
	if (sr.at (13) ._size != 4 || sr.at (13) ._deviceAddress != 4
		|| sr.at (13) ._idxDevice != 1 || sr.at (13) ._type != RQT_WRITE)
			return false;
	if (sr.at (14) ._size != 4 || sr.at (14) ._deviceAddress != 4
		|| sr.at (14) ._idxDevice != 2 || sr.at (14) ._type != RQT_WRITE)
			return false;
	if (sr.at (15) ._size != 4 || sr.at (15) ._deviceAddress != 4
		|| sr.at (15) ._idxDevice != 3 || sr.at (15) ._type != RQT_WRITE)
			return false;
	if (sr.at (16) ._size != 4 || sr.at (16) ._deviceAddress != 4
		|| sr.at (16) ._idxDevice != 4 || sr.at (16) ._type != RQT_WRITE)
			return false;
	if (sr.at (17) ._size != 2 || sr.at (17) ._deviceAddress != 8
		|| sr.at (17) ._idxDevice != 4 || sr.at (17) ._type != RQT_WRITE)
			return false;
	if (sr.at (18) ._size != 2 || sr.at (18) ._deviceAddress != 8
		|| sr.at (18) ._idxDevice != 2 || sr.at (18) ._type != RQT_WRITE)
			return false;
	if (sr.at (19) ._size != 2 || sr.at (19) ._deviceAddress != 8
		|| sr.at (19) ._idxDevice != 3 || sr.at (19) ._type != RQT_WRITE)
			return false;

	return true;
}