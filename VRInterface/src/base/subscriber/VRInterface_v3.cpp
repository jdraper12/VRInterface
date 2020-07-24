//$Id$
//------------------------------------------------------------------------------
//                                  VRInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Implements VRInterface Class

#include "VRInterface_v3.hpp"
#include "MessageInterface.hpp"		// for debugging
#include "SubscriberException.hpp"	// for SubscriberException()
#include "StringUtil.hpp"				// for ToReal()
// #include "PlotInterface.hpp"       // Change this to whatever handles plot
#include "TextParser.hpp"          // for SeparateBrackets()
#include "StringUtil.hpp"          // for ToReal()
#include "CoordinateConverter.hpp" // for Convert()
#include "SpaceObject.hpp"         // for GetEpoch()
#include "RgbColor.hpp"            // for Color::ToIntColor()
#include <algorithm>               // for find(), distance()
#include "ColorTypes.hpp"          // for namespace GmatColor::
#include "FileUtil.hpp"				  // for fileName validation
#include "FileManager.hpp"			  // for file management
#include "DataManager.hpp"




#define DEBUG_BUFFER_UPDATE
#define STOPPED_HERE
//#define Q1 0
//#define Q2 1
//#define Q3 2
//#define Q4 3

enum {Q1, Q2, Q3, Q4};

#define __REMOVE_OBJ_BY_SETTING_FLAG__
// If following flag is not defined, it will set data to zero for
// unpublished object
//#define __USE_PREVIOUS_DATA_FOR_UNPUBLISHED_OBJ__
// Show one-time warning if plotting object data is absent when data is published
#define __SHOW_WARNING_FOR_UNPUBLISHED_OBJ__


//---------------------------------
// static data
//---------------------------------
// remember to keep order

const std::string
VRInterface::PARAMETER_TEXT[VRInterfaceParamCount - SubscriberParamCount] =
{
	"Add",
	"CoordinateSystem",
	//"DrawObject",
	//"DrawLabel",
	//"DrawVelocity",
	//"DrawLineColour",
	//"DrawTrajectory",
	"SpacecraftRadii", 
	//"ModelFile",
	"DataCollectFrequency",
	"UpdatePlotFrequency",
	"NumPointsToRedraw",
	"ShowPlot",
	"MaxPlotPoints",
	"JsonFileLocation"
};


const Gmat::ParameterType
VRInterface::PARAMETER_TYPE[VRInterfaceParamCount - SubscriberParamCount] =
{
	Gmat::OBJECTARRAY_TYPE,			//"Add",
	Gmat::OBJECT_TYPE,            //"CoordinateSystem"
	//Gmat::BOOLEANARRAY_TYPE,      //"DrawObject"
	//Gmat::BOOLEANARRAY_TYPE,		  //"DrawLabel",
	//Gmat::BOOLEANARRAY_TYPE,		  //"DrawVelocity",
	//Gmat::BOOLEANARRAY_TYPE,		  //"DrawLineColour",
	//Gmat::BOOLEANARRAY_TYPE,		  //"DrawTrajectory",
	Gmat::INTEGER_TYPE,				//"Spacecraft Radii",
	//Gmat::FILENAME_TYPE,				//"ModelFile",
	Gmat::INTEGER_TYPE,				//"DataCollectFrequency",
	Gmat::INTEGER_TYPE,           //"UpdatePlotFrequency"
	Gmat::INTEGER_TYPE,           //"NumPointsToRedraw"
	Gmat::BOOLEAN_TYPE,           //"ShowPlot"
	Gmat::INTEGER_TYPE,           //"MaxPlotPoints"
	Gmat::FILENAME_TYPE,				//"JsonFile",

};


