//$Id$
//------------------------------------------------------------------------------
//                                  VRInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Implements VRInterface Class

#include "VRInterface_v2.hpp"
#include "MessageInterface.hpp"    // for debugging

#define DEBUG

#define __REMOVE_OBJ_BY_SETTING_FLAG__
// If following flag is not defined, it will set data to zero for
// unpublished object
//#define __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__
// Show one-time warning if plotting object data is absent when data is published
#define __SHOW_WARNING_FOR_UNPUBLISHED_OBJ__


//static data define here, PARAMETER_TEXT & PARAMETER_TYPE
// remember to keep order
const std::string
VRInterface::PARAMETER_TEXT[VRInterfaceParamCount - SubscriberParamCount] =
{ 
	"Add",
	"CoordinateSystem",
	"DrawObject",
	"DrawLabel",
	"DrawVelocity",
	"DrawLineColour",
	"DrawTrajectory",
	"SpacePoint Radii",
	"ModelFile",
	"DataCollectFrequency",
	"UpdatePlotFrequency",
	"NumPointsToRedraw",
	"ShowPlot",
	"MaxPlotPoints",
	"Testerino"
};

const Gmat::ParameterType
VRInterface::PARAMETER_TYPE[VRInterfaceParamCount - SubscriberParamCount] =
{
	Gmat::OBJECTARRAY_TYPE,			//"Add",
	Gmat::OBJECT_TYPE,            //"CoordinateSystem"
	Gmat::BOOLEANARRAY_TYPE,      //"DrawObject"
	Gmat::BOOLEANARRAY_TYPE,		//"DrawLabel",
	Gmat::BOOLEANARRAY_TYPE,		//"DrawVelocity",
	Gmat::BOOLEANARRAY_TYPE,		//"DrawLineColour",
	Gmat::BOOLEANARRAY_TYPE,		//"DrawTrajectory",
	Gmat::INTEGER_TYPE,				//"SpacePoint Radii",
	Gmat::STRINGARRAY_TYPE,			//"ModelFile",
	Gmat::INTEGER_TYPE,				//"DataCollectFrequency",
	Gmat::INTEGER_TYPE,           //"UpdatePlotFrequency"
	Gmat::INTEGER_TYPE,           //"NumPointsToRedraw"
	Gmat::BOOLEAN_TYPE,           //"ShowPlot"
	Gmat::INTEGER_TYPE,           //"MaxPlotPoints"
	Gmat::STRINGARRAY_TYPE			//"Testerino"
};


//------------------------------------------------------------------------------
// VRInterface(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 * Declaring and initializing variables here
 * @param <type> Typename
 * @param <name> Name of the object
 * OPTIONAL @param for file management
 */
 //------------------------------------------------------------------------------
VRInterface::VRInterface(const std::string &type, const std::string &name)
	: Subscriber(type, name)
{
	// GmatBase data 
	parameterCount = VRInterfaceParamCount;
	objectTypes.push_back(GmatType::GetTypeId("VRInterface"));
	objectTypeNames.push_back("VRInterface");

	mViewCoordSystem = NULL; // or nullptr

	mOldName = instanceName;
	mViewCoordSysName = "EarthMJ2000Eq";

	mDataCollectFrequency = 1;
	mUpdatePlotFrequency = 50;
	mNumPointsToRedraw = 0;
	mNumData = 0;
	mNumCollected = 0;
	mMaxData = 20000;
	mDataAbsentWarningCount = 0;

	mScNameArray.clear();
	mObjectNameArray.clear();
	mAllSpNameArray.clear();
	mAllRefObjectNames.clear();
	mObjectArray.clear();
	mDrawOrbitArray.clear();
	mDrawObjectArray.clear();
	mAllSpArray.clear();
	mScXArray.clear();
	mScYArray.clear();
	mScZArray.clear();
	mScVxArray.clear();
	mScVyArray.clear();
	mScVzArray.clear();
	mScPrevDataPresent.clear();
	mScPrevEpoch.clear();
	mScPrevX.clear();
	mScPrevY.clear();
	mScPrevZ.clear();
	mScPrevVx.clear();
	mScPrevVy.clear();
	mScPrevVz.clear();

	mDrawOrbitMap.clear();
	mShowObjectMap.clear();

	mDefaultOrbitColorMap.clear();
	mDefaultTargetColorMap.clear();
	mCurrentOrbitColorMap.clear();
	mCurrentTargetColorMap.clear();

	mAllSpCount = 0;
	mScCount = 0;
	mObjectCount = 0;
	mNonStdBodyCount = 0;


}

//------------------------------------------------------------------------------
// VRInterface(const VRInterface &vri)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
 //------------------------------------------------------------------------------
VRInterface::VRInterface(const VRInterface &vri)
	: Subscriber(vri)
{

}

//------------------------------------------------------------------------------
// VRInterface& operator=(const VRInterface &vri)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
 //------------------------------------------------------------------------------
VRInterface& VRInterface::operator=(const VRInterface& vri)
{
	if (this == &vri)
		return *this;

	Subscriber::operator=(vri);
}

//------------------------------------------------------------------------------
// ~VRInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 * @note This destructor does not delete 3DView window, but clears data.
 *       3DView window is deleted when it is closed by the user or GMAT
 *       shuts down.
 */
 //------------------------------------------------------------------------------
VRInterface::~VRInterface()
{
	// clear buffers, delete cache data etc.
}

// Methods inherited from Subscriber

//------------------------------------------------------------------------------
// void Activate(bool state)
//------------------------------------------------------------------------------
/**
 * Turns on or off the plot.
 * This methods is called from the Toggle command.
 * Toggle command implementation
 */
 //------------------------------------------------------------------------------
