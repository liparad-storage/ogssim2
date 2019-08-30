void
createTestHardware (
	Volume					& decRAID,
	Device					& dev,
	vector <Volume>			& subVols) {
	decRAID._decraidScheme = DRD_SD2S;
	decRAID._suSize = 1024;
	decRAID._numDevices = 20;
	decRAID._numRedundancyDevices = 4;

	dev._physicalCapacity = 32*1024;

	subVols.push_back (Volume () );
		subVols.back () ._type = VTP_JBOD;
		subVols.back () ._numDevices = 4;
	subVols.push_back (Volume () );
		subVols.back () ._type = VTP_RAID1;
		subVols.back () ._numDevices = 4;
	subVols.push_back (Volume () );
		subVols.back () ._type = VTP_RAID01;
		subVols.back () ._numDevices = 4;
		subVols.back () ._suSize = 1024;
	subVols.push_back (Volume () );
		subVols.back () ._type = VTP_RAIDNP;
		subVols.back () ._numDevices = 4;
		subVols.back () ._numRedundancyDevices = 1;
		subVols.back () ._declustering = DCL_OFF;
}

UT_DecRAIDVolCtrl::UT_DecRAIDVolCtrl (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_DecRAIDVolCtrl> (MTP_DECRAIDCTRL) {
	_tests.push_back (make_pair ("Generation after failure",
		&UT_DecRAIDVolCtrl::manage1Failure) );
	_tests.push_back (make_pair ("Generation after renewal",
		&UT_DecRAIDVolCtrl::manage1Renewal) );
}

UT_DecRAIDVolCtrl::~UT_DecRAIDVolCtrl () {  }

OGSS_Bool
UT_DecRAIDVolCtrl::manage1Failure () {
	Volume					decRAID;
	Device					dev;
	vector <Volume>			subVols;
	vector <Request>		subrequests;

	Request					evFailure;
		evFailure._type = RQT_EVFLT;
		evFailure._idxDevice = 0;
		evFailure._date = 10.;
		evFailure._mainIdx = OGSS_ULONG_MAX;
		evFailure._majrIdx = 1;
		evFailure._minrIdx = 0;

	createTestHardware (decRAID, dev, subVols);

	DecRAIDVolCtrl			ctrl {decRAID, dev, subVols};

	ctrl._generateRequestsFromFailure (evFailure, subrequests);

	cout << "date\tiDev\tstrp\t@Dev\tsize\tprio\tchld" << endl;

	for (auto elt: subrequests) {
		cout << elt._date << "\t" << elt._idxDevice << "\t"
			<< elt._deviceAddress / ctrl._numBytesBySU << "\t" << elt._deviceAddress << "\t"
			<< elt._size << "\t" << elt._numPrioChild << "\t" << elt._numChild << endl;
	}

	return false;
}

OGSS_Bool
UT_DecRAIDVolCtrl::manage1Renewal () {
	Volume					decRAID;
	Device					dev;
	vector <Volume>			subVols;
	vector <Request>		subrequests;

	Request					evFailure;
		evFailure._type = RQT_EVFLT;
		evFailure._idxDevice = 0;
		evFailure._date = 1.;
		evFailure._mainIdx = OGSS_ULONG_MAX;
		evFailure._majrIdx = 1;
		evFailure._minrIdx = 0;

	Request					evRenewal;
		evRenewal._type = RQT_EVRPL;
		evRenewal._idxDevice = 0;
		evRenewal._date = 10.;
		evRenewal._mainIdx = OGSS_ULONG_MAX;
		evRenewal._majrIdx = 2;
		evRenewal._minrIdx = 0;

	createTestHardware (decRAID, dev, subVols);

	DecRAIDVolCtrl			ctrl {decRAID, dev, subVols};

	ctrl._generateRequestsFromFailure (evRenewal, subrequests);
	subrequests.clear ();

	ctrl._generateRequestsFromRenewal (evRenewal, subrequests);

	cout << "date\tiDev\tstrp\t@Dev\tsize\tprio\tchld" << endl;

	for (auto elt: subrequests) {
		cout << elt._date << "\t" << elt._idxDevice << "\t"
			<< elt._deviceAddress / ctrl._numBytesBySU << "\t" << elt._deviceAddress << "\t"
			<< elt._size << "\t" << elt._numPrioChild << "\t" << elt._numChild << endl;
	}

	return false;
}