//------------------------------------------------------------------------------
// VRInterface(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 * Declaring and initializing default variables here
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

	//different to v1, which is:
	//objectTypeNames.push_back("VRInterface");
	//objectTypeNames.push_back("Plot");

	mViewCoordSystem = NULL; // or nullptr

	jsonFileName = { R"(C:\Users\User\Documents\Unity\JSONs\json.json)" };
	jsonFullPathFileName = { R"(C:\Users\User\Documents\Unity\JSONs\json.json)" };


	mSolverIterOption = SI_NONE;

	mOldName = instanceName;
	mViewCoordSysName = "EarthMJ2000Eq";

	mScRadiiDefault = 50;	//km

	mDataCollectFrequency = 1;
	mUpdatePlotFrequency = 50;
	mNumPointsToRedraw = 0;
	mNumData = 0;
	mNumCollected = 0;
	mMaxData = 20000;
	mDataAbsentWarningCount = 0;

	mScNameArray.clear();
	mCbNameArray.clear();
	mObjectNameArray.clear();
	mAllSpNameArray.clear();
	mAllRefObjectNames.clear();
	mObjectArray.clear();
	mDrawOrbitArray.clear();
	mDrawObjectArray.clear();
	mAllSpArray.clear();
	mCbArray.clear();

	mSpRadii.clear();

	mScXArray.clear();
	mScYArray.clear();
	mScZArray.clear();
	mScVxArray.clear();
	mScVyArray.clear();
	mScVzArray.clear();
	mScQArray.clear();
	mScPrevDataPresent.clear();
	//mScPrevEpoch.clear();
	//mScPrevX.clear();
	//mScPrevY.clear();
	//mScPrevZ.clear();
	//mScPrevVx.clear();
	//mScPrevVy.clear();
	//mScPrevVz.clear();
	//mScPrevQArray.clear();

	mCbXArray.clear();
	mCbYArray.clear();
	mCbZArray.clear();
	mCbVxArray.clear();
	mCbVyArray.clear();
	mCbVzArray.clear();
	mCbQArray.clear();
	mCbPrevDataPresent.clear();
	//mCbPrevEpoch.clear();
	//mCbPrevX.clear();
	//mCbPrevY.clear();
	//mCbPrevZ.clear();
	//mCbPrevVx.clear();
	//mCbPrevVy.clear();
	//mCbPrevVz.clear();
	//mCbPrevQArray.clear();

	mDrawOrbitMap.clear();
	mShowObjectMap.clear();

	mDefaultOrbitColorMap.clear();
	mDefaultTargetColorMap.clear();
	mCurrentOrbitColorMap.clear();
	mCurrentTargetColorMap.clear();

	mAllSpCount = 0;
	mScCount = 0;
	mObjectCount = 0;
	mCbCount = 0;

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
	// Just copy configured object pointer
	mViewCoordSystem = vri.mViewCoordSystem;

	jsonFileName = vri.jsonFileName;
	jsonFullPathFileName = vri.jsonFullPathFileName;

	mSolverIterOption = vri.mSolverIterOption;

	mOldName = vri.mOldName;
	mViewCoordSysName = vri.mViewCoordSysName;

	mScRadiiDefault = vri.mScRadiiDefault;

	mDataCollectFrequency = vri.mDataCollectFrequency;
	mUpdatePlotFrequency = vri.mUpdatePlotFrequency;
	mNumPointsToRedraw = vri.mNumPointsToRedraw;
	mMaxData = vri.mMaxData;

	mAllSpCount = vri.mAllSpCount;
	mScCount = vri.mScCount;
	mObjectCount = vri.mObjectCount;

	mObjectArray = vri.mObjectArray;
	mCbNameArray = vri.mCbNameArray;
	mDrawOrbitArray = vri.mDrawOrbitArray;
	mDrawObjectArray = vri.mDrawObjectArray;
	mAllSpArray = vri.mAllSpArray;
	mScNameArray = vri.mScNameArray;
	mObjectNameArray = vri.mObjectNameArray;
	mAllSpNameArray = vri.mAllSpNameArray;
	mAllRefObjectNames = vri.mAllRefObjectNames;
	mCbArray = vri.mCbArray;

	mSpRadii = vri.mSpRadii;

	mScXArray = vri.mScXArray;
	mScYArray = vri.mScYArray;
	mScZArray = vri.mScZArray;
	mScVxArray = vri.mScVxArray;
	mScVyArray = vri.mScVyArray;
	mScVzArray = vri.mScVzArray;
	mScQArray = vri.mScQArray;
	mScPrevDataPresent = vri.mScPrevDataPresent;
	//mScPrevEpoch = vri.mScPrevEpoch;
	//mScPrevX = vri.mScPrevX;
	//mScPrevY = vri.mScPrevY;
	//mScPrevZ = vri.mScPrevZ;
	//mScPrevVx = vri.mScPrevVx;
	//mScPrevVy = vri.mScPrevVy;
	//mScPrevVz = vri.mScPrevVz;
	//mScPrevQArray = vri.mScPrevQArray;

	mCbXArray = vri.mCbXArray;
	mCbYArray = vri.mCbYArray;
	mCbZArray = vri.mCbZArray;
	mCbVxArray = vri.mCbVxArray;
	mCbVyArray = vri.mCbVyArray;
	mCbVzArray = vri.mCbVzArray;
	mCbQArray = vri.mCbQArray;
	mCbPrevDataPresent = vri.mCbPrevDataPresent;
	//mCbPrevEpoch = vri.mCbPrevEpoch;
	//mCbPrevX = vri.mCbPrevX;
	//mCbPrevY = vri.mCbPrevY;
	//mCbPrevZ = vri.mCbPrevZ;
	//mCbPrevVx = vri.mCbPrevVx;
	//mCbPrevVy = vri.mCbPrevVy;
	//mCbPrevVz = vri.mCbPrevVz;
	//mCbPrevQArray = vri.mCbPrevQArray;

	mDrawOrbitMap = vri.mDrawOrbitMap;
	mShowObjectMap = vri.mShowObjectMap;

	mDefaultOrbitColorMap = vri.mDefaultOrbitColorMap;
	mDefaultTargetColorMap = vri.mDefaultTargetColorMap;
	mCurrentOrbitColorMap = vri.mCurrentOrbitColorMap;
	mCurrentTargetColorMap = vri.mCurrentTargetColorMap;

	mNumData = vri.mNumData;
	mNumCollected = vri.mNumCollected;
	mDataAbsentWarningCount = vri.mDataAbsentWarningCount;

	parameterCount = VRInterfaceParamCount; 
	// this is not in version 1
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

	// Just copy configured object pointer
	mViewCoordSystem = vri.mViewCoordSystem;

	mSolverIterOption = vri.mSolverIterOption;

	jsonFileName = vri.jsonFileName;
	jsonFullPathFileName = vri.jsonFullPathFileName;

	mOldName = vri.mOldName;
	mViewCoordSysName = vri.mViewCoordSysName;

	mScRadiiDefault = vri.mScRadiiDefault;

	mDataCollectFrequency = vri.mDataCollectFrequency;
	mUpdatePlotFrequency = vri.mUpdatePlotFrequency;
	mNumPointsToRedraw = vri.mNumPointsToRedraw;
	mMaxData = vri.mMaxData;

	mAllSpCount = vri.mAllSpCount;
	mScCount = vri.mScCount;
	mObjectCount = vri.mObjectCount;

	mObjectArray = vri.mObjectArray;
	mCbNameArray = vri.mCbNameArray;
	mDrawOrbitArray = vri.mDrawOrbitArray;
	mDrawObjectArray = vri.mDrawObjectArray;
	mAllSpArray = vri.mAllSpArray;
	mScNameArray = vri.mScNameArray;
	mObjectNameArray = vri.mObjectNameArray;
	mAllSpNameArray = vri.mAllSpNameArray;
	mAllRefObjectNames = vri.mAllRefObjectNames;
	mCbArray = vri.mCbArray;

	mSpRadii = vri.mSpRadii;

	mScXArray = vri.mScXArray;
	mScYArray = vri.mScYArray;
	mScZArray = vri.mScZArray;
	mScVxArray = vri.mScVxArray;
	mScVyArray = vri.mScVyArray;
	mScVzArray = vri.mScVzArray;
	mScQArray = vri.mScQArray;
	mScPrevDataPresent = vri.mScPrevDataPresent;
	//mScPrevEpoch = vri.mScPrevEpoch;
	//mScPrevX = vri.mScPrevX;
	//mScPrevY = vri.mScPrevY;
	//mScPrevZ = vri.mScPrevZ;
	//mScPrevVx = vri.mScPrevVx;
	//mScPrevVy = vri.mScPrevVy;
	//mScPrevVz = vri.mScPrevVz;
	//mScPrevQArray = vri.mScPrevQArray;

	mCbXArray = vri.mCbXArray;
	mCbYArray = vri.mCbYArray;
	mCbZArray = vri.mCbZArray;
	mCbVxArray = vri.mCbVxArray;
	mCbVyArray = vri.mCbVyArray;
	mCbVzArray = vri.mCbVzArray;
	mCbQArray = vri.mCbQArray;
	mCbPrevDataPresent = vri.mCbPrevDataPresent;
	//mCbPrevEpoch = vri.mCbPrevEpoch;
	//mCbPrevX = vri.mCbPrevX;
	//mCbPrevY = vri.mCbPrevY;
	//mCbPrevZ = vri.mCbPrevZ;
	//mCbPrevVx = vri.mCbPrevVx;
	//mCbPrevVy = vri.mCbPrevVy;
	//mCbPrevVz = vri.mCbPrevVz;
	//mCbPrevQArray = vri.mCbPrevQArray;

	mDrawOrbitMap = vri.mDrawOrbitMap;
	mShowObjectMap = vri.mShowObjectMap;

	mDefaultOrbitColorMap = vri.mDefaultOrbitColorMap;
	mDefaultTargetColorMap = vri.mDefaultTargetColorMap;
	mCurrentOrbitColorMap = vri.mCurrentOrbitColorMap;
	mCurrentTargetColorMap = vri.mCurrentTargetColorMap;

	mNumData = vri.mNumData;
	mNumCollected = vri.mNumCollected;
	mDataAbsentWarningCount = vri.mDataAbsentWarningCount;

	return *this;
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
	jstream.flush();
	jstream.close();
	// clear buffers, delete cache data etc.
}




//---------------------------------------------------------------
// Methods inherited from GmatBase
//---------------------------------------------------------------


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
	// Subscriber does not implement a validate

	//return GmatBase::Validate();
	return true;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
/**
* Set pointers and assignment. Initialise instance of interface.
* Usually this would be called from OrbitView, so it's initilisation
* is mirrored here, after OrbitPlot Initialisation
*/
//------------------------------------------------------------------------------
bool VRInterface::Initialize() {
	bool foundSc = false;
	bool retval = false;

	if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)
		// do nothing 
		return true;

	Subscriber::Initialize(); // all others have this. Why?

	if (active && !isInitialized) {
		// if subscriber active, and this not yet initialized
		// stuff to do on start of mission run happens here 
		// careful with this if statement. Moved from lower down

		Integer nullCounter = 0;

		if (mAllSpCount == 0)
		{
			active = false;
			MessageInterface::ShowMessage
			("*** WARNING *** The %s named \"%s\" will be turned off. "
				"No SpacePoints were added to plot.\n", GetTypeName().c_str(), GetName().c_str());
			return false;
		}

		// check for spacecaft is included in the plot
		for (int i = 0; i < mAllSpCount; i++)
		{
			if (mAllSpArray[i])
			{
				if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
				{
					foundSc = true;
					break;
				}
			}
			else
				nullCounter++;
		}

		if (nullCounter == mAllSpCount)
		{
			active = false;
			MessageInterface::ShowMessage
			("*** WARNING *** The %s named \"%s\" will be turned off.  "
				"%d SpaceObjects have NULL pointers.\n", GetTypeName().c_str(),
				GetName().c_str(), nullCounter);
			return false;
		}

		if (!foundSc)
		{
			active = false;
			MessageInterface::ShowMessage
			("*** WARNING *** The %s named \"%s\" will be turned off. "
				"No Spacecraft was added to plot.\n", GetTypeName().c_str(), GetName().c_str());
			return false;
		}

		// Build color maps

		for (int i = 0; i < mAllSpCount; i++)
		{
			SpacePoint *sp = mAllSpArray[i];

			if (sp)
			{

				mDefaultOrbitColorMap[mAllSpArray[i]->GetName()] = mAllSpArray[i]->GetCurrentOrbitColor();
				mDefaultTargetColorMap[mAllSpArray[i]->GetName()] = mAllSpArray[i]->GetCurrentTargetColor();
			}
		}

		// Save to current color map
		mCurrentOrbitColorMap = mDefaultOrbitColorMap;
		mCurrentTargetColorMap = mDefaultTargetColorMap;

		// if statement used to be here

		ClearDynamicArrays();
		BuildDynamicArrays();
		DataManager bdg;
		bdg.BuildDynamicBuffers(mObjectCount);

		// all coordinate system stuff set here
		// OrbitView set max points to plot here
		// objects array and pointers set here in OrbitView
		// update frequency set here
		// GlPlot initialized here 

		isInitialized = true;
		retval = true;
	}
	else {
		if (!active) {
			retval = false;
		}
	}
	return retval;
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