bool VRInterface::Activate(bool state)
{
#ifdef DEBUG
	MessageInterface::ShowMessage
	("OrbitPlot::Activate() this=<%p>'%s' entered, state=%d, isInitialized=%d\n",
		this, GetName().c_str(), state, isInitialized);
#endif

	return Subscriber::Activate(state);
}



// Methods inherited from GmatBase

//------------------------------------------------------------------------------
//  bool Validate()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run validation that the object needs.
 *
 * @return true unless validation fails.
 */
 //------------------------------------------------------------------------------
bool VRInterface::Validate() {
	// validate stuff here

}

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
/**
* Set pointers and assignment. Initialise instance of interface.
*/
//------------------------------------------------------------------------------
bool VRInterface::Initialize() {

	if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)
		// do nothing 
		return true;


	Subscriber::Initialize(); // all others have this. Why?
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the VRInterface.
 *
 * @return clone of the VRInterface.
 *
 */
 //------------------------------------------------------------------------------
GmatBase* VRInterface::Clone() const
{
	return (new VRInterface(*this));
}

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
 //---------------------------------------------------------------------------
void VRInterface::Copy(const GmatBase* orig)
{
	operator=(*((VRInterface *)(orig)));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action. It is a utility method that derived
 * classes override to provide functionality that cannnot be 
 * implemented through basic parameter setting calls.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
 //------------------------------------------------------------------------------
	bool VRInterface::TakeAction(const std::string &action,
		const std::string &actionData)
{
#ifdef DEBUG
	MessageInterface::ShowMessage
	("VRInterface::TakeAction() '%s' entered, action='%s', actionData='%s'\n",
		GetName().c_str(), action.c_str(), actionData.c_str());
#endif

	if (action == "Bar")
	{
		return Foo();
	}
	else if (action == "Bar")
	{
		return Foo(actionData);
	}

	else if (action == "Bar")
	{
		// perform change
		return true;
	}

	return false;
}


// Methods inherited from Subscriber

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool VRInterface::Distribute(int len)
{
	// Real data conversion issue?
	// is this necessary?
	return false;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * Process trajectory data distributed by the GMAT Publisher
 *
 * @param <dat> An array of data to receive
 * @param <len> The number of data in the array
 */
 //------------------------------------------------------------------------------
bool VRInterface::Distribute(const Real *dat, Integer len)
{
#ifdef DEBUG
	MessageInterface::ShowMessage
	("===========================================================================\n"
		"VRInterface::Distribute() instanceName=%s, active=%d, isEndOfRun=%d, "
		"isEndOfReceive=%d\n   mAllSpCount=%d, mScCount=%d, len=%d, runstate=%d, "
		"isDataStateChanged=%d\n", instanceName.c_str(), active, isEndOfRun, isEndOfReceive,
		mAllSpCount, mScCount, len, runstate, isDataStateChanged);
#endif

	if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS) {
		return true;
	}

	// if data state changed from on to off or vice versa, update plot data so
	// data points can be flagged.
	if (isDataStateChanged)
	{
		if (isDataOn) {
			// put pen down for instanceName
		}
		else {
			// put pen down for instanceName
		}
			
		isDataStateChanged = false;
	}

	if (!active || mScCount <= 0)
		// if inactive or no objects to plot, return immediately
	{
	#if DBGLVL_UPDATE
		if (!active)
			MessageInterface::ShowMessage
			("==> OrbitView::Distribute() '%s' just returning true, it is not active\n",
				instanceName.c_str());
		else
			MessageInterface::ShowMessage
			("==> OrbitView::Distribute() '%s' just returning true, no spacecraft to plot\n",
				instanceName.c_str());
	#endif
		return true;
	}

	if (isEndOfRun) { // do stuff here once run is ended, finish up etc. 
		// signifiy end of run, sort shit out
		return true;
	}

	if (isEndOfReceive)
		// end of a trajectory segment
	{
		if ((mSolverIterOption == SI_CURRENT) &&
			(runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS))
			// if solver passed and still iterating
		{
			// update data in solver buffer with current solver data
			// plot new trajectory segment to represent iteration steps 
		}
		else
		{
			// refresh plot whatever that means
		}
	}

	if (len <= 0)	//If there is no data in buffer?
		return true;

	#ifdef DEBUG
		MessageInterface::ShowMessage("%s, len=%d\n", GetName().c_str(), len);
		for (int i = 0; i < len; i++)
			MessageInterface::ShowMessage("%.11f  ", dat[i]);
		MessageInterface::ShowMessage("\n");
	#endif

	if (!active) { // insert other conditions for inactivity here

		return true;
	}





	if ((mSolverIterOption == SI_NONE) && (runstate == Gmat::SOLVING)) {
		// no solver iterations and currently solving
	#ifdef DEBUG
			MessageInterface::ShowMessage
			("   Just returning: SolverIterations is %d and runstate is %d\n",
				mSolverIterOption, runstate);
	#endif
		return true;
	}

	// conditions passed, update data as normal

	if (len >= 7) { 
		// there is mission data in the stream, do main implementation here
		// publish data

		UpdateData(dat, len);
		return true;
	}
	
	// what if 0<len<7 -- test this


	return true; // Distribute must always return true
					 // so that ReceiveData() is called.
}




//----------------------------------------------------------------------
//		Implmentation methods here
//----------------------------------------------------------------------






// NOTHING BELOW IS INTEGRATED YET

//------------------------------------------------------------------------------
// void BufferZeroData(Integer scIndex)
//------------------------------------------------------------------------------
/**
 * Fills spacecraft state with zero
 */
 //------------------------------------------------------------------------------
void VRInterface::BufferZeroData(Integer scIndex)
{
	mScXArray[scIndex] = 0.0;
	mScYArray[scIndex] = 0.0;
	mScZArray[scIndex] = 0.0;
	mScVxArray[scIndex] = 0.0;
	mScVyArray[scIndex] = 0.0;
	mScVzArray[scIndex] = 0.0;
	//quaternions
}


//------------------------------------------------------------------------------
// void BufferPreviousData(Integer scIndex)
//------------------------------------------------------------------------------
/**
 * Fills spacecraft state with previous data
 */
 //------------------------------------------------------------------------------
void VRInterface::BufferPreviousData(Integer scIndex)
{
#if DBGLVL_DATA
	MessageInterface::ShowMessage
	("===> OrbitPlot::BufferPreviousData() setting to previous data\n   scIndex=%d, "
		"prev x,y,z: %f, %f, %f\n", scIndex, mScPrevX[scIndex], mScPrevY[scIndex],
		mScPrevZ[scIndex]);
#endif
	mScXArray[scIndex] = mScPrevX[scIndex];
	mScYArray[scIndex] = mScPrevY[scIndex];
	mScZArray[scIndex] = mScPrevZ[scIndex];
	mScVxArray[scIndex] = mScPrevVx[scIndex];
	mScVyArray[scIndex] = mScPrevVy[scIndex];
	mScVzArray[scIndex] = mScPrevVz[scIndex];
	//quaternions
}


//------------------------------------------------------------------------------
// Integer BufferOrbitData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * @return 1 if continue to updating plot
 *         2 if solving and plotting current iteration
 */
 //------------------------------------------------------------------------------
Integer VRInterface::BufferOrbitData(const Real *dat, Integer len)
{
#if DBGLVL_DATA
	MessageInterface::ShowMessage
	("OrbitPlot::BufferOrbitData() <%p>'%s' entered, len=%d, epoch=%.12f, mNumData=%d\n",
		this, GetName().c_str(), len, dat[0], mNumData);
#endif

	//------------------------------------------------------------
	// buffer orbit data
	//------------------------------------------------------------
	// zero data length is already checked in UpdateData()

#if DBGLVL_DATA > 1
	MessageInterface::ShowMessage
	("   mNumData=%d, mDataCollectFrequency=%d, currentProvider=<%p>\n",
		mNumData, mDataCollectFrequency, currentProvider);
	WriteCoordinateSystem(theDataCoordSystem, "   theDataCoordSystem");
	WriteCoordinateSystem(mViewCoordSystem, "   mViewCoordSystem");
#endif

	mNumCollected++;

#if DBGLVL_DATA > 1
	MessageInterface::ShowMessage
	("   currentProvider=%d, theDataLabels.size()=%d\n",
		currentProvider, theDataLabels.size());
#endif

	// @note
	// New Publisher code doesn't assign currentProvider anymore,
	// it just copies current labels. There was an issue with
	// provider id keep incrementing if data is regisgered and
	// published inside a GmatFunction
	StringArray dataLabels = theDataLabels[0];

#if DBGLVL_DATA_LABELS
	MessageInterface::ShowMessage("   Data labels for %s =\n   ", GetName().c_str());
	for (int j = 0; j < (int)dataLabels.size(); j++)
		MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
	MessageInterface::ShowMessage("\n");
#endif

	Integer idX, idY, idZ;
	Integer idVx, idVy, idVz;
	Integer scIndex = -1;

	for (Integer i = 0; i < mScCount; i++)
	{
		idX = FindIndexOfElement(dataLabels, mScNameArray[i] + ".X");
		idY = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Y");
		idZ = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Z");

		idVx = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vx");
		idVy = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vy");
		idVz = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vz");

#if DBGLVL_DATA_LABELS
		MessageInterface::ShowMessage
		("   mScNameArray[%d]=%s, idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d\n",
			i, mScNameArray[i].c_str(), idX, idY, idZ, idVx, idVy, idVz);
#endif

		scIndex++;

		// If any of index not found, handle absent data and continue with the next spacecraft
		if (idX == -1 || idY == -1 || idZ == -1 ||
			idVx == -1 || idVy == -1 || idVz == -1)
		{
			HandleAbsentData(mScNameArray[i], scIndex, dat[0]);
			mScPrevDataPresent[scIndex] = false;
			continue;
		}


		// Buffer actual data

#if DBGLVL_DATA
		MessageInterface::ShowMessage
		("   %s, sat='%s', epoch = %.11f, X,Y,Z = %f, %f, %f\n", GetName().c_str(),
			mScNameArray[i].c_str(), dat[0], dat[idX], dat[idY], dat[idZ]);
#endif

		// If distributed data coordinate system is different from view
		// coordinate system, convert data here.
		// If we convert after current epoch, it will not give correct
		// results, if origin is spacecraft,
		// ie, sat->GetMJ2000State(epoch) will not give correct results.
		if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
			(mViewCoordSystem != theDataCoordSystem))
		{
#if DBGLVL_DATA
			MessageInterface::ShowMessage
			("   Converting data from '%s' to '%s'\n", theDataCoordSystem->GetName().c_str(),
				mViewCoordSystem->GetName().c_str());
#endif

			CoordinateConverter coordConverter;
			Rvector6 inState, outState;

			// convert position and velocity
			inState.Set(dat[idX], dat[idY], dat[idZ],
				dat[idVx], dat[idVy], dat[idVz]);

			coordConverter.Convert(dat[0], inState, theDataCoordSystem,
				outState, mViewCoordSystem);

			mScXArray[scIndex] = outState[0];
			mScYArray[scIndex] = outState[1];
			mScZArray[scIndex] = outState[2];
			mScVxArray[scIndex] = outState[3];
			mScVyArray[scIndex] = outState[4];
			mScVzArray[scIndex] = outState[5];
		}
		else
		{
			mScXArray[scIndex] = dat[idX];
			mScYArray[scIndex] = dat[idY];
			mScZArray[scIndex] = dat[idZ];
			mScVxArray[scIndex] = dat[idVx];
			mScVyArray[scIndex] = dat[idVy];
			mScVzArray[scIndex] = dat[idVz];
		}

		// Save old data for next time
		mScPrevDataPresent[scIndex] = true;
		mScPrevEpoch[scIndex] = dat[0];
		mScPrevX[scIndex] = mScXArray[scIndex];
		mScPrevY[scIndex] = mScYArray[scIndex];
		mScPrevZ[scIndex] = mScZArray[scIndex];
		mScPrevVx[scIndex] = mScVxArray[scIndex];
		mScPrevVy[scIndex] = mScVyArray[scIndex];
		mScPrevVz[scIndex] = mScVzArray[scIndex];

#if DBGLVL_DATA
		MessageInterface::ShowMessage
		("   after buffering, scNo=%d, scIndex=%d, X,Y,Z = %f, %f, %f\n",
			i, scIndex, mScXArray[scIndex], mScYArray[scIndex], mScZArray[scIndex]);
#endif

#if DBGLVL_DATA > 1
		MessageInterface::ShowMessage
		("   Vx,Vy,Vz = %f, %f, %f\n",
			mScVxArray[scIndex], mScVyArray[scIndex], mScVzArray[scIndex]);
#endif
	}

	// if only showing current iteration, buffer data and return
	if (mSolverIterOption == SI_CURRENT)
	{
		// save data when targeting or last iteration
		if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
		{
			mCurrScArray.push_back(mScNameArray);
			mCurrEpochArray.push_back(dat[0]);
			mCurrXArray.push_back(mScXArray);
			mCurrYArray.push_back(mScYArray);
			mCurrZArray.push_back(mScZArray);
			mCurrVxArray.push_back(mScVxArray);
			mCurrVyArray.push_back(mScVyArray);
			mCurrVzArray.push_back(mScVzArray);
		}

		if (runstate == Gmat::SOLVING)
		{
#if DBGLVL_DATA
			MessageInterface::ShowMessage
			("=====> num buffered = %d\n", mCurrEpochArray.size());
#endif
			return 2;
		}
	}

	return 1;
}


