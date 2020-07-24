//$Id$
//------------------------------------------------------------------------------
//                                  VRInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Declares VRInterface Class

#include "Subscriber.hpp"
#include "VRInterfaceDefs.hpp"

class VRInterface_API VRInterface : public Subscriber
{
public:
	// typical declarations 

	VRInterface(const std::string &type, const std::string &name);
	VRInterface(const VRInterface &op);
	VRInterface& operator=(const VRInterface&);
	virtual ~VRInterface();





	// methods inherited from GmatBase

	virtual bool Validate();
	virtual bool Initialize();

	virtual GmatBase* Clone() const;
	virtual void		Copy(const GmatBase* orig);


	// methods inherited from Subscriber
	virtual bool Activate(bool state = true);

	virtual bool Distribute(Integer len);
	virtual bool Distribute(const Real * dat, Integer len);
	
	// An emumeration of parameters for OpenFramesInterface, starting at the end
	// of the @ref Subscriber::SubscriberParamCount enumeration.

protected: //internal variables

	// copied from OrbitPlot.hpp

	CoordinateSystem *mViewCoordSystem;

	std::string mOldName;
	std::string mViewCoordSysName;

	// object names and arrays
	std::vector<SpacePoint*> mObjectArray;	// list of SP and their options
	std::vector<SpacePoint*> mAllSpArray;
	BooleanArray mDrawOrbitArray;
	BooleanArray mDrawObjectArray;

	StringArray mScNameArray;
	StringArray mObjectNameArray;
	StringArray mAllSpNameArray;
	StringArray mAllRefObjectNames;

	Integer mAllSpCount;
	Integer mScCount;
	Integer mObjectCount;
	Integer mNonStdBodyCount;

	// for data control
	Integer mDataCollectFrequency;
	Integer mUpdatePlotFrequency;
	Integer mNumPointsToRedraw;
	Integer mNumData;
	Integer mNumCollected;
	Integer mDataAbsentWarningCount;
	Integer mMaxData;

	// arrays for holding distributed data
	RealArray mScXArray;
	RealArray mScYArray;
	RealArray mScZArray;
	RealArray mScVxArray;
	RealArray mScVyArray;
	RealArray mScVzArray;

	// arrays for holding previous distributed data
	BooleanArray mScPrevDataPresent;
	RealArray mScPrevEpoch;
	RealArray mScPrevX;
	RealArray mScPrevY;
	RealArray mScPrevZ;
	RealArray mScPrevVx;
	RealArray mScPrevVy;
	RealArray mScPrevVz;

	// arrays for holding solver current data
	std::vector<StringArray> mCurrScArray;
	std::vector<Real> mCurrEpochArray;
	std::vector<RealArray> mCurrXArray;
	std::vector<RealArray> mCurrYArray;
	std::vector<RealArray> mCurrZArray;
	std::vector<RealArray> mCurrVxArray;
	std::vector<RealArray> mCurrVyArray;
	std::vector<RealArray> mCurrVzArray;

	// maps for object and color
	std::map<std::string, UnsignedInt> mDefaultOrbitColorMap;
	std::map<std::string, UnsignedInt> mDefaultTargetColorMap;
	std::map<std::string, UnsignedInt> mCurrentOrbitColorMap;
	std::map<std::string, UnsignedInt> mCurrentTargetColorMap;

	// maps for object, flag, and epoch
	std::map<std::string, bool> mDrawOrbitMap;
	std::map<std::string, bool> mShowObjectMap;
	std::map<std::string, Real> mScInitialEpochMap;




	enum //template for now. Reflect changes across other 2 enums
	{
		ADD = SubscriberParamCount,    ///< Add spacepoint(s) to be plotted
		COORD_SYSTEM,                  ///< Set the plotting coordinate system
		DRAW_OBJECT,                   ///< Enable drawing a spacepoint's object
		DRAW_LABEL,                    ///< Enable drawing name label for a spacepoint
		DRAW_VELOCITY,                 ///< Enable drawing a spacecraft's velocity
		DRAW_LINE_COLOUR,              ///< Sets the line colour of a spacecraft's trajectory
												 //	change from ORBIT_COLOR
		DRAW_TRAJECTORY,					 ///< Enable drawing of a spacecraft's trajectory 
												 // 	may be redundant due to above
		SC_RADII,							 ///< Sets the radii of a spacecraft if spherical representation used
		MODEL_FILE,							 ///< Sets the model file override for OpenFrames
		DATA_COLLECT_FREQUENCY,			 
		UPDATE_PLOT_FREQUENCY,
		NUM_POINTS_TO_REDRAW,
		SHOW_PLOT,                     ///< Active or deactive plotting
		MAX_DATA,
		TESTERINO,							 //tester
		VRInterfaceParamCount,			 ///< Count of the parameters for this class
	};

	//private: was here in OFInterface implementation

	// these dictate the sizes of the index arrays to access enumerated
	// parameters
	static const Gmat::ParameterType
		PARAMETER_TYPE[VRInterfaceParamCount - SubscriberParamCount];
	static const std::string
		PARAMETER_TEXT[VRInterfaceParamCount - SubscriberParamCount];

};

/** to do:

TakeAction





*/