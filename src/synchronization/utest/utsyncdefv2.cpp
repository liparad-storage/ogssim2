#include <cmath>

UT_SyncDefV2::UT_SyncDefV2 (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_SyncDefV2> (MTP_SYNCDEFV2) {
	_tests.push_back (make_pair ("1 request",
		&UT_SyncDefV2::oneRequest) );
	_tests.push_back (make_pair ("2 requests",
		&UT_SyncDefV2::twoRequests) );
	_tests.push_back (make_pair ("2 requests -- small lambda",
		&UT_SyncDefV2::twoRequests_smallLambda) );
	_tests.push_back (make_pair ("2 requests -- huge lambda",
		&UT_SyncDefV2::twoRequests_hugeLambda) );
	_tests.push_back (make_pair ("2 requests -- 2 volumes",
		&UT_SyncDefV2::twoRequests_twoVolumes) );
	_tests.push_back (make_pair ("3 requests -- in RAIDNP",
		&UT_SyncDefV2::threeRequests_RAIDNP) );
/*	_tests.push_back (make_pair ("4 requests -- user/system",
		&UT_SyncDefV2::multiRequests_system) );
	_tests.push_back (make_pair ("2 requests before failure recovery",
		&UT_SyncDefV2::failure2ReqBRecovery) );
	_tests.push_back (make_pair ("2 requests after failure recovery",
		&UT_SyncDefV2::failure2ReqARecovery) );
	_tests.push_back (make_pair ("2 requests after renewal before failure recovery",
		&UT_SyncDefV2::renewal2ReqBRecovery) );
	_tests.push_back (make_pair ("2 requests after renewal after failure recovery",
		&UT_SyncDefV2::renewal2ReqARecovery) );
	_tests.push_back (make_pair ("2 requests after renewal after renewal copy",
		&UT_SyncDefV2::renewal2ReqACopy) );
*/}

UT_SyncDefV2::~UT_SyncDefV2 () {  }

OGSS_Bool
UT_SyncDefV2::oneRequest () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 1; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.front () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.front () ._interface = 2; vV.front () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.front () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = r._transferTimeB1 = .1;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	r._mainIdx = 1;				sync.addEntry (r);
	Request r1 {r};
	r1._majrIdx = 1;			sync.addEntry (r1);
	Request r2 {r1};			r2._numChild = 0;
	r2._minrIdx = 1;			sync.addEntry (r2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 17.2)	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::twoRequests () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 1; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.front () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.front () ._interface = 2; vV.front () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.front () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = r._transferTimeB1 = .1;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	Request					s {r};						s._date = 25.;

	r._mainIdx = 1;				sync.addEntry (r);
	s._mainIdx = 2;				sync.addEntry (s);
	Request r1 {r}, s1 {s};
	r1._majrIdx = 1;			sync.addEntry (r1);
	s1._majrIdx = 1;			sync.addEntry (s1);
	Request r2 {r1}, s2 {s1};	r2._numChild = 0;		s2._numChild = 0;
	r2._minrIdx = 1;			sync.addEntry (r2);
	s2._minrIdx = 1;			sync.addEntry (s2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 17.2)	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (2, 0, 0)][SyncDefV2::TO_TIR])
		- 17.2)	< numeric_limits <float> ::epsilon ())
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::twoRequests_smallLambda () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 1; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.front () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.front () ._interface = 2; vV.front () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.front () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = r._transferTimeB1 = .1;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	Request					s {r};						s._date = 10.;

	r._mainIdx = 1;				sync.addEntry (r);
	s._mainIdx = 2;				sync.addEntry (s);
	Request r1 {r}, s1 {s};
	r1._majrIdx = 1;			sync.addEntry (r1);
	s1._majrIdx = 1;			sync.addEntry (s1);
	Request r2 {r1}, s2 {s1};	r2._numChild = 0;		s2._numChild = 0;
	r2._minrIdx = 1;			sync.addEntry (r2);
	s2._minrIdx = 1;			sync.addEntry (s2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 17.2)	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (2, 0, 0)][SyncDefV2::TO_TIR])
		- 22.2)	< numeric_limits <float> ::epsilon ())
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::twoRequests_hugeLambda () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 1; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.front () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.front () ._interface = 2; vV.front () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.front () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = r._transferTimeB1 = .1;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	Request					s {r};						s._date = 5.05;

	r._mainIdx = 1;				sync.addEntry (r);
	s._mainIdx = 2;				sync.addEntry (s);
	Request r1 {r}, s1 {s};
	r1._majrIdx = 1;			sync.addEntry (r1);
	s1._majrIdx = 1;			sync.addEntry (s1);
	Request r2 {r1}, s2 {s1};	r2._numChild = 0;		s2._numChild = 0;
	r2._minrIdx = 1;			sync.addEntry (r2);
	s2._minrIdx = 1;			sync.addEntry (s2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 17.2)	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (2, 0, 0)][SyncDefV2::TO_TIR])
		- 27.15) < numeric_limits <float> ::epsilon ())
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::twoRequests_twoVolumes () {
	HardwareParameters		hp;
		hp._numInterfaces = 4; hp._numTiers = 1; hp._numVolumes = 2;
		hp._numDevices = 2; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2; vV.back () ._parent = 0;
		vV.push_back (Volume () ); vV.back () ._interface = 3; vV.back () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 1;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = .1; r._transferTimeB1 = .3;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	Request					s {r};						s._date = 5.05;
	s._idxVolume = 1;		s._idxDevice = 1;

	r._mainIdx = 1;				sync.addEntry (r);
	s._mainIdx = 2;				sync.addEntry (s);
	Request r1 {r}, s1 {s};
	r1._majrIdx = 1;			sync.addEntry (r1);
	s1._majrIdx = 1;			sync.addEntry (s1);
	Request r2 {r1}, s2 {s1};	r2._numChild = 0;		s2._numChild = 0;
	r2._minrIdx = 1;			sync.addEntry (r2);
	s2._minrIdx = 1;			sync.addEntry (s2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 17.4)	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (2, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (2, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (2, 0, 0)][SyncDefV2::TO_TIR])
		- 17.85) < numeric_limits <float> ::epsilon ())
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::threeRequests_RAIDNP () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2; vV.back () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.};

	r._serviceTime = 10.; r._idxVolume = 0; r._idxDevice = 0;
	r._numChild = 1;
	r._transferTimeA1 = r._transferTimeB1 = .1;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = 3.;

	r._mainIdx = 1;				sync.addEntry (r);
	Request r1 {r};				r1._numChild = 3; 		r1._numPrioChild = 2;
	r1._majrIdx = 1;			sync.addEntry (r1);
	Request r2 {r1}, s2 {r1}, t2 {r1};
	r2._numChild = 0;			s2._numChild = 0;		t2._numChild = 0;
	r2._numPrioChild = 1;		s2._numPrioChild = 0;	t2._numPrioChild = 1;
	r2._minrIdx = 1;			sync.addEntry (r2);
	s2._minrIdx = 2;			sync.addEntry (s2);
	t2._minrIdx = 3;			sync.addEntry (t2);

	sync.process ();

	if (fabs (sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    - sync._rslt [make_tuple (1, 0, 0)][SyncDefV2::ARRIVL]
		    -(sync._data [make_tuple (1, 0, 0)][SyncDefV2::FM_TIR]
		    + sync._data [make_tuple (1, 0, 0)][SyncDefV2::TO_TIR])
		- 43.2)	< numeric_limits <float> ::epsilon ())
		return true;

	return false;
}
/*
OGSS_Bool
UT_SyncDefV2::multiRequests_system () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 4; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2; vV.back () ._parent = 0;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					r {5.}, s {10.}, t {15.}, u {20.};

	r._mainIdx = 0;
	r._serviceTime = 10.; r._idxVolume = 0;
	r._idxDevice = 0; 	r._numChild = 1;
	r._transferTimeA1 = 20; r._transferTimeB1 = .5;
	r._transferTimeA2 = r._transferTimeB2 = .5;
	r._transferTimeA3 = r._transferTimeB3 = .5;

	s._mainIdx = 1;		s._system = true;
	s._serviceTime = 5.; s._idxVolume = 0;
	s._idxDevice = 1; 	s._numChild = 1;
	s._transferTimeA1 = s._transferTimeB1 = .5;
	s._transferTimeA2 = s._transferTimeB2 = .5;
	s._transferTimeA3 = s._transferTimeB3 = .5;

	t._mainIdx = 2;		t._system = true;
	t._serviceTime = 8.5; t._idxVolume = 0;
	t._idxDevice = 2; 	t._numChild = 1;
	t._transferTimeA1 = t._transferTimeB1 = .5;
	t._transferTimeA2 = t._transferTimeB2 = .5;
	t._transferTimeA3 = t._transferTimeB3 = .5;

	u._mainIdx = 3;
	u._serviceTime = 10.; u._idxVolume = 0;
	u._idxDevice = 3; 	u._numChild = 1;
	u._transferTimeA1 = u._transferTimeB1 = .5;
	u._transferTimeA2 = u._transferTimeB2 = .5;
	u._transferTimeA3 = u._transferTimeB3 = .5;

	sync.addEntry (r); sync.addEntry (s); sync.addEntry (t); sync.addEntry (u);

	Request r1 {r}, s1 {s}, t1 {t}, u1 {u};
	r1._majrIdx = 1;			s1._majrIdx = 1;
	t1._majrIdx = 1;			u1._majrIdx = 1;
	sync.addEntry (r1);sync.addEntry (s1);sync.addEntry (t1);sync.addEntry (u1);

	Request r2 {r1}, s2 {s1}, t2 {t1}, u2 {u1};
	r2._minrIdx = 1;			s2._minrIdx = 1;
	t2._minrIdx = 1;			u2._minrIdx = 1;
	sync.addEntry (r2);sync.addEntry (s2);sync.addEntry (t2);sync.addEntry (u2);

	sync.process ();

	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 32.5)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 23.5)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (2, 0, 0) ) ._responseTime - 23.5)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (3, 0, 0) ) ._responseTime - 18)
		< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::failure2ReqBRecovery () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2;
			vV.back () ._parent = 0; vV.back () ._type = VTP_DECRAID;
			vV.back () ._suSize = 4;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					rR {5.}, rW {5.}, s {10.};

	rR._mainIdx = 0;		rR._multiple = true;	rR._operation = ROP_NATIVE;
	rR._serviceTime = 5.;	rR._idxVolume = 0;
	rR._idxDevice = 0;		rR._numChild = 1;
	rR._transferTimeA1 = 	rR._transferTimeB1 = .0;
	rR._transferTimeA2 = 	rR._transferTimeB2 = .0;
	rR._transferTimeA3 = 	rR._transferTimeB3 = .0;
	rR._deviceAddress = 0; 	rR._size = 4;

	rW._mainIdx = 1;		rW._multiple = true;	rW._operation = ROP_NATIVE;
	rW._serviceTime = 5.;	rW._idxVolume = 0;
	rW._idxDevice = 0;		rW._numChild = 1;
	rW._transferTimeA1 =	rW._transferTimeB1 = .0;
	rW._transferTimeA2 =	rW._transferTimeB2 = .0;
	rW._transferTimeA3 =	rW._transferTimeB3 = .0;
	rW._deviceAddress = 4;	rW._size = 4;

	s._mainIdx = OGSS_ULONG_MAX;	s._system = true;		s._type = RQT_FKFLT;
	s._majrIdx = 1;					s._minrIdx = 0;
	s._serviceTime = 5.;			s._idxVolume = 0;
	s._idxDevice = 0;				s._numChild = 4;		s._numPrioChild = 2;
	s._transferTimeA1 =				s._transferTimeB1 = .0;
	s._transferTimeA2 =				s._transferTimeB2 = .0;
	s._transferTimeA3 =				s._transferTimeB3 = .0;
	s._deviceAddress = 0;			s._size = 4;
	sync.addEntry (rR); sync.addEntry (rW); sync.addEntry (s);

	Request rR1 {rR}, rW1 {rW}, s1 {s};
	rR1._majrIdx = 1;	rR1._numPrioChild = 1;	rR1._numChild = 4;
	rW1._majrIdx = 1;	rW1._numPrioChild = 0;	rW1._numChild = 5;
	s1._minrIdx = 1;	s1._idxDevice = 1;		s1._numPrioChild = 1;	s1._numLink = 0;
	sync.addEntry (rR1); sync.addEntry (rW1); sync.addEntry (s1);

	Request rR2 {rR1}, rW2 {rW1}, s2 {s1};
	rR2._minrIdx = 1;	rR2._numChild = 0;		rR2._idxDevice = 0;		rR2._numPrioChild = 0;
	rW2._minrIdx = 1;	rW2._numChild = 0;		rW2._idxDevice = 0;		rW2._numPrioChild = 0;
	s2._minrIdx = 2;	s2._idxDevice = 2;		s2._numPrioChild = 0;
	sync.addEntry (rR2); sync.addEntry (rW2); sync.addEntry (s2);

	Request s3 {s2};
	s3._minrIdx = 3;	s3._idxDevice = 1;		s3._numPrioChild = 1; 	s3._deviceAddress = 4;	s3._numLink = 1;
	sync.addEntry (s3);

	Request s4 {s3};
	s4._minrIdx = 4;	s4._idxDevice = 2;		s4._numPrioChild = 0;
	sync.addEntry (s4);

	Request rR5 {rR2}, rW5 {rW2};
	rR5._minrIdx = 2;	rR5._numChild = 0;		rR5._idxDevice = 2;		rR5._operation = ROP_REDIRECTED;
	rW5._minrIdx = 2;	rW5._numChild = 0;		rW5._idxDevice = 1;		rW5._operation = ROP_REDIRECTED;
	sync.addEntry (rR5); sync.addEntry (rW5);

	Request rW6 {rW5};
	rW6._minrIdx = 3;	rW6._numPrioChild = 0;	rW6._idxDevice = 2;
	sync.addEntry (rW6);

	Request rR9 {rR5}, rW9 {rW6};
	rR9._minrIdx = 3;	rR9._idxDevice = 1;		rR9._numPrioChild = 1;	rR9._operation = ROP_RECOVER;
	rW9._minrIdx = 4;	rW9._idxDevice = 1;		rW9._operation = ROP_RECOVER;
	sync.addEntry (rR9); sync.addEntry (rW9);

	Request rR10 {rR9}, rW10 {rW9};
	rR10._minrIdx = 4;	rR10._idxDevice = 2;	rR10._numPrioChild = 0;
	rW10._minrIdx = 5;	rW10._idxDevice = 2;
	sync.addEntry (rR10); sync.addEntry (rW10);

	sync.process ();

	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 10)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 10)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 1) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 2) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 3) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 4) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::failure2ReqARecovery () {
		HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2;
			vV.back () ._parent = 0; vV.back () ._type = VTP_DECRAID;
			vV.back () ._suSize = 4;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					rR {30.}, rW {30.}, s {10.};

	rR._mainIdx = 0;		rR._multiple = true;	rR._operation = ROP_NATIVE;
	rR._serviceTime = 5.;	rR._idxVolume = 0;
	rR._idxDevice = 0;		rR._numChild = 1;
	rR._transferTimeA1 = 	rR._transferTimeB1 = .0;
	rR._transferTimeA2 = 	rR._transferTimeB2 = .0;
	rR._transferTimeA3 = 	rR._transferTimeB3 = .0;
	rR._deviceAddress = 0; 	rR._size = 4;

	rW._mainIdx = 1;		rW._multiple = true;	rW._operation = ROP_NATIVE;
	rW._serviceTime = 5.;	rW._idxVolume = 0;
	rW._idxDevice = 0;		rW._numChild = 1;
	rW._transferTimeA1 =	rW._transferTimeB1 = .0;
	rW._transferTimeA2 =	rW._transferTimeB2 = .0;
	rW._transferTimeA3 =	rW._transferTimeB3 = .0;
	rW._deviceAddress = 4;	rW._size = 4;

	s._mainIdx = OGSS_ULONG_MAX;	s._system = true;		s._type = RQT_FKFLT;
	s._majrIdx = 1;					s._minrIdx = 0;
	s._serviceTime = 5.;			s._idxVolume = 0;
	s._idxDevice = 0;				s._numChild = 4;		s._numPrioChild = 2;
	s._transferTimeA1 =				s._transferTimeB1 = .0;
	s._transferTimeA2 =				s._transferTimeB2 = .0;
	s._transferTimeA3 =				s._transferTimeB3 = .0;
	s._deviceAddress = 0;			s._size = 4;
	sync.addEntry (rR); sync.addEntry (rW); sync.addEntry (s);

	Request rR1 {rR}, rW1 {rW}, s1 {s};
	rR1._majrIdx = 1;	rR1._numPrioChild = 1;	rR1._numChild = 4;
	rW1._majrIdx = 1;	rW1._numPrioChild = 0;	rW1._numChild = 5;
	s1._minrIdx = 1;	s1._idxDevice = 1;		s1._numPrioChild = 1;	s1._numLink = 0;
	sync.addEntry (rR1); sync.addEntry (rW1); sync.addEntry (s1);

	Request rR2 {rR1}, rW2 {rW1}, s2 {s1};
	rR2._minrIdx = 1;	rR2._numChild = 0;		rR2._idxDevice = 0;		rR2._numPrioChild = 0;
	rW2._minrIdx = 1;	rW2._numChild = 0;		rW2._idxDevice = 0;		rW2._numPrioChild = 0;
	s2._minrIdx = 2;	s2._idxDevice = 2;		s2._numPrioChild = 0;
	sync.addEntry (rR2); sync.addEntry (rW2); sync.addEntry (s2);

	Request s3 {s2};
	s3._minrIdx = 3;	s3._idxDevice = 1;		s3._numPrioChild = 1; 	s3._deviceAddress = 4;	s3._numLink = 1;
	sync.addEntry (s3);

	Request s4 {s3};
	s4._minrIdx = 4;	s4._idxDevice = 2;		s4._numPrioChild = 0;
	sync.addEntry (s4);

	Request rR5 {rR2}, rW5 {rW2};
	rR5._minrIdx = 2;	rR5._numChild = 0;		rR5._idxDevice = 2;		rR5._operation = ROP_REDIRECTED;
	rW5._minrIdx = 2;	rW5._numChild = 0;		rW5._idxDevice = 1;		rW5._operation = ROP_REDIRECTED;
	sync.addEntry (rR5); sync.addEntry (rW5);

	Request rW6 {rW5};
	rW6._minrIdx = 3;	rW6._numPrioChild = 0;	rW6._idxDevice = 2;
	sync.addEntry (rW6);

	Request rR9 {rR5}, rW9 {rW6};
	rR9._minrIdx = 3;	rR9._idxDevice = 1;		rR9._numPrioChild = 1;	rR9._operation = ROP_RECOVER;
	rW9._minrIdx = 4;	rW9._idxDevice = 1;		rW9._operation = ROP_RECOVER;
	sync.addEntry (rR9); sync.addEntry (rW9);

	Request rR10 {rR9}, rW10 {rW9};
	rR10._minrIdx = 4;	rR10._idxDevice = 2;	rR10._numPrioChild = 0;
	rW10._minrIdx = 5;	rW10._idxDevice = 2;
	sync.addEntry (rR10); sync.addEntry (rW10);

	sync.process ();

	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 5.)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 10.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 1) ) ._responseTime - 5.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 2) ) ._responseTime - 10.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 3) ) ._responseTime - 10.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 4) ) ._responseTime - 15.)
	 	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::renewal2ReqBRecovery () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2;
			vV.back () ._parent = 0; vV.back () ._type = VTP_DECRAID;
			vV.back () ._suSize = 4;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					rR {15.}, rW {15.}, s {40.}, t {OGSS_REAL_MAX};

	rR._mainIdx = 0;		rR._multiple = true;	rR._operation = ROP_NATIVE;
	rR._serviceTime = 5.;	rR._idxVolume = 0;
	rR._idxDevice = 0;		rR._numChild = 1;
	rR._transferTimeA1 = 	rR._transferTimeB1 = .0;
	rR._transferTimeA2 = 	rR._transferTimeB2 = .0;
	rR._transferTimeA3 = 	rR._transferTimeB3 = .0;
	rR._deviceAddress = 0; 	rR._size = 4;

	rW._mainIdx = 1;		rW._multiple = true;	rW._operation = ROP_NATIVE;
	rW._serviceTime = 5.;	rW._idxVolume = 0;
	rW._idxDevice = 0;		rW._numChild = 1;
	rW._transferTimeA1 =	rW._transferTimeB1 = .0;
	rW._transferTimeA2 =	rW._transferTimeB2 = .0;
	rW._transferTimeA3 =	rW._transferTimeB3 = .0;
	rW._deviceAddress = 4;	rW._size = 4;

	s._mainIdx = OGSS_ULONG_MAX;	s._system = true;		s._type = RQT_FKFLT;
	s._majrIdx = 1;					s._minrIdx = 0;
	s._serviceTime = 5.;			s._idxVolume = 0;
	s._idxDevice = 0;				s._numChild = 4;		s._numPrioChild = 2;
	s._transferTimeA1 =				s._transferTimeB1 = .0;
	s._transferTimeA2 =				s._transferTimeB2 = .0;
	s._transferTimeA3 =				s._transferTimeB3 = .0;
	s._deviceAddress = 0;			s._size = 4;

	t._mainIdx = OGSS_ULONG_MAX;	t._system = true;		t._type = RQT_FKRPL;
	t._majrIdx = 2;					t._minrIdx = 0;
	t._serviceTime = 5.;			t._idxVolume = 0;
	t._idxDevice = 0;				t._numChild = 4;		t._numPrioChild = 2;
	t._transferTimeA1 =				t._transferTimeB1 = .0;
	t._transferTimeA2 =				t._transferTimeB2 = .0;
	t._transferTimeA3 =				t._transferTimeB3 = .0;
	t._deviceAddress = 0;			t._size = 4;
	sync.addEntry (rR); sync.addEntry (rW); sync.addEntry (s); sync.addEntry (t);

	Request rR1 {rR}, rW1 {rW}, s1 {s}, t1 {t};
	rR1._majrIdx = 1;	rR1._numPrioChild = 2;	rR1._numChild = 6;
	rW1._majrIdx = 1;	rW1._numPrioChild = 0;	rW1._numChild = 7;
	s1._minrIdx = 1;	s1._idxDevice = 1;		s1._numPrioChild = 1;	s1._numLink = 0;
	t1._minrIdx = 1;	t1._idxDevice = 2;		t1._numPrioChild = 1;	t1._numLink = 0;
	sync.addEntry (rR1); sync.addEntry (rW1); sync.addEntry (s1); sync.addEntry (t1);

	Request rR2 {rR1}, rW2 {rW1}, s2 {s1}, t2 {t1};
	rR2._minrIdx = 1;	rR2._numChild = 0;		rR2._idxDevice = 0;		rR2._numPrioChild = 0;
	rW2._minrIdx = 1;	rW2._numChild = 0;		rW2._idxDevice = 0;		rW2._numPrioChild = 0;
	s2._minrIdx = 2;	s2._idxDevice = 2;		s2._numPrioChild = 0;
	t2._minrIdx = 2;	t2._idxDevice = 0;		t2._numPrioChild = 0;
	sync.addEntry (rR2); sync.addEntry (rW2); sync.addEntry (s2); sync.addEntry (t2);

	Request s3 {s2}, t3 {t2};
	s3._minrIdx = 3;	s3._idxDevice = 1;		s3._numPrioChild = 1; 	s3._deviceAddress = 4;	s3._numLink = 1;
	t3._minrIdx = 3;	t3._idxDevice = 2;		t3._numPrioChild = 1;	t3._deviceAddress = 4;	t3._numLink = 1;
	sync.addEntry (s3); sync.addEntry (t3);

	Request s4 {s3}, t4 {t3};
	s4._minrIdx = 4;	s4._idxDevice = 2;		s4._numPrioChild = 0;
	t4._minrIdx = 4;	t4._idxDevice = 0;		t4._numPrioChild = 0;
	sync.addEntry (s4); sync.addEntry (t4);

	Request rR5 {rR2}, rW5 {rW2};
	rR5._minrIdx = 2;	rR5._numChild = 0;		rR5._idxDevice = 0;		rR5._operation = ROP_REDIRECTED;
	rW5._minrIdx = 2;	rW5._numChild = 0;		rW5._idxDevice = 0;		rW5._operation = ROP_REDIRECTED;
	sync.addEntry (rR5); sync.addEntry (rW5);

	Request rW6 {rW5};
	rW6._minrIdx = 3;	rW6._numPrioChild = 0;	rW6._idxDevice = 1;
	sync.addEntry (rW6);

	Request rR9 {rR5}, rW9 {rW6};
	rR9._minrIdx = 3;	rR9._idxDevice = 1;		rR9._numPrioChild = 1;	rR9._operation = ROP_RECOVER;
	rW9._minrIdx = 4;	rW9._idxDevice = 1;		rW9._operation = ROP_RECOVER;
	sync.addEntry (rR9); sync.addEntry (rW9);

	Request rR10 {rR9}, rW10 {rW9};
	rR10._minrIdx = 4;	rR10._idxDevice = 0;	rR10._numPrioChild = 0;
	rW10._minrIdx = 5;	rW10._idxDevice = 0;
	sync.addEntry (rR10); sync.addEntry (rW10);

	Request rR13 {rR10}, rW13 {rW10};
	rR13._minrIdx = 5;	rR13._idxDevice = 2;	rR13._numPrioChild = 1;	rR13._operation = ROP_COPY;
	rW13._minrIdx = 6;	rW13._idxDevice = 0;	rW13._numPrioChild = 0; rW13._operation = ROP_COPY;
	sync.addEntry (rR13); sync.addEntry (rW13);

	Request rR14 {rR13}, rW14 {rW13};
	rR14._minrIdx = 6;	rR14._idxDevice = 0;	rR14._numPrioChild = 0;
	rW14._minrIdx = 7;	rW14._idxDevice = 1;
	sync.addEntry (rR14); sync.addEntry (rW14);

	sync.process ();

	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 10)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 10)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 1) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 2) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 3) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 4) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 1) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 2) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 3) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 4) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::renewal2ReqARecovery () {
		HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2;
			vV.back () ._parent = 0; vV.back () ._type = VTP_DECRAID;
			vV.back () ._suSize = 4;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;
		vD.push_back (Device () ); vD.back () ._parent = 0;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					rR {20.}, rW {20.}, s {5.}, t {OGSS_REAL_MAX};

	rR._mainIdx = 0;		rR._multiple = true;	rR._operation = ROP_NATIVE;
	rR._serviceTime = 5.;	rR._idxVolume = 0;
	rR._idxDevice = 0;		rR._numChild = 1;
	rR._transferTimeA1 = 	rR._transferTimeB1 = .0;
	rR._transferTimeA2 = 	rR._transferTimeB2 = .0;
	rR._transferTimeA3 = 	rR._transferTimeB3 = .0;
	rR._deviceAddress = 0; 	rR._size = 4;

	rW._mainIdx = 1;		rW._multiple = true;	rW._operation = ROP_NATIVE;
	rW._serviceTime = 5.;	rW._idxVolume = 0;
	rW._idxDevice = 0;		rW._numChild = 1;
	rW._transferTimeA1 =	rW._transferTimeB1 = .0;
	rW._transferTimeA2 =	rW._transferTimeB2 = .0;
	rW._transferTimeA3 =	rW._transferTimeB3 = .0;
	rW._deviceAddress = 4;	rW._size = 4;

	s._mainIdx = OGSS_ULONG_MAX;	s._system = true;		s._type = RQT_FKFLT;
	s._majrIdx = 1;					s._minrIdx = 0;
	s._serviceTime = 5.;			s._idxVolume = 0;
	s._idxDevice = 0;				s._numChild = 4;		s._numPrioChild = 2;
	s._transferTimeA1 =				s._transferTimeB1 = .0;
	s._transferTimeA2 =				s._transferTimeB2 = .0;
	s._transferTimeA3 =				s._transferTimeB3 = .0;
	s._deviceAddress = 0;			s._size = 4;

	t._mainIdx = OGSS_ULONG_MAX;	t._system = true;		t._type = RQT_FKRPL;
	t._majrIdx = 2;					t._minrIdx = 0;
	t._serviceTime = 5.;			t._idxVolume = 0;
	t._idxDevice = 0;				t._numChild = 4;		t._numPrioChild = 2;
	t._transferTimeA1 =				t._transferTimeB1 = .0;
	t._transferTimeA2 =				t._transferTimeB2 = .0;
	t._transferTimeA3 =				t._transferTimeB3 = .0;
	t._deviceAddress = 0;			t._size = 4;
	sync.addEntry (rR); sync.addEntry (rW); sync.addEntry (s); sync.addEntry (t);

	Request rR1 {rR}, rW1 {rW}, s1 {s}, t1 {t};
	rR1._majrIdx = 1;	rR1._numPrioChild = 2;	rR1._numChild = 6;
	rW1._majrIdx = 1;	rW1._numPrioChild = 0;	rW1._numChild = 7;
	s1._minrIdx = 1;	s1._idxDevice = 1;		s1._numPrioChild = 1;	s1._numLink = 0;
	t1._minrIdx = 1;	t1._idxDevice = 2;		t1._numPrioChild = 1;	t1._numLink = 0;
	sync.addEntry (rR1); sync.addEntry (rW1); sync.addEntry (s1); sync.addEntry (t1);

	Request rR2 {rR1}, rW2 {rW1}, s2 {s1}, t2 {t1};
	rR2._minrIdx = 1;	rR2._numChild = 0;		rR2._idxDevice = 0;		rR2._numPrioChild = 0;
	rW2._minrIdx = 1;	rW2._numChild = 0;		rW2._idxDevice = 0;		rW2._numPrioChild = 0;
	s2._minrIdx = 2;	s2._idxDevice = 2;		s2._numPrioChild = 0;
	t2._minrIdx = 2;	t2._idxDevice = 0;		t2._numPrioChild = 0;
	sync.addEntry (rR2); sync.addEntry (rW2); sync.addEntry (s2); sync.addEntry (t2);

	Request s3 {s2}, t3 {t2};
	s3._minrIdx = 3;	s3._idxDevice = 1;		s3._numPrioChild = 1; 	s3._deviceAddress = 4;	s3._numLink = 1; s3._date = 30.;
	t3._minrIdx = 3;	t3._idxDevice = 2;		t3._numPrioChild = 1;	t3._deviceAddress = 4;	t3._numLink = 1;
	sync.addEntry (s3); sync.addEntry (t3);

	Request s4 {s3}, t4 {t3};
	s4._minrIdx = 4;	s4._idxDevice = 2;		s4._numPrioChild = 0;
	t4._minrIdx = 4;	t4._idxDevice = 0;		t4._numPrioChild = 0;
	sync.addEntry (s4); sync.addEntry (t4);

	Request rR5 {rR2}, rW5 {rW2};
	rR5._minrIdx = 2;	rR5._numChild = 0;		rR5._idxDevice = 0;		rR5._operation = ROP_REDIRECTED;
	rW5._minrIdx = 2;	rW5._numChild = 0;		rW5._idxDevice = 0;		rW5._operation = ROP_REDIRECTED;
	sync.addEntry (rR5); sync.addEntry (rW5);

	Request rW6 {rW5};
	rW6._minrIdx = 3;	rW6._numPrioChild = 0;	rW6._idxDevice = 1;
	sync.addEntry (rW6);

	Request rR9 {rR5}, rW9 {rW6};
	rR9._minrIdx = 3;	rR9._idxDevice = 1;		rR9._numPrioChild = 1;	rR9._operation = ROP_RECOVER;
	rW9._minrIdx = 4;	rW9._idxDevice = 1;		rW9._operation = ROP_RECOVER;
	sync.addEntry (rR9); sync.addEntry (rW9);

	Request rR10 {rR9}, rW10 {rW9};
	rR10._minrIdx = 4;	rR10._idxDevice = 0;	rR10._numPrioChild = 0;
	rW10._minrIdx = 5;	rW10._idxDevice = 0;
	sync.addEntry (rR10); sync.addEntry (rW10);

	Request rR13 {rR10}, rW13 {rW10};
	rR13._minrIdx = 5;	rR13._idxDevice = 2;	rR13._numPrioChild = 1;	rR13._operation = ROP_COPY;
	rW13._minrIdx = 6;	rW13._idxDevice = 0;	rW13._numPrioChild = 0; rW13._operation = ROP_COPY;
	sync.addEntry (rR13); sync.addEntry (rW13);

	Request rR14 {rR13}, rW14 {rW13};
	rR14._minrIdx = 6;	rR14._idxDevice = 0;	rR14._numPrioChild = 0;
	rW14._minrIdx = 7;	rW14._idxDevice = 1;
	sync.addEntry (rR14); sync.addEntry (rW14);

	sync.process ();

	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 10)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 5)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 1) ) ._responseTime - 5.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 2) ) ._responseTime - 10.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 3) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 4) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 1) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 2) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 3) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 4) ) ._responseTime - .0)
	 	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}

OGSS_Bool
UT_SyncDefV2::renewal2ReqACopy () {
	HardwareParameters		hp;
		hp._numInterfaces = 3; hp._numTiers = 1; hp._numVolumes = 1;
		hp._numDevices = 3; hp._hostInterface = 0;
	vector <Tier>			vT;
		vT.push_back (Tier () ); vT.back () ._interface = 1;
	vector <Volume>			vV; 
		vV.push_back (Volume () ); vV.back () ._interface = 2;
			vV.back () ._parent = 0; vV.back () ._type = VTP_DECRAID;
			vV.back () ._suSize = 4;
	vector <Device>			vD;
		vD.push_back (Device () ); vD.back () ._parent = 0; vD.back () ._physicalCapacity = 8;
		vD.push_back (Device () ); vD.back () ._parent = 0; vD.back () ._physicalCapacity = 8;
		vD.push_back (Device () ); vD.back () ._parent = 0; vD.back () ._physicalCapacity = 8;

	SyncDefV2				sync (nullptr, hp, vT, vV, vD);
	Request					rR {50.}, rW {50.}, s {5.}, t {OGSS_REAL_MAX};

	rR._mainIdx = 0;		rR._multiple = true;	rR._operation = ROP_NATIVE;
	rR._serviceTime = 5.;	rR._idxVolume = 0;
	rR._idxDevice = 0;		rR._numChild = 1;
	rR._transferTimeA1 = 	rR._transferTimeB1 = .0;
	rR._transferTimeA2 = 	rR._transferTimeB2 = .0;
	rR._transferTimeA3 = 	rR._transferTimeB3 = .0;
	rR._deviceAddress = 0; 	rR._size = 4;

	rW._mainIdx = 1;		rW._multiple = true;	rW._operation = ROP_NATIVE;
	rW._serviceTime = 5.;	rW._idxVolume = 0;
	rW._idxDevice = 0;		rW._numChild = 1;
	rW._transferTimeA1 =	rW._transferTimeB1 = .0;
	rW._transferTimeA2 =	rW._transferTimeB2 = .0;
	rW._transferTimeA3 =	rW._transferTimeB3 = .0;
	rW._deviceAddress = 4;	rW._size = 4;

	s._mainIdx = OGSS_ULONG_MAX;	s._system = true;		s._type = RQT_FKFLT;
	s._majrIdx = 1;					s._minrIdx = 0;
	s._serviceTime = 5.;			s._idxVolume = 0;
	s._idxDevice = 0;				s._numChild = 4;		s._numPrioChild = 2;
	s._transferTimeA1 =				s._transferTimeB1 = .0;
	s._transferTimeA2 =				s._transferTimeB2 = .0;
	s._transferTimeA3 =				s._transferTimeB3 = .0;
	s._deviceAddress = 0;			s._size = 4;

	t._mainIdx = OGSS_ULONG_MAX;	t._system = true;		t._type = RQT_FKRPL;
	t._majrIdx = 2;					t._minrIdx = 0;
	t._serviceTime = 5.;			t._idxVolume = 0;
	t._idxDevice = 0;				t._numChild = 4;		t._numPrioChild = 2;
	t._transferTimeA1 =				t._transferTimeB1 = .0;
	t._transferTimeA2 =				t._transferTimeB2 = .0;
	t._transferTimeA3 =				t._transferTimeB3 = .0;
	t._deviceAddress = 0;			t._size = 4;
	sync.addEntry (rR); sync.addEntry (rW); sync.addEntry (s); sync.addEntry (t);

	Request rR1 {rR}, rW1 {rW}, s1 {s}, t1 {t};
	rR1._majrIdx = 1;	rR1._numPrioChild = 2;	rR1._numChild = 6;
	rW1._majrIdx = 1;	rW1._numPrioChild = 0;	rW1._numChild = 7;
	s1._minrIdx = 1;	s1._idxDevice = 1;		s1._numPrioChild = 1;	s1._numLink = 0;
	t1._minrIdx = 1;	t1._idxDevice = 2;		t1._numPrioChild = 1;	t1._numLink = 0;
	sync.addEntry (rR1); sync.addEntry (rW1); sync.addEntry (s1); sync.addEntry (t1);

	Request rR2 {rR1}, rW2 {rW1}, s2 {s1}, t2 {t1};
	rR2._minrIdx = 1;	rR2._numChild = 0;		rR2._idxDevice = 0;		rR2._numPrioChild = 0;
	rW2._minrIdx = 1;	rW2._numChild = 0;		rW2._idxDevice = 0;		rW2._numPrioChild = 0;
	s2._minrIdx = 2;	s2._idxDevice = 2;		s2._numPrioChild = 0;
	t2._minrIdx = 2;	t2._idxDevice = 0;		t2._numPrioChild = 0;
	sync.addEntry (rR2); sync.addEntry (rW2); sync.addEntry (s2); sync.addEntry (t2);

	Request s3 {s2}, t3 {t2};
	s3._minrIdx = 3;	s3._idxDevice = 1;		s3._numPrioChild = 1; 	s3._deviceAddress = 4;	s3._numLink = 1;
	t3._minrIdx = 3;	t3._idxDevice = 2;		t3._numPrioChild = 1;	t3._deviceAddress = 4;	t3._numLink = 1;
	sync.addEntry (s3); sync.addEntry (t3);

	Request s4 {s3}, t4 {t3};
	s4._minrIdx = 4;	s4._idxDevice = 2;		s4._numPrioChild = 0;
	t4._minrIdx = 4;	t4._idxDevice = 0;		t4._numPrioChild = 0;
	sync.addEntry (s4); sync.addEntry (t4);

	Request rR5 {rR2}, rW5 {rW2};
	rR5._minrIdx = 2;	rR5._numChild = 0;		rR5._idxDevice = 0;		rR5._operation = ROP_REDIRECTED;
	rW5._minrIdx = 2;	rW5._numChild = 0;		rW5._idxDevice = 0;		rW5._operation = ROP_REDIRECTED;
	sync.addEntry (rR5); sync.addEntry (rW5);

	Request rW6 {rW5};
	rW6._minrIdx = 3;	rW6._numPrioChild = 0;	rW6._idxDevice = 1;
	sync.addEntry (rW6);

	Request rR9 {rR5}, rW9 {rW6};
	rR9._minrIdx = 3;	rR9._idxDevice = 1;		rR9._numPrioChild = 1;	rR9._operation = ROP_RECOVER;
	rW9._minrIdx = 4;	rW9._idxDevice = 1;		rW9._operation = ROP_RECOVER;
	sync.addEntry (rR9); sync.addEntry (rW9);

	Request rR10 {rR9}, rW10 {rW9};
	rR10._minrIdx = 4;	rR10._idxDevice = 0;	rR10._numPrioChild = 0;
	rW10._minrIdx = 5;	rW10._idxDevice = 0;
	sync.addEntry (rR10); sync.addEntry (rW10);

	Request rR13 {rR10}, rW13 {rW10};
	rR13._minrIdx = 5;	rR13._idxDevice = 2;	rR13._numPrioChild = 1;	rR13._operation = ROP_COPY;
	rW13._minrIdx = 6;	rW13._idxDevice = 0;	rW13._numPrioChild = 0; rW13._operation = ROP_COPY;
	sync.addEntry (rR13); sync.addEntry (rW13);

	Request rR14 {rR13}, rW14 {rW13};
	rR14._minrIdx = 6;	rR14._idxDevice = 0;	rR14._numPrioChild = 0;
	rW14._minrIdx = 7;	rW14._idxDevice = 1;
	sync.addEntry (rR14); sync.addEntry (rW14);

	sync.process ();


	if (fabs (sync._requests.at (make_tuple (0, 0, 0) ) ._responseTime - 5.)
		< numeric_limits <float> ::epsilon ()
	 && fabs (sync._requests.at (make_tuple (1, 0, 0) ) ._responseTime - 10.)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 1) ) ._responseTime - 5.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 2) ) ._responseTime - 10.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 3) ) ._responseTime - 10.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 1, 4) ) ._responseTime - 15.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 1) ) ._responseTime - 5.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 2) ) ._responseTime - 10.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 3) ) ._responseTime - 10.0)
	 	< numeric_limits <float> ::epsilon ()
	 && fabs (sync._eventRequests.at (make_tuple (OGSS_ULONG_MAX, 2, 4) ) ._responseTime - 15.0)
	 	< numeric_limits <float> ::epsilon () )
		return true;

	return false;
}
*/