//------------------------------------------------------------------------------
// void HandleAbsentData(Integer scIndex, Real currEpoch)
//------------------------------------------------------------------------------
void VRInterface::HandleAbsentData(const std::string &scName, Integer scIndex,
	Real currEpoch)
{
#ifdef DEBUG_ABSENT_DATA
	MessageInterface::ShowMessage
	("=> OrbitPlot::HandleAbsentData() entered, scName='%s', scIndex=%d, "
		"currEpoch=%.12f\n", scName.c_str(), scIndex, currEpoch);
	MessageInterface::ShowMessage
	("   '%s' data not present so buffering with zero or previous data\n"
		"   currEpoch=%.12f, mScCount=%d, mNumData=%d, mScPrevDataPresent[%d]=%s, "
		"prevEpoch[%d]=%.12f\n", scName.c_str(), currEpoch, mScCount,
		mNumData, scIndex, mScPrevDataPresent[scIndex] == true ? "true" : "false",
		scIndex, mScPrevEpoch[scIndex]);
#endif

	// If __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__ is defined, use previous
	// data if object is not published with data (see GMT-5650)
#ifdef __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__
	bool useZeroForDataAbsent = false;
	BufferPreviousData(scIndex);
	// Send one time flag to plots to ignore time order
	PlotInterface::TakeGlAction(instanceName, "IgnoreTimeSequence");
#else
	bool useZeroForDataAbsent = true;
	BufferZeroData(scIndex);
#endif

	// Display warning message about data cannot be displayed correctly
	// Check if data are on and off
	// If status is solving or solved pass, skip message
	if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
	{
#ifdef DEBUG_ABSENT_DATA
		MessageInterface::ShowMessage
		("   => Status is solving, so skip warning message\n");
#endif
	}
	else if (mNumData > 2)
	{
		// Check if time is ordered in forward or backward
		if (currEpoch < mScPrevEpoch[scIndex] || mScPrevDataPresent[scIndex])
		{
			// If spacecraft going backward and previous data not present, skip message
			if (currEpoch < mScPrevEpoch[scIndex] && !mScPrevDataPresent[scIndex])
			{
#ifdef DEBUG_ABSENT_DATA
				MessageInterface::ShowMessage
				("   => Spacecraft is going backward and previous data not "
					"present, so skip warning message\n");
#endif
			}
			// If current time and previous time is the same, skip message
			else if (currEpoch == mScPrevEpoch[scIndex])
			{
#ifdef DEBUG_ABSENT_DATA
				MessageInterface::ShowMessage
				("   => Current and previous times are the same, so skip warning message\n");
#endif
			}
			else
			{
#ifdef __SHOW_WARNING_FOR_UNPUBLISHED_OBJ__
				if (mDataAbsentWarningCount < (mScCount * 10))
				{
					bool writeWarning = true;
					mDataAbsentWarningCount++;

					// If first time warning and previous data not present, skip message
					if (mScPrevDataPresent[scIndex] && mDataAbsentWarningCount == 1)
					{
						writeWarning = false;
#ifdef DEBUG_ABSENT_DATA
						MessageInterface::ShowMessage
						("   => First time warning and previous data not present, "
							"so skip warning message\n");
#endif
					}

					if (writeWarning)
					{
						std::string dataValueMsg = "previous data";
						if (useZeroForDataAbsent)
							dataValueMsg = "zero";

						MessageInterface::ShowMessage
						("*** WARNING *** Drawing object '%s' has no data published at "
							"epoch %.12f so data is set to %s. Plot cannot be displayed "
							"correctly when drawing object is not time ordered or data is zero. "
							"Last data published at %.12f.\n", scName.c_str(), currEpoch,
							dataValueMsg.c_str(), mScPrevEpoch[scIndex]);
						if (mDataAbsentWarningCount == (mScCount * 10))
							MessageInterface::ShowMessage("*** MAXIMUM WARNING message reached.\n");
					}
				}
#endif
			}
		}
	}
#ifdef DEBUG_ABSENT_DATA
	MessageInterface::ShowMessage
	("OrbitPlot::HandleAbsentData() leaving, scName='%s', scIndex=%d, "
		"currEpoch=%.12f\n", scName.c_str(), scIndex, currEpoch);
#endif
}