//

//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName = "")
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool VRInterface::SetName(const std::string &who, const std::string &oldName)
{


	if (oldName == "")
		mOldName = instanceName;
	else
		mOldName = oldName;

	return GmatBase::SetName(who);
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


	if (action == "Clear")
	{
		return ClearSpacePointList();
	}
	else if (action == "Remove")
	{
		return RemoveSpacePoint(actionData);
	}
	else if (action == "Finalize")
	{
		// This action is usually called when GMAT function finalizes
		// PlotInterface::DeleteGlPlot(instanceName);
	}
	else if (action == "PenUp")
	{
		isDataOn = false;
		return true;
	}
	else if (action == "PenDown")
	{
		isDataOn = true;
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
* Resets the reference object name when the reference object
* is renamed elsewhere.
*/
//------------------------------------------------------------------------------

bool VRInterface::RenameRefObject(const UnsignedInt type,
	const std::string &oldName,
	const std::string &newName)
{


	if (type == Gmat::SPACECRAFT || type == Gmat::GROUND_STATION ||
		type == Gmat::CALCULATED_POINT)
	{
		// for spacecraft name
		for (int i = 0; i < mAllSpCount; i++)
		{

			if (mAllSpNameArray[i] == oldName)
				mAllSpNameArray[i] = newName;
		}

		//----------------------------------------------------
		// Since spacecraft name is used as key for showing
		// and drawing object map, I can't change the key name,
		// so it is removed and inserted with new name.
		//----------------------------------------------------

		std::map<std::string, bool>::iterator drawOrbitPos, showObjectPos;
		drawOrbitPos = mDrawOrbitMap.find(oldName);
		showObjectPos = mShowObjectMap.find(oldName);
		if (drawOrbitPos != mDrawOrbitMap.end() &&
			showObjectPos != mShowObjectMap.end())
		{

			mDrawOrbitMap[newName] = mDrawOrbitMap[oldName];
			mShowObjectMap[newName] = mShowObjectMap[oldName];
			mDrawOrbitMap.erase(drawOrbitPos);
			mShowObjectMap.erase(showObjectPos);


		}

		//----------------------------------------------------
		// Since object name is used as key for object default
		// color map, I can't change the key name, so it is
		// removed and inserted with new name
		//----------------------------------------------------

		std::map<std::string, UnsignedInt>::iterator defOrbColorPos, defTargColorPos;
		defOrbColorPos = mDefaultOrbitColorMap.find(oldName);
		defTargColorPos = mDefaultTargetColorMap.find(oldName);

		if (defOrbColorPos != mDefaultOrbitColorMap.end() &&
			defTargColorPos != mDefaultTargetColorMap.end())
		{

			// add new spacecraft name key and delete old
			mDefaultOrbitColorMap[newName] = mDefaultOrbitColorMap[oldName];
			mDefaultTargetColorMap[newName] = mDefaultTargetColorMap[oldName];
			mDefaultOrbitColorMap.erase(defOrbColorPos);
			mDefaultTargetColorMap.erase(defTargColorPos);


		}

		//----------------------------------------------------
		// Since object name is used as key for object current
		// color map, I can't change the key name, so it is
		// removed and inserted with new name
		//----------------------------------------------------

		std::map<std::string, UnsignedInt>::iterator curOrbColorPos, curTargColorPos;
		curOrbColorPos = mCurrentOrbitColorMap.find(oldName);
		curTargColorPos = mCurrentTargetColorMap.find(oldName);

		if (curOrbColorPos != mCurrentOrbitColorMap.end() &&
			curTargColorPos != mCurrentTargetColorMap.end())
		{

			// Add new object name key and delete old
			mCurrentOrbitColorMap[newName] = mCurrentOrbitColorMap[oldName];
			mCurrentTargetColorMap[newName] = mCurrentTargetColorMap[oldName];
			mCurrentOrbitColorMap.erase(curOrbColorPos);
			mCurrentTargetColorMap.erase(curTargColorPos);
		}
	}
	else if (type == Gmat::COORDINATE_SYSTEM)
	{
		if (mViewCoordSysName == oldName)
			mViewCoordSysName = newName;
	}


	return true;
}


//---------------------------------------------------------------
// Methods inherited from Subscriber
//---------------------------------------------------------------

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
	return Subscriber::Activate(state);
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
	#ifdef DEBUG_DISTRIBUTE
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

	if (isEndOfRun) {	// this runs twice
			DataManager wtj;
			wtj.WriteToJson(jsonFileName, jsonFullPathFileName, jstream, mScCount, mCbCount,
				mScNameArray, mCbNameArray, mSpRadii,
				mDefaultOrbitColorMap, mMaxData);
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

	// conditions passed, start buffering data

	if (len >= 7) {
		// there is mission data in the stream, do main implementation here
		// publish data

		DataControl(dat, len);
		return true;
	}

	// what if 0<len<7 -- test this


	return true; // Distribute must always return true
					 // so that ReceiveData() is called.
}

//------------------------------------------------------------------------------
// Methods for parameters
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string VRInterface::GetParameterText(const Integer id) const
{
	if (id >= SubscriberParamCount && id < VRInterfaceParamCount)
		return PARAMETER_TEXT[id - SubscriberParamCount];
	else
		return Subscriber::GetParameterText(id);

}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer VRInterface::GetParameterID(const std::string &str) const
{
	if (str == "OrbitColor" || str == "TargetColor")
		return Gmat::PARAMETER_REMOVED;

	for (int i = SubscriberParamCount; i < VRInterfaceParamCount; i++)
	{
		if (str == PARAMETER_TEXT[i - SubscriberParamCount])
			return i;
	}

	return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType VRInterface::GetParameterType(const Integer id) const
{
	if (id >= SubscriberParamCount && id < VRInterfaceParamCount)
		return PARAMETER_TYPE[id - SubscriberParamCount];
	else
		return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string VRInterface::GetParameterTypeString(const Integer id) const
{
	return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
// Identify read-only parameters
bool VRInterface::IsParameterReadOnly(const Integer id) const
{
	// if I want these to be read only
	//if (id == READ_ONLY_PARAMETER || id == READ_ONLY_PARAMETER)
	//	return true;

	return Subscriber::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer VRInterface::GetIntegerParameter(const Integer id) const
{
	switch (id)
	{
	case DATA_COLLECT_FREQUENCY:
		return mDataCollectFrequency;
	case UPDATE_PLOT_FREQUENCY:
		return mUpdatePlotFrequency;
	case NUM_POINTS_TO_REDRAW:
		return mNumPointsToRedraw;
	case MAX_DATA:
		return mMaxData;
	case SC_RADII:
		return mScRadiiDefault;
	default:
		return Subscriber::GetIntegerParameter(id);
	}
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer VRInterface::GetIntegerParameter(const std::string &label) const
{
	return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer VRInterface::SetIntegerParameter(const Integer id, const Integer value)
{
	switch (id)
	{
	case DATA_COLLECT_FREQUENCY:
		if (value > 0)
		{
			mDataCollectFrequency = value;
			return value;
		}
		else
		{
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 1).c_str(),
				"DataCollectFrequency", "Integer Number > 0");
			throw se;
		}
	case UPDATE_PLOT_FREQUENCY:
		if (value > 0)
		{
			mUpdatePlotFrequency = value;
			return value;
		}
		else
		{
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 1).c_str(),
				"UpdatePlotFrequency", "Integer Number > 0");
			throw se;
		}
	case NUM_POINTS_TO_REDRAW:
		if (value >= 0)
		{
			mNumPointsToRedraw = value;
			return value;
		}
		else
		{
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 1).c_str(),
				"NumPointsToRedraw", "Integer Number >= 0");
			throw se;
		}
	case MAX_DATA:
		if (value > 0)
		{
			mMaxData = value;
			return value;
		}
		else
		{
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 1).c_str(),
				"MaxPlotPoints", "Integer Number > 0");
			throw se;
		}
	case SC_RADII:
		if (value > 0) {
			mScRadiiDefault = value;
		}			
		else {
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				GmatStringUtil::ToString(value, 1).c_str(),
				"SpacecraftRadii", "Integer Number > 0");
			throw se;
		}
		return value;
	default:
		return Subscriber::SetIntegerParameter(id, value);
	}
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer VRInterface::SetIntegerParameter(const std::string &label,
	const Integer value)
{
	return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string VRInterface::GetStringParameter(const Integer id) const
{


	switch (id) {
		case ADD:
			return GetObjectStringList();
		case COORD_SYSTEM:
			return mViewCoordSysName;
		case JSON_FILE:
			return jsonFileName;
		default:
			return Subscriber::GetStringParameter(id);
	}
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string VRInterface::GetStringParameter(const std::string &label) const
{


	return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
//Sets the string value of the parameter with the specied ID.
bool VRInterface::SetStringParameter(const Integer id, const std::string &value)
{


	switch (id)
	{
	case COORD_SYSTEM:
		mViewCoordSysName = value;
		return true;
	case ADD:
	{
		if (value[0] == '{')
		{
			try
			{
				TextParser tp;
				ClearSpacePointList();
				StringArray spList = tp.SeparateBrackets(value, "{}", ",");
				for (UnsignedInt i = 0; i < spList.size(); i++)
					AddSpacePoint(spList[i], mAllSpCount);
				return true;
			}
			catch (BaseException &)
			{
				SubscriberException se;
				se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
					"Add", "Valid CelestialBody list");
				throw se;
			}
		}
		else
		{
			return AddSpacePoint(value, mAllSpCount);
		}
	}
	case JSON_FILE: {

		// do validation here
		if (!GmatFileUtil::IsValidFileName(value))
		{
			std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Filename", msg.c_str());
			throw se;
		}
		jsonFileName = value;
		if (jsonFileName == "") {
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
				"JSON_FILE");
			throw se;
		}
		jsonFullPathFileName = GmatBase::GetFullPathFileName(jsonFileName, GetName(), jsonFileName, "JSON_FILE",
			false, ".json", true);

		// Check for invalid output directory
		if (jsonFullPathFileName == "")
		{
			lastErrorMessage = FileManager::Instance()->GetLastFilePathMessage();
			throw SubscriberException(lastErrorMessage);
		}
		return true;
	}


	// case DRAW_LINE_COLOUR:

	default:
		return Subscriber::SetStringParameter(id, value);
	}
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool VRInterface::SetStringParameter(const std::string &label,
	const std::string &value)
{
	return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
// Handle string parameters here
//
bool VRInterface::SetStringParameter(const Integer id, const std::string &value,
	const Integer index)
{
	switch (id)
	{
	case COORD_SYSTEM:
		mViewCoordSysName = value;
		return true;
	case ADD:
	{
		if (value[0] == '{')
		{
			try
			{
				TextParser tp;
				ClearSpacePointList();
				StringArray spList = tp.SeparateBrackets(value, "{}", ",");
				for (UnsignedInt i = 0; i < spList.size(); i++)
					AddSpacePoint(spList[i], mAllSpCount);
				return true;
			}
			catch (BaseException &)
			{
				SubscriberException se;
				se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
					"Add", "Valid CelestialBody list");
				throw se;
			}
		}
		else
		{
			return AddSpacePoint(value, mAllSpCount);
		}
	}
	//case ORBIT_COLOR:
	//case TARGET_COLOR:
		//if (value[0] == '[')
		//	PutUnsignedIntValue(id, value);
		//return true;
	default:
		return Subscriber::SetStringParameter(id, value);
	}
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool VRInterface::SetStringParameter(const std::string &label,
	const std::string &value,
	const Integer index)
{


	return SetStringParameter(GetParameterID(label), value, index);
}



//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& VRInterface::GetStringArrayParameter(const Integer id) const
{
	switch (id)
	{
	case ADD:
		return mAllSpNameArray;
	default:
		return Subscriber::GetStringArrayParameter(id);
	}
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& VRInterface::GetStringArrayParameter(const std::string &label) const
{
	return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool VRInterface::GetBooleanParameter(const Integer id) const
{
	if (id == SHOW_PLOT)
		return active;
	return Subscriber::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool VRInterface::GetBooleanParameter(const std::string &label) const
{
	return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool VRInterface::SetBooleanParameter(const Integer id, const bool value)
{


	if (id == SHOW_PLOT)
	{
		active = value;
		return active;
	}

	return Subscriber::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
bool VRInterface::SetBooleanParameter(const std::string &label, const bool value)
{
	return SetBooleanParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
const BooleanArray& VRInterface::GetBooleanArrayParameter(const Integer id) const
{
	switch (id) {
	//case DRAW_OBJECT:
	//{
	//	return mDrawObjectArray;
	//}
	//case DRAW_LABEL:
	//{
	//	return mDrawLabelArray;
	//}
	default:
	{
		return Subscriber::GetBooleanArrayParameter(id);
	}
	}
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
const BooleanArray& VRInterface::GetBooleanArrayParameter(const std::string &label) const
{
	Integer id = GetParameterID(label);
	return GetBooleanArrayParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const Integer id, const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool VRInterface::SetBooleanArrayParameter(const Integer id,
	const BooleanArray &valueArray)
{


	//if (id == DRAW_OBJECT)
	//{


	//	// Check size of arrays in Initialize() or Interpreter::FinalPass()?
	//	//if (mAllSpNameArray.size() != valueArray.size())
	//	//   throw SubscriberException
	//	//      ("The count doesn't match with added objects" + GetParameterText(id));

	//	mDrawObjectArray = valueArray;
	//	Integer minCount = mAllSpNameArray.size() < mDrawObjectArray.size() ?
	//		mAllSpNameArray.size() : mDrawObjectArray.size();

	//	// GUI uses mShowObjectMap so update it
	//	for (Integer i = 0; i < minCount; i++)
	//	{
	//		bool tf = mDrawObjectArray[i];
	//		mShowObjectMap[mAllSpNameArray[i]] = tf;
	//	}
	//	return true;
	//}
	return Subscriber::SetBooleanArrayParameter(id, valueArray);
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const std::string &label,
//                                const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool VRInterface::SetBooleanArrayParameter(const std::string &label,
	const BooleanArray &valueArray)
{
	Integer id = GetParameterID(label);
	return SetBooleanArrayParameter(id, valueArray);
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string VRInterface::GetRefObjectName(const UnsignedInt type) const
{
	if (type == Gmat::COORDINATE_SYSTEM)
	{
		return mViewCoordSysName; //just return this
	}

	return Subscriber::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool VRInterface::HasRefObjectTypeArray()
{
	return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
 //------------------------------------------------------------------------------
const ObjectTypeArray& VRInterface::GetRefObjectTypeArray()
{
	refObjectTypes.push_back(Gmat::SPACE_POINT);
	return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& VRInterface::GetRefObjectNameArray(const UnsignedInt type)
{

	if (type == Gmat::COORDINATE_SYSTEM || type == Gmat::UNKNOWN_OBJECT)
	{
		refObjectNames.push_back(mViewCoordSysName);
	}

	if (type == Gmat::SPACE_POINT || type == Gmat::UNKNOWN_OBJECT)
	{
		refObjectNames.insert(refObjectNames.end(), mAllSpNameArray.begin(),
			mAllSpNameArray.end());
	}


	return refObjectNames;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* VRInterface::GetRefObject(const UnsignedInt type,
	const std::string &name)
{
	if (type == Gmat::COORDINATE_SYSTEM)
	{
		if (name == mViewCoordSysName)
			return mViewCoordSystem;
	}

	return Subscriber::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Set reference object pointer.
 *
 * @param <obj>  Reference object pointer to set to given object type and name
 * @param <type> Reference object type
 * @param <name> Reference object name
 */
 //------------------------------------------------------------------------------
bool VRInterface::SetRefObject(GmatBase *obj, const UnsignedInt type,
	const std::string &name)
{


	if (obj == NULL)
	{

		return false;
	}

	std::string realName = name;
	if (name == "")
		realName = obj->GetName();

	if (obj->IsOfType(Gmat::SPACE_POINT))
	{


		for (Integer i = 0; i < mAllSpCount; i++)
		{


			if (mAllSpNameArray[i] == realName)
			{


				mAllSpArray[i] = (SpacePoint*)(obj);
			}
		}

		// If spacecraft, save initial epoch so that data will not be buffered before
		// the initial epoch
		if (obj->IsOfType(Gmat::SPACECRAFT))
		{
			SpaceObject *so = (SpaceObject*)(obj);

			mScInitialEpochMap[so->GetName()] = so->GetEpoch();
		}


		return true;
	}

	if (type == Gmat::COORDINATE_SYSTEM)
	{
		if (realName == mViewCoordSysName)
			mViewCoordSystem = (CoordinateSystem*)obj;


		return true;
	}


	return Subscriber::SetRefObject(obj, type, realName);
}

//------------------------------------------------------------------------------
// virtual void SetOrbitColorChanged(GmatBase *originator, const std::string &newColor, ...)
//------------------------------------------------------------------------------
/**
 * Sets object orbit color change.
 *
 * @param originator  The assignment command pointer who is setting
 * @param newColor  New color to be applied to the object
 * @param objName  Name of the object
 * @param desc  Description of property change
 * @param isSpacecraft Set to true if object is a Spcecraft
 */
 //------------------------------------------------------------------------------
void VRInterface::SetOrbitColorChanged(GmatBase *originator,
	const std::string &newColor,
	const std::string &objName,
	const std::string &desc)
{


	UnsignedInt intColor = RgbColor::ToIntColor(newColor);
	mDefaultOrbitColorMap[objName] = intColor;
	mCurrentOrbitColorMap[objName] = intColor;


}


//------------------------------------------------------------------------------
// virtual void SetTargetColorChanged(GmatBase *originator, const std::string &newColor, ...)
//------------------------------------------------------------------------------
/**
 * Sets object target color change.
 *
 * @param originator  The assignment command pointer who is setting
 * @param newColor  New color to be applied to the object
 * @param objName  Name of the object
 * @param desc  Description of property change
 * @param isSpacecraft Set to true if object is a Spcecraft
 */
 //------------------------------------------------------------------------------
void VRInterface::SetTargetColorChanged(GmatBase *originator,
	const std::string &newColor,
	const std::string &objName,
	const std::string &desc)
{


	UnsignedInt intColor = RgbColor::ToIntColor(newColor);
	mDefaultTargetColorMap[objName] = intColor;
	mCurrentTargetColorMap[objName] = intColor;


}

//------------------------------------------------------------------------------
// virtual void SetSegmentOrbitColor(GmatBase *originator, bool overrideColor,
//                           UnsignedInt orbitColor)
//------------------------------------------------------------------------------
/**
 * Sets propagation segment orbit color so that subscribers can handle appropriately.
 *
 * @param originator  The Propagate command pointer who is setting
 * @param overrideColor  The flag indicating whether or not to override orbit color
 * @param orbitColor  New orbit color to be applied to the space object
 */
 //------------------------------------------------------------------------------
void VRInterface::SetSegmentOrbitColor(GmatBase *originator, bool overrideColor,
	UnsignedInt orbitColor, const StringArray &objNames)
{


	// If overrding color, set the same color for all objects
	if (overrideColor)
	{
		for (UnsignedInt i = 0; i < objNames.size(); i++)
			mCurrentOrbitColorMap[objNames[i]] = orbitColor;
	}
	else
	{
		for (UnsignedInt i = 0; i < objNames.size(); i++)
		{
			std::string objName = objNames[i];
			mCurrentOrbitColorMap[objName] = mDefaultOrbitColorMap[objName];
		}
	}


}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
UnsignedInt VRInterface::GetPropertyObjectType(const Integer id) const
{
	if (id == ADD)
		return Gmat::SPACE_POINT;

	return Subscriber::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// bool GetShowObject(const std::string &name)
//------------------------------------------------------------------------------
bool VRInterface::GetShowObject(const std::string &name)
{


	return mShowObjectMap[name];
}


//------------------------------------------------------------------------------
// void SetShowObject(const std::string &name, bool value)
//------------------------------------------------------------------------------
void VRInterface::SetShowObject(const std::string &name, bool value)
{


	mShowObjectMap[name] = value;
	if (value)
		mDrawOrbitMap[name] = value;

	if (mShowObjectMap.find(name) != mShowObjectMap.end())
	{
		for (int i = 0; i < mAllSpCount; i++)
			if (mAllSpNameArray[i] == name)
				mDrawObjectArray[i] = value;
	}
}



//------------------------------------------------------------------------------
// Internal Methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// const StringArray& GetSpacePointList()
//------------------------------------------------------------------------------
const StringArray& VRInterface::GetSpacePointList()
{

	return mAllSpNameArray;
}

//------------------------------------------------------------------------------
// const StringArray& GetNonSpacecraftList()
//------------------------------------------------------------------------------
const StringArray& VRInterface::GetNonSpacecraftList()
{
	return mObjectNameArray;
}


//------------------------------------------------------------------------------
// Protected Methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool DataControl(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool VRInterface::DataControl(const Real *dat, Integer len)
{
	if (len == 0)
		return true;

	// Skip data if data publishing command such as Propagate is inside a function
	// and this VRInterface is not a global nor a local object (i.e declared in the main script)
	// (LOJ: 2015.08.17)
	if (currentProvider && currentProvider->TakeAction("IsInFunction"))
	{
		bool skipData = false;
		// Check for spacepoints if data should be skipped or not
		for (int i = 0; i < mAllSpCount; i++)
		{
			SpacePoint *sp = mAllSpArray[i];

			if (sp)
			{

				// Skip data if VRInterface is global and spacepoint is local
				if (IsGlobal() && sp->IsLocal())
				{

					skipData = true;
					break;
				}
				// Skip data if spacepoint is not a global nor a local object
				else if (!(sp->IsGlobal()) && !(sp->IsLocal()))
				{
					skipData = true;
					break;
				}
			}
		}

		if (skipData)
		{
			return true;
		}
	}

	mNumData++;

	// Buffer data if data collect frequency is met or fist data
	if ((mNumData % mDataCollectFrequency) == 0 || (mNumData == 1))
	{
		bool status = (BufferOrbitData(dat, len) && 
							BufferCelestialBodyData(dat, len));

		// numCollected increments by one in BufferOrbitData(...)

		// if solving and plotting current iteration just return
		if (status == false)
			return true;

		bool solving = false;
		if (runstate == Gmat::SOLVING)
			solving = true;

		bool inFunction = false;
		if (currentProvider && currentProvider->TakeAction("IsInFunction"))
			inFunction = true;

		bool update = (mNumCollected % mUpdatePlotFrequency) == 0;

		// publish final solution data to plotter/data manager
		DataManager atb;
		atb.AddToBuffer(dat[0], 
			mScCount, mCbCount, mScNameArray, mCbNameArray,
			mScXArray, mScYArray, mScZArray,
			mScVxArray, mScVyArray, mScVzArray,
			mCbXArray, mCbYArray, mCbZArray,
			mCbVxArray, mCbVyArray, mCbVzArray,
			solving,	mSolverIterOption, update, isDataOn, mMaxData, inFunction);


		if (update)
			mNumCollected = 0;
	}


	return true;
}


//------------------------------------------------------------------------------
// Integer BufferOrbitData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * @return true if continue to updating plot
 *         false if solving and plotting current iteration
 */
 //------------------------------------------------------------------------------
bool VRInterface::BufferOrbitData(const Real * dat, Integer len)
{
	mNumCollected++;

	// @note
	// New Publisher code doesn't assign currentProvider anymore,
	// it just copies current labels. There was an issue with
	// provider id keep incrementing if data is regisgered and
	// published inside a GmatFunction

	StringArray dataLabels = theDataLabels[0];

	// method only applies to spacecraft 

	Integer idX, idY, idZ;
	Integer idVx, idVy, idVz;
	Integer scIndex = -1;

	for (Integer i = 0; i < mScCount; i++)
		// iterates for all spacecraft
	{
		idX = FindIndexOfElement(dataLabels, mScNameArray[i] + ".X");
		idY = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Y");
		idZ = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Z");

		idVx = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vx");
		idVy = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vy");
		idVz = FindIndexOfElement(dataLabels, mScNameArray[i] + ".Vz");

		//append quat to this

		scIndex++;

		// If any of index not found, handle absent data and continue with the next spacecraft
		if (idX == -1 || idY == -1 || idZ == -1 ||
			idVx == -1 || idVy == -1 || idVz == -1)
		{
			// handle absent data
			mScPrevDataPresent[scIndex] = false;
			continue;
		}

		// If distributed data coordinate system is different from view
		// coordinate system, convert data here.
		// If we convert after current epoch, it will not give correct
		// results, if origin is spacecraft,
		// ie, sat->GetMJ2000State(epoch) will not give correct results.
		if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
			(mViewCoordSystem != theDataCoordSystem))
		{

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
		//mScPrevEpoch[scIndex] = dat[0];
		//mScPrevX[scIndex] = mScXArray[scIndex];
		//mScPrevY[scIndex] = mScYArray[scIndex];
		//mScPrevZ[scIndex] = mScZArray[scIndex];
		//mScPrevVx[scIndex] = mScVxArray[scIndex];
		//mScPrevVy[scIndex] = mScVyArray[scIndex];
		//mScPrevVz[scIndex] = mScVzArray[scIndex];

		// put quats in here!!
	}

	if (mSolverIterOption == SI_CURRENT)
	{
		// save data when targeting or last iteration
		if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS)
		{
			return true; // careful having this here
		}

		if (runstate == Gmat::SOLVING)
		{
			return false;	// solving 
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// bool BufferCelestialBodyData(const Real *dat, Integer len)
//------------------------------------------------------------------------------
/**
 * @return 1 if continue to updating plot
 *         2 if solving and plotting current iteration
 */
 //------------------------------------------------------------------------------
bool VRInterface::BufferCelestialBodyData(const Real *dat, Integer len)
{
	bool viewRotMatComputed = false;
	bool eclipticRotMatComputed = false;

	Integer cbIndex = -1;

	for (int i = 0; i < mCbCount; i++)	//iterate through all celestial bodies
	{
		cbIndex++; 

		SpacePoint *cb = mCbArray[i];	// can also try with mObjectArray

		Rvector6 cbMjEqState;
		try
		{
			cbMjEqState = cb->GetMJ2000State(dat[0]);	// assuming dat[0] is time
			// what coordinate system is this in???
		}
		catch (BaseException &)
		{
			SubscriberException se;
			se.SetDetails(errorMessageFormat.c_str(),
				"Error getting Cb state");
			throw se;
		}

		// If distributed data coordinate system is different from view
		// coordinate system, convert data here.
		// If we convert after current epoch, it will not give correct
		// results, if origin is spacecraft,
		// ie, sat->GetMJ2000State(epoch) will not give correct results.

		if ((theDataCoordSystem != NULL && mViewCoordSystem != NULL) &&
			(mViewCoordSystem != theDataCoordSystem)) {
			CoordinateConverter coordConverter;
			Rvector6 outState;

			coordConverter.Convert(dat[0], cbMjEqState, theDataCoordSystem,
				outState, mViewCoordSystem);

			mCbXArray[cbIndex] = outState[0];
			mCbYArray[cbIndex] = outState[1];
			mCbZArray[cbIndex] = outState[2];
			mCbVxArray[cbIndex] = outState[3];	// are we even sure this is V??
			mCbVyArray[cbIndex] = outState[4];
			mCbVzArray[cbIndex] = outState[5];
		}
		else {
			mCbXArray[cbIndex] = cbMjEqState[0];
			mCbYArray[cbIndex] = cbMjEqState[1];
			mCbZArray[cbIndex] = cbMjEqState[2];
			mCbVxArray[cbIndex] = cbMjEqState[3];	// are we even sure this is V??
			mCbVyArray[cbIndex] = cbMjEqState[4];
			mCbVzArray[cbIndex] = cbMjEqState[5];
		}

		// Save old data for next time
		mCbPrevDataPresent[cbIndex] = true;
		//mCbPrevEpoch[cbIndex] = dat[0];
		//mCbPrevX[cbIndex] = mCbXArray[cbIndex];
		//mCbPrevY[cbIndex] = mCbYArray[cbIndex];
		//mCbPrevZ[cbIndex] = mCbZArray[cbIndex];
		//mCbPrevVx[cbIndex] = mCbVxArray[cbIndex];
		//mCbPrevVy[cbIndex] = mCbVyArray[cbIndex];
		//mCbPrevVz[cbIndex] = mCbVzArray[cbIndex];
	}

	// skipped out view coordinate system stuff here 
			
	// if has attitude, update attitude here

	// skipped out Current trajecotry implementation

	// this data control potentially doesn't have to happen as the same check is performed in BufferOrbitData

	if (mSolverIterOption == SI_CURRENT) {
		// save data when targeting or last iteration
		if (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS) {
			return true; // careful having this here
		}
		if (runstate == Gmat::SOLVING) {
			return false;	// solving 
		}
	}

	return true;
}



//------------------------------------------------------------------------------
// virtual std::string GetObjectStringList() const
//------------------------------------------------------------------------------
/**
 * Formats object names into {}. This includes all SpacePoint objects to list.
 * If subclass should not include all objects, then this method should be
 * overridden in the subclass.
 * This protected method is inherited from the Subscriber
 */
 //------------------------------------------------------------------------------
std::string VRInterface::GetObjectStringList() const
{
	MessageInterface::ShowMessage("VRInterface::GetObjectStringList() entered\n");

	Integer objCount = mAllSpNameArray.size();
	std::string objList = "{ ";
	for (Integer i = 0; i < objCount; i++)
	{
		if (i == objCount - 1)
			objList += mAllSpNameArray[i];
		else
			objList += mAllSpNameArray[i] + ", ";
	}
	objList += " }";
	return objList;
}


//------------------------------------------------------------------------------
// bool AddSpacePoint(const std::string &name, Integer index, bool show = true)
//------------------------------------------------------------------------------
// Adds Spacecraft and other objects to object arrays
bool VRInterface::AddSpacePoint(const std::string &name, Integer index, bool show)
{


	// if name not in the list, add
	if (find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name) ==
		mAllSpNameArray.end())
	{
		// Do we want to add any new object here? Like Sun in the following.
		// VRInterface.Add = {DefaultSC, Earth};
		// VRInterface.Add = {Sun};
		// If yes, we should not check for index. Just commenting out for now (LOJ: 2011.01.14)
		//if (name != "" && index == mAllSpCount)
		if (name != "")
		{
			mAllSpNameArray.push_back(name);
			mAllSpArray.push_back(NULL);
			mAllSpCount = mAllSpNameArray.size();

			mDrawOrbitMap[name] = show;
			mShowObjectMap[name] = show;

			// Initially set to white
			mDefaultOrbitColorMap[name] = GmatColor::WHITE;
			mDefaultTargetColorMap[name] = GmatColor::WHITE;

			// Ignore array value more than actual map size
			if (mDrawObjectArray.size() < mShowObjectMap.size())
				mDrawObjectArray.push_back(true); //added (LOJ: 2011.01.13 for bug 2215 fix)
		}
	}



	return true;
}



//------------------------------------------------------------------------------
// bool ClearSpacePointList()
//------------------------------------------------------------------------------
// Clears all object arrays called from TakeAction("Clear")
bool VRInterface::ClearSpacePointList()
{
	//updateHere
	//MessageInterface::ShowMessage("VRInterface::ClearSpacePointList()\n");

	mAllSpNameArray.clear();
	mAllSpArray.clear();
	mObjectArray.clear();
	mDrawOrbitArray.clear();
	mDrawObjectArray.clear();
	mScNameArray.clear();
	mCbNameArray.clear();
	mObjectNameArray.clear();
	mCbArray.clear();

	mSpRadii.clear();

	mScXArray.clear();
	mScYArray.clear();
	mScZArray.clear();
	mScVxArray.clear();
	mScVyArray.clear();
	mScVzArray.clear();
	mScQArray.clear();
	mScPrevDataPresent.clear();
	//mScPrevEpoch.clear();
	//mScPrevX.clear();
	//mScPrevY.clear();
	//mScPrevZ.clear();
	//mScPrevVx.clear();
	//mScPrevVy.clear();
	//mScPrevVz.clear();
	//mScPrevQArray.clear();

	mCbXArray.clear();
	mCbYArray.clear();
	mCbZArray.clear();
	mCbVxArray.clear();
	mCbVyArray.clear();
	mCbVzArray.clear();
	mCbQArray.clear();
	mCbPrevDataPresent.clear();
	//mCbPrevEpoch.clear();
	//mCbPrevX.clear();
	//mCbPrevY.clear();
	//mCbPrevZ.clear();
	//mCbPrevVx.clear();
	//mCbPrevVy.clear();
	//mCbPrevVz.clear();
	//mCbPrevQArray.clear();

	mDefaultOrbitColorMap.clear();
	mDefaultTargetColorMap.clear();

	mAllSpCount = 0;
	mCbCount = 0;
	mScCount = 0;
	mObjectCount = 0;
	mDataAbsentWarningCount = 0;

	return true;
}


//------------------------------------------------------------------------------
// bool RemoveSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes spacecraft from the spacecraft list
 *
 * @param <name> spacecraft name to be removed from the list
 *
 * @return true if spacecraft was removed from the list, false otherwise
 *
 */
 //------------------------------------------------------------------------------
bool VRInterface::RemoveSpacePoint(const std::string &name)
{
	//-----------------------------------------------------------------
#ifdef __REMOVE_OBJ_BY_SETTING_FLAG__
//-----------------------------------------------------------------

	for (UnsignedInt i = 0; i < mObjectNameArray.size(); i++)
	{
		if (mObjectNameArray[i] == name)
		{
			mDrawOrbitArray[i] = false;
			// PlotInterface::SetGlDrawOrbitFlag(instanceName, mDrawOrbitArray);
			return true;
		}
	}

	return false;

	//-----------------------------------------------------------------
#else
//-----------------------------------------------------------------

	bool removedFromScArray = false;
	bool removedFromAllSpArray = false;

	//-------------------------------------------------------
	// remove from mScNameArray
	//-------------------------------------------------------


	StringArray::iterator scPos =
		find(mScNameArray.begin(), mScNameArray.end(), name);

	if (scPos != mScNameArray.end())
	{
		MessageInterface::ShowMessage("sc to be erased=%s\n", (*scPos).c_str());

		// erase given spacecraft from the arrays
		mScNameArray.erase(scPos);
		mScXArray.erase(mScXArray.begin());
		mScYArray.erase(mScYArray.begin());
		mScZArray.erase(mScZArray.begin());
		mScVxArray.erase(mScVxArray.begin());
		mScVyArray.erase(mScVyArray.begin());
		mScVzArray.erase(mScVzArray.begin());
		mScPrevDataPresent.erase(mScPrevDataPresent.begin());
		mScPrevEpoch.erase(mScPrevEpoch.begin());
		mScPrevX.erase(mScPrevX.begin());
		mScPrevY.erase(mScPrevY.begin());
		mScPrevZ.erase(mScPrevZ.begin());
		mScPrevVx.erase(mScPrevVx.begin());
		mScPrevVy.erase(mScPrevVy.begin());
		mScPrevVz.erase(mScPrevVz.begin());

		mScCount = mScNameArray.size();



		removedFromScArray = true;
	}


	//-------------------------------------------------------
	// remove from mAllSpNameArray and mObjectNameArray
	//-------------------------------------------------------


	StringArray::iterator spPos =
		find(mAllSpNameArray.begin(), mAllSpNameArray.end(), name);
	StringArray::iterator objPos =
		find(mObjectNameArray.begin(), mObjectNameArray.end(), name);

	if (spPos != mAllSpNameArray.end() && objPos != mObjectNameArray.end())
	{
		mAllSpNameArray.erase(spPos);
		mObjectNameArray.erase(objPos);
		mAllSpCount = mAllSpNameArray.size();
		removedFromAllSpArray = true;



		std::map<std::string, UnsignedInt>::iterator orbColorPos, targColorPos;
		orbColorPos = mDefaultOrbitColorMap.find(name);
		targColorPos = mDefaultTargetColorMap.find(name);

		if (orbColorPos != mDefaultOrbitColorMap.end() &&
			targColorPos != mDefaultTargetColorMap.end())
		{
			// erase given spacecraft name
			mDefaultOrbitColorMap.erase(orbColorPos);
			mDefaultTargetColorMap.erase(targColorPos);
			mAllSpCount = mAllSpNameArray.size();
			removedFromAllSpArray = true;
		}
	}

	//-------------------------------------------------------
	// remove from mObjectArray
	//-------------------------------------------------------


	for (std::vector<SpacePoint*>::iterator objptPos = mObjectArray.begin();
		objptPos != mObjectArray.end(); ++objptPos)
	{
		MessageInterface::ShowMessage
		("mObjectArray=%s\n", (*objptPos)->GetName().c_str());
		if ((*objptPos)->GetName() == name)
		{
			mObjectArray.erase(objptPos);
			break;
		}
	}



	// Set all object array and pointers
	if (removedFromScArray && removedFromAllSpArray)
		PlotInterface::SetGlObject(instanceName, mObjectNameArray, mObjectArray);

	return (removedFromScArray && removedFromAllSpArray);

#endif
}


//------------------------------------------------------------------------------
// Integer FindIndexOfElement(StringArray &labelArray, const std::string &label)
//------------------------------------------------------------------------------
/*
 * Finds the index of the element label from the element label array.
 *
 * Typical element label array contains:
 *    All.epoch, scName.X, scName.Y, scName.Z, scName.Vx, scName.Vy, scName.Vz.
 */
 //------------------------------------------------------------------------------
Integer VRInterface::FindIndexOfElement(StringArray &labelArray,
	const std::string &label)
{
	std::vector<std::string>::iterator pos;
	pos = find(labelArray.begin(), labelArray.end(), label);
	if (pos == labelArray.end())
		return -1;
	else
		return distance(labelArray.begin(), pos);
}


//------------------------------------------------------------------------------
// void BuildDynamicArrays()
//------------------------------------------------------------------------------
void VRInterface::BuildDynamicArrays()
// updateHere
{
	// Build Quaternion arrays
	for (int i = Q1; i < (Q4 + 1); i++) {
		mScQArray.push_back(RealArray());
		mCbQArray.push_back(RealArray());
	}

	// Add spacecraft objects to the list first
	for (int i = 0; i < mAllSpCount; i++)
	{
		if (mAllSpArray[i])
		{
			if (mAllSpArray[i]->IsOfType(Gmat::SPACECRAFT))
			{
				// Add to spacecraft list
				mScNameArray.push_back(mAllSpNameArray[i]);

				mScXArray.push_back(0.0);
				mScYArray.push_back(0.0);
				mScZArray.push_back(0.0);
				mScVxArray.push_back(0.0);
				mScVyArray.push_back(0.0);
				mScVzArray.push_back(0.0);

				mScQArray[Q1].push_back(0.0);

				mScPrevDataPresent.push_back(false);
				//mScPrevEpoch.push_back(0.0);
				//mScPrevX.push_back(0.0);
				//mScPrevY.push_back(0.0);
				//mScPrevZ.push_back(0.0);
				//mScPrevVx.push_back(0.0);
				//mScPrevVy.push_back(0.0);
				//mScPrevVz.push_back(0.0);
				//mScPrevQArray.push_back(0.0);

				// Add to all object list
				mObjectNameArray.push_back(mAllSpNameArray[i]);
				mDrawOrbitArray.push_back(mDrawOrbitMap[mAllSpNameArray[i]]);
				mDrawObjectArray.push_back(mShowObjectMap[mAllSpNameArray[i]]);
				mObjectArray.push_back(mAllSpArray[i]);

				// add radii to array
				mSpRadii.push_back(mScRadiiDefault);
			}
		}
		else
		{
			MessageInterface::ShowMessage
			("The SpacePoint name: %s has NULL pointer.\n It will be removed "
				"from the %s.\n", mAllSpNameArray[i].c_str(), GetTypeName().c_str());
		}
	}

	// Add celestial bodies (and other Obj) to the list

	for (int i = 0; i < mAllSpCount; i++)
	{
		if (mAllSpArray[i])
		{
			if (mAllSpArray[i]->IsOfType(Gmat::CELESTIAL_BODY))
			{
				mCbXArray.push_back(0.0);
				mCbYArray.push_back(0.0);
				mCbZArray.push_back(0.0);
				mCbVxArray.push_back(0.0);
				mCbVyArray.push_back(0.0);
				mCbVzArray.push_back(0.0);
				mCbQArray.push_back(0.0); // remember this should be 2D!
				mCbPrevDataPresent.push_back(false);
				//mCbPrevEpoch.push_back(0.0);
				//mCbPrevX.push_back(0.0);
				//mCbPrevY.push_back(0.0);
				//mCbPrevZ.push_back(0.0);
				//mCbPrevVx.push_back(0.0);
				//mCbPrevVy.push_back(0.0);
				//mCbPrevVz.push_back(0.0);
				//mCbPrevQArray.push_back(0.0);

				// add to name array
				mCbNameArray.push_back(mAllSpNameArray[i]);

				// add to Cb array
				mCbArray.push_back(mAllSpArray[i]);

				// add to all objects list 
				mObjectNameArray.push_back(mAllSpNameArray[i]);
				mDrawOrbitArray.push_back(mDrawOrbitMap[mAllSpNameArray[i]]);
				mDrawObjectArray.push_back(mShowObjectMap[mAllSpNameArray[i]]);
				mObjectArray.push_back(mAllSpArray[i]);

				// add to radii array. From ViewCanvas addOtherObject...
				mSpRadii.push_back(((CelestialBody*)(mAllSpArray[i]))->GetEquatorialRadius());

			}
		}
	}

	mScCount = mScNameArray.size();
	mObjectCount = mObjectNameArray.size();
	mCbCount = mCbNameArray.size();

}


//------------------------------------------------------------------------------
// void ClearDynamicArrays()
//------------------------------------------------------------------------------
void VRInterface::ClearDynamicArrays()
{
	//updateHere

	mObjectNameArray.clear();
	mObjectArray.clear();
	mDrawOrbitArray.clear();
	mDrawObjectArray.clear();
	mScNameArray.clear();
	mCbNameArray.clear();

	mSpRadii.clear();

	mScXArray.clear();
	mScYArray.clear();
	mScZArray.clear();
	mScVxArray.clear();
	mScVyArray.clear();
	mScVzArray.clear();
	mScQArray.clear();
	mScPrevDataPresent.clear();
	//mScPrevEpoch.clear();
	//mScPrevX.clear();
	//mScPrevY.clear();
	//mScPrevZ.clear();
	//mScPrevVx.clear();
	//mScPrevVy.clear();
	//mScPrevVz.clear();
	//mScPrevQArray.clear();

	mCbXArray.clear();
	mCbYArray.clear();
	mCbZArray.clear();
	mCbVxArray.clear();
	mCbVyArray.clear();
	mCbVzArray.clear();
	mCbQArray.clear();
	mCbPrevDataPresent.clear();
	//mCbPrevEpoch.clear();
	//mCbPrevX.clear();
	//mCbPrevY.clear();
	//mCbPrevZ.clear();
	//mCbPrevVx.clear();
	//mCbPrevVy.clear();
	//mCbPrevVz.clear();
	//mCbPrevQArray.clear();
}


//------------------------------------------------------------------------------
// void UpdateObjectList(SpacePoint *sp, bool show = false)
//------------------------------------------------------------------------------
/**
 * Add non-spacecraft object to the list.
 */
 //------------------------------------------------------------------------------
void VRInterface::UpdateObjectList(SpacePoint *sp, bool show)
{
	// Add all spacepoint objects
	std::string name = sp->GetName();
	StringArray::iterator pos =
		find(mObjectNameArray.begin(), mObjectNameArray.end(), name);

	// if name not found, add to arrays
	if (pos == mObjectNameArray.end())
	{

		mObjectNameArray.push_back(name);
		mObjectArray.push_back(sp);
		mDrawOrbitMap[name] = show;
		mShowObjectMap[name] = show;
		mDrawOrbitArray.push_back(show);
		mDrawObjectArray.push_back(show);
		mObjectCount = mObjectNameArray.size();
	}
	else
	{

	}
}


//------------------------------------------------------------------------------
// void WriteCoordinateSystem(CoordinateSystem *cs, const std::string &label = "")
//------------------------------------------------------------------------------
// for debug
void VRInterface::WriteCoordinateSystem(CoordinateSystem *cs, const std::string &label)
{
	if (cs == NULL)
	{
		MessageInterface::ShowMessage("%s CoordinateSystem is NULL\n");
		return;
	}

	std::string originType = "UNKNOWN";
	std::string originName = "UNKNOWN";
	if (cs->GetOrigin())
	{
		originType = (cs->GetOrigin())->GetTypeName();
		originName = (cs->GetOrigin())->GetName();
	}

	MessageInterface::ShowMessage
	("%s = <%p>'%s', isInitialized = %d, origin = <%p><%s>'%s'\n", label.c_str(), cs,
		cs->GetName().c_str(), cs->IsInitialized(), cs->GetOrigin(), originType.c_str(),
		originName.c_str());
}