//------------------------------------------------------------------------------
// bool UpdateData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool VRInterface::UpdateData(const Real *dat, Integer len)
{
	if (len == 0)
		return true;

	// Skip data if data publishing command such as Propagate is inside a function
	// and this OrbitPlot is not a global nor a local object (i.e declared in the main script)
	// (LOJ: 2015.08.17)
	if (currentProvider && currentProvider->TakeAction("IsInFunction"))
	{
#ifdef DEBUG_FUNCTION_DATA
		MessageInterface::ShowMessage
		("   Data is published from the function, '%s' IsGlobal:%s, IsLocal:%s\n",
			GetName().c_str(), IsGlobal() ? "Yes" : "No", IsLocal() ? "Yes" : "No");
#endif

		bool skipData = false;
		// Check for spacepoints if data should be skipped or not
		for (int i = 0; i < mAllSpCount; i++)
		{
			SpacePoint *sp = mAllSpArray[i];
#ifdef DEBUG_FUNCTION_DATA
			MessageInterface::ShowMessage
			("   mAllSpNameArray[%d]=<%p>'%s'\n", i, mAllSpArray[i],
				mAllSpNameArray[i].c_str());
#endif

			if (sp)
			{
#ifdef DEBUG_FUNCTION_DATA
				MessageInterface::ShowMessage
				("   sp = <%p>[%s]'%s', IsGlobal=%d, IsLocal=%d\n",
					sp, sp->GetTypeName().c_str(), sp->GetName().c_str(),
					sp->IsGlobal(), sp->IsLocal());
#endif

				// Skip data if OrbitPlot is global and spacepoint is local
				if (IsGlobal() && sp->IsLocal())
				{
#ifdef DEBUG_FUNCTION_DATA
					MessageInterface::ShowMessage
					("   Skip data since '%s' is global and spacepoint is local\n",
						GetName().c_str());
#endif
					skipData = true;
					break;
				}
				// Skip data if spacepoint is not a global nor a local object
				else if (!(sp->IsGlobal()) && !(sp->IsLocal()))
				{
#ifdef DEBUG_FUNCTION_DATA
					MessageInterface::ShowMessage
					("   Skip data since spacepoint is not a global nor a local object\n");
#endif
					skipData = true;
					break;
				}
			}
		}

		if (skipData)
		{
#ifdef DEBUG_FUNCTION_DATA
			MessageInterface::ShowMessage
			("OrbitPlot::Update() this=<%p>'%s' just returning true\n   data is "
				"from a function and spacepoint is not a global nor a local object\n",
				this, GetName().c_str());
#endif
			return true;
		}
	}


	mNumData++;

#if DBGLVL_UPDATE > 1
	MessageInterface::ShowMessage
	("\nOrbitPlot::UpdateData() entered, mNumData=%d, mDataCollectFrequency=%d, "
		"currentProvider=<%p>\n", mNumData, mDataCollectFrequency, currentProvider);
	ColorMap::const_iterator iter = mCurrentOrbitColorMap.begin();
	while (iter != mCurrentOrbitColorMap.end())
	{
		MessageInterface::ShowMessage
		("mCurrentOrbitColorMap[%s] = %06X\n", (iter->first).c_str(), iter->second);
		iter++;
	}
#endif

	// Buffer data if data collect frequency is met or fist data
	if ((mNumData % mDataCollectFrequency) == 0 || (mNumData == 1))
	{
		Integer status = BufferOrbitData(dat, len);

		// if solving and plotting current iteration just return
		if (status == 2)
			return true;

#if DBGLVL_UPDATE > 0
		MessageInterface::ShowMessage("==========> Updating plot data\n");
#endif

		bool solving = false;
		if (runstate == Gmat::SOLVING)
			solving = true;

		bool inFunction = false;
		if (currentProvider && currentProvider->TakeAction("IsInFunction"))
			inFunction = true;

		bool update = (mNumCollected % mUpdatePlotFrequency) == 0;

		PlotInterface::
			UpdateGlPlot(instanceName, mOldName, mScNameArray, dat[0], mScXArray,
				mScYArray, mScZArray, mScVxArray, mScVyArray, mScVzArray,
				mCurrentOrbitColorMap, mCurrentTargetColorMap, solving,
				mSolverIterOption, update, isDataOn, inFunction);

		if (update)
			mNumCollected = 0;
	}

#if DBGLVL_UPDATE > 1
	MessageInterface::ShowMessage("OrbitPlot::UpdateData() returning true\n");
#endif
	return true;
}












// NOTHING BELOW IS INTEGRATED YET

//------------------------------------------------------------------------------
// void BufferZeroData(Integer scIndex)
//------------------------------------------------------------------------------
/**
 * Fills spacecraft state with zero
 */
 //------------------------------------------------------------------------------
void VRInterface::BufferZeroData(Integer scIndex)
{
	mScXArray[scIndex] = 0.0;
	mScYArray[scIndex] = 0.0;
	mScZArray[scIndex] = 0.0;
	mScVxArray[scIndex] = 0.0;
	mScVyArray[scIndex] = 0.0;
	mScVzArray[scIndex] = 0.0;
	//quaternions
}


//------------------------------------------------------------------------------
// void BufferPreviousData(Integer scIndex)
//------------------------------------------------------------------------------
/**
 * Fills spacecraft state with previous data
 */
 //------------------------------------------------------------------------------
void VRInterface::BufferPreviousData(Integer scIndex)
{
#if DBGLVL_DATA
	MessageInterface::ShowMessage
	("===> OrbitPlot::BufferPreviousData() setting to previous data\n   scIndex=%d, "
		"prev x,y,z: %f, %f, %f\n", scIndex, mScPrevX[scIndex], mScPrevY[scIndex],
		mScPrevZ[scIndex]);
#endif
	mScXArray[scIndex] = mScPrevX[scIndex];
	mScYArray[scIndex] = mScPrevY[scIndex];
	mScZArray[scIndex] = mScPrevZ[scIndex];
	mScVxArray[scIndex] = mScPrevVx[scIndex];
	mScVyArray[scIndex] = mScPrevVy[scIndex];
	mScVzArray[scIndex] = mScPrevVz[scIndex];
	//quaternions
}


//------------------------------------------------------------------------------
// Integer BufferOrbitData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * @return 1 if continue to updating plot
 *         2 if solving and plotting current iteration
 */
 //------------------------------------------------------------------------------
Integer VRInterface::BufferOrbitData(const Real *dat, Integer len)
{
#if DBGLVL_DATA
	MessageInterface::ShowMessage
	("OrbitPlot::BufferOrbitData() <%p>'%s' entered, len=%d, epoch=%.12f, mNumData=%d\n",
		this, GetName().c_str(), len, dat[0], mNumData);
#endif

	//------------------------------------------------------------
	// buffer orbit data
	//------------------------------------------------------------
	// zero data length is already checked in UpdateData()

#if DBGLVL_DATA > 1
	MessageInterface::ShowMessage
	("   mNumData=%d, mDataCollectFrequency=%d, currentProvider=<%p>\n",
		mNumData, mDataCollectFrequency, currentProvider);
	WriteCoordinateSystem(theDataCoordSystem, "   theDataCoordSystem");
	WriteCoordinateSystem(mViewCoordSystem, "   mViewCoordSystem");
#endif

	mNumCollected++;

#if DBGLVL_DATA > 1
	MessageInterface::ShowMessage
	("   currentProvider=%d, theDataLabels.size()=%d\n",
		currentProvider, theDataLabels.size());
#endif

	// @note
	// New Publisher code doesn't assign currentProvider anymore,
	// it just copies current labels. There was an issue with
	// provider id keep incrementing if data is regisgered and
	// published inside a GmatFunction
	StringArray dataLabels = theDataLabels[0];

#if DBGLVL_DATA_LABELS
	MessageInterface::ShowMessage("   Data labels for %s =\n   ", GetName().c_str());
	for (int j = 0; j < (int)dataLabels.size(); j++)
		MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
	MessageInterface::ShowMessage("\n");
#endif

	Integer idX, idY, idZ;
	Integer idVx, idVy, idVz;
	Integer scIndex = -1;

	for (Integer i = 0; i < mScCount; i++)
	{
		idX = FindIndexOfElement(dataLabels, mScNameArray[i] + ".X");
		idY = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Y");
		idZ = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Z");

		idVx = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vx");
		idVy = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vy");
		idVz = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vz");

#if DBGLVL_DATA_LABELS
		MessageInterface::ShowMessage
		("   mScNameArray[%d]=%s, idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d\n",
			i, mScNameArray[i].c_str(), idX, idY, idZ, idVx, idVy, idVz);
#endif

		scIndex++;

		// If any of index not found, handle absent data and continue with the next spacecraft
		if (idX == -1 || idY == -1 || idZ == -1 ||
			idVx == -1 || idVy == -1 || idVz == -1)
		{
			HandleAbsentData(mScNameArray[i], scIndex, dat[0]);
			mScPrevDataPresent[scIndex] = false;
			continue;
		}


		// Buffer actual data

#if DBGLVL_DATA
		MessageInterface::ShowMessage
		("   %s, sat='%s', epoch = %.11f, X,Y,Z = %f, %f, %f\n", GetName().c_str(),
			mScNameArray[i].c_str(), dat[0], dat[idX], dat[idY], dat[idZ]);
#endif

		// If distributed data coordinate system is different from view
		// coordinate system, convert data here.
		// If we convert after current epoch, it will not give correct
		// results, if origin is spacecraft,
		// ie, sat->GetMJ2000State(epoch) will not give correct results.
		if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
			(mViewCoordSystem != theDataCoordSystem))
		{
#if DBGLVL_DATA
			MessageInterface::ShowMessage
			("   Converting data from '%s' to '%s'\n", theDataCoordSystem->GetName().c_str(),
				mViewCoordSystem->GetName().c_str());
#endif

			CoordinateConverter coordConverter;
			Rvector6 inState, outState;

			// convert position and velocity
			inState.Set(dat[idX], dat[idY], dat[idZ],
				dat[idVx], dat[idVy], dat[idVz]);

			coordConverter.Convert(dat[0], inState, theDataCoordSystem,
				outState, mViewCoordSystem);

			mScXArray[scIndex] = outState[0];
			mScYArray[scIndex] = outState[1];
			mScZArray[scIndex] = outState[2];
			mScVxArray[scIndex] = outState[3];
			mScVyArray[scIndex] = outState[4];
			mScVzArray[scIndex] = outState[5];
		}
		else
		{
			mScXArray[scIndex] = dat[idX];
			mScYArray[scIndex] = dat[idY];
			mScZArray[scIndex] = dat[idZ];
			mScVxArray[scIndex] = dat[idVx];
			mScVyArray[scIndex] = dat[idVy];
			mScVzArray[scIndex] = dat[idVz];
		}

		// Save old data for next time
		mScPrevDataPresent[scIndex] = true;
		mScPrevEpoch[scIndex] = dat[0];
		mScPrevX[scIndex] = mScXArray[scIndex];
		mScPrevY[scIndex] = mScYArray[scIndex];
		mScPrevZ[scIndex] = mScZArray[scIndex];
		mScPrevVx[scIndex] = mScVxArray[scIndex];
		mScPrevVy[scIndex] = mScVyArray[scIndex];
		mScPrevVz[scIndex] = mScVzArray[scIndex];

#if DBGLVL_DATA
		MessageInterface::ShowMessage
		("   after buffering, scNo=%d, scIndex=%d, X,Y,Z = %f, %f, %f\n",
			i, scIndex, mScXArray[scIndex], mScYArray[scIndex], mScZArray[scIndex]);
#endif

#if DBGLVL_DATA > 1
		MessageInterface::ShowMessage
		("   Vx,Vy,Vz = %f, %f, %f\n",
			mScVxArray[scIndex], mScVyArray[scIndex], mScVzArray[scIndex]);
#endif
	}

	// if only showing current iteration, buffer data and return
	if (mSolverIterOption == SI_CURRENT)
	{
		// save data when targeting or last iteration
		if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
		{
			mCurrScArray.push_back(mScNameArray);
			mCurrEpochArray.push_back(dat[0]);
			mCurrXArray.push_back(mScXArray);
			mCurrYArray.push_back(mScYArray);
			mCurrZArray.push_back(mScZArray);
			mCurrVxArray.push_back(mScVxArray);
			mCurrVyArray.push_back(mScVyArray);
			mCurrVzArray.push_back(mScVzArray);
		}

		if (runstate == Gmat::SOLVING)
		{
#if DBGLVL_DATA
			MessageInterface::ShowMessage
			("=====> num buffered = %d\n", mCurrEpochArray.size());
#endif
			return 2;
		}
	}

	return 1;
}


//------------------------------------------------------------------------------
// void HandleAbsentData(Integer scIndex, Real currEpoch)
//------------------------------------------------------------------------------
void VRInterface::HandleAbsentData(const std::string &scName, Integer scIndex,
	Real currEpoch)
{
#ifdef DEBUG_ABSENT_DATA
	MessageInterface::ShowMessage
	("=> OrbitPlot::HandleAbsentData() entered, scName='%s', scIndex=%d, "
		"currEpoch=%.12f\n", scName.c_str(), scIndex, currEpoch);
	MessageInterface::ShowMessage
	("   '%s' data not present so buffering with zero or previous data\n"
		"   currEpoch=%.12f, mScCount=%d, mNumData=%d, mScPrevDataPresent[%d]=%s, "
		"prevEpoch[%d]=%.12f\n", scName.c_str(), currEpoch, mScCount,
		mNumData, scIndex, mScPrevDataPresent[scIndex] == true ? "true" : "false",
		scIndex, mScPrevEpoch[scIndex]);
#endif

	// If __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__ is defined, use previous
	// data if object is not published with data (see GMT-5650)
#ifdef __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__
	bool useZeroForDataAbsent = false;
	BufferPreviousData(scIndex);
	// Send one time flag to plots to ignore time order
	PlotInterface::TakeGlAction(instanceName, "IgnoreTimeSequence");
#else
	bool useZeroForDataAbsent = true;
	BufferZeroData(scIndex);
#endif

	// Display warning message about data cannot be displayed correctly
	// Check if data are on and off
	// If status is solving or solved pass, skip message
	if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
	{
#ifdef DEBUG_ABSENT_DATA
		MessageInterface::ShowMessage
		("   => Status is solving, so skip warning message\n");
#endif
	}
	else if (mNumData > 2)
	{
		// Check if time is ordered in forward or backward
		if (currEpoch < mScPrevEpoch[scIndex] || mScPrevDataPresent[scIndex])
		{
			// If spacecraft going backward and previous data not present, skip message
			if (currEpoch < mScPrevEpoch[scIndex] && !mScPrevDataPresent[scIndex])
			{
#ifdef DEBUG_ABSENT_DATA
				MessageInterface::ShowMessage
				("   => Spacecraft is going backward and previous data not "
					"present, so skip warning message\n");
#endif
			}
			// If current time and previous time is the same, skip message
			else if (currEpoch == mScPrevEpoch[scIndex])
			{
#ifdef DEBUG_ABSENT_DATA
				MessageInterface::ShowMessage
				("   => Current and previous times are the same, so skip warning message\n");
#endif
			}
			else
			{
#ifdef __SHOW_WARNING_FOR_UNPUBLISHED_OBJ__
				if (mDataAbsentWarningCount < (mScCount * 10))
				{
					bool writeWarning = true;
					mDataAbsentWarningCount++;

					// If first time warning and previous data not present, skip message
					if (mScPrevDataPresent[scIndex] && mDataAbsentWarningCount == 1)
					{
						writeWarning = false;
#ifdef DEBUG_ABSENT_DATA
						MessageInterface::ShowMessage
						("   => First time warning and previous data not present, "
							"so skip warning message\n");
#endif
					}

					if (writeWarning)
					{
						std::string dataValueMsg = "previous data";
						if (useZeroForDataAbsent)
							dataValueMsg = "zero";

						MessageInterface::ShowMessage
						("*** WARNING *** Drawing object '%s' has no data published at "
							"epoch %.12f so data is set to %s. Plot cannot be displayed "
							"correctly when drawing object is not time ordered or data is zero. "
							"Last data published at %.12f.\n", scName.c_str(), currEpoch,
							dataValueMsg.c_str(), mScPrevEpoch[scIndex]);
						if (mDataAbsentWarningCount == (mScCount * 10))
							MessageInterface::ShowMessage("*** MAXIMUM WARNING message reached.\n");
					}
				}
#endif
			}
		}
	}
#ifdef DEBUG_ABSENT_DATA
	MessageInterface::ShowMessage
	("OrbitPlot::HandleAbsentData() leaving, scName='%s', scIndex=%d, "
		"currEpoch=%.12f\n", scName.c_str(), scIndex, currEpoch);
#endif
}


//------------------------------------------------------------------------------
// bool UpdateData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool VRInterface::UpdateData(const Real *dat, Integer len)
{
	if (len == 0)
		return true;

	// Skip data if data publishing command such as Propagate is inside a function
	// and this OrbitPlot is not a global nor a local object (i.e declared in the main script)
	// (LOJ: 2015.08.17)
	if (currentProvider && currentProvider->TakeAction("IsInFunction"))
	{
#ifdef DEBUG_FUNCTION_DATA
		MessageInterface::ShowMessage
		("   Data is published from the function, '%s' IsGlobal:%s, IsLocal:%s\n",
			GetName().c_str(), IsGlobal() ? "Yes" : "No", IsLocal() ? "Yes" : "No");
#endif

		bool skipData = false;
		// Check for spacepoints if data should be skipped or not
		for (int i = 0; i < mAllSpCount; i++)
		{
			SpacePoint *sp = mAllSpArray[i];
#ifdef DEBUG_FUNCTION_DATA
			MessageInterface::ShowMessage
			("   mAllSpNameArray[%d]=<%p>'%s'\n", i, mAllSpArray[i],
				mAllSpNameArray[i].c_str());
#endif

			if (sp)
			{
#ifdef DEBUG_FUNCTION_DATA
				MessageInterface::ShowMessage
				("   sp = <%p>[%s]'%s', IsGlobal=%d, IsLocal=%d\n",
					sp, sp->GetTypeName().c_str(), sp->GetName().c_str(),
					sp->IsGlobal(), sp->IsLocal());
#endif

				// Skip data if OrbitPlot is global and spacepoint is local
				if (IsGlobal() && sp->IsLocal())
				{
#ifdef DEBUG_FUNCTION_DATA
					MessageInterface::ShowMessage
					("   Skip data since '%s' is global and spacepoint is local\n",
						GetName().c_str());
#endif
					skipData = true;
					break;
				}
				// Skip data if spacepoint is not a global nor a local object
				else if (!(sp->IsGlobal()) && !(sp->IsLocal()))
				{
#ifdef DEBUG_FUNCTION_DATA
					MessageInterface::ShowMessage
					("   Skip data since spacepoint is not a global nor a local object\n");
#endif
					skipData = true;
					break;
				}
			}
		}

		if (skipData)
		{
#ifdef DEBUG_FUNCTION_DATA
			MessageInterface::ShowMessage
			("OrbitPlot::Update() this=<%p>'%s' just returning true\n   data is "
				"from a function and spacepoint is not a global nor a local object\n",
				this, GetName().c_str());
#endif
			return true;
		}
	}


	mNumData++;

#if DBGLVL_UPDATE > 1
	MessageInterface::ShowMessage
	("\nOrbitPlot::UpdateData() entered, mNumData=%d, mDataCollectFrequency=%d, "
		"currentProvider=<%p>\n", mNumData, mDataCollectFrequency, currentProvider);
	ColorMap::const_iterator iter = mCurrentOrbitColorMap.begin();
	while (iter != mCurrentOrbitColorMap.end())
	{
		MessageInterface::ShowMessage
		("mCurrentOrbitColorMap[%s] = %06X\n", (iter->first).c_str(), iter->second);
		iter++;
	}
#endif

	// Buffer data if data collect frequency is met or fist data
	if ((mNumData % mDataCollectFrequency) == 0 || (mNumData == 1))
	{
		Integer status = BufferOrbitData(dat, len);

		// if solving and plotting current iteration just return
		if (status == 2)
			return true;

#if DBGLVL_UPDATE > 0
		MessageInterface::ShowMessage("==========> Updating plot data\n");
#endif

		bool solving = false;
		if (runstate == Gmat::SOLVING)
			solving = true;

		bool inFunction = false;
		if (currentProvider && currentProvider->TakeAction("IsInFunction"))
			inFunction = true;

		bool update = (mNumCollected % mUpdatePlotFrequency) == 0;

		PlotInterface::
			UpdateGlPlot(instanceName, mOldName, mScNameArray, dat[0], mScXArray,
				mScYArray, mScZArray, mScVxArray, mScVyArray, mScVzArray,
				mCurrentOrbitColorMap, mCurrentTargetColorMap, solving,
				mSolverIterOption, update, isDataOn, inFunction);

		if (update)
			mNumCollected = 0;
	}

#if DBGLVL_UPDATE > 1
	MessageInterface::ShowMessage("OrbitPlot::UpdateData() returning true\n");
#endif
	return true;
}
