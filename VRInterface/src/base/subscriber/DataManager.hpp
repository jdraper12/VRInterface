#define JSON_DLL
//#define	SP	0
//#define	X	1
//#define	Y	2
//#define	Z	3
//#define	VX	4
//#define	VY	5
//#define	VZ	6

#include "VRInterfaceDefs.hpp"

#include <fstream>
#include <iostream>		// for string stream
#include <sstream>		// for string stream
#include <iomanip>		// for setw()
// #include <json/json.h>


class VRInterface_API DataManager // : public VRInterface
{
public:
	// singleton implementation:
	// static DataManager &Instance();

	DataManager();
	virtual ~DataManager();

	DataManager(const DataManager &source);
	DataManager& operator=(const DataManager &rhs);

	void BuildDynamicBuffers(const Integer numSp);
	void ClearDynamicBuffers();

	void AddToBuffer(	//static ensures there is only once instance
		const Real time,
		const Integer scCount, const Integer cbCount,
		const StringArray scNames, const StringArray cbNames,
		const RealArray scPosX, const RealArray scPosY, const RealArray scPosZ,
		const RealArray scVelX, const RealArray scVelY, const RealArray scVelZ,
		const RealArray cbPosX, const RealArray cbPosY, const RealArray cbPosZ,
		const RealArray cbVelX, const RealArray cbVelY, const RealArray cbVelZ,
		bool solving, Integer solverOption, bool updateCanvas,
		bool drawing, const Integer maxData, bool inFunction = false);

	//virtual void AddToBuffer(	//static ensures there is only once instance
	//	const std::string& jsonFileName, const std::string& jsonFullPathFileName,
	//	std::ofstream& jstream,
	//	const Real& time,
	//	const Integer& scCount, const Integer& cbCount,
	//	const StringArray& scNames, const StringArray& cbNames,
	//	const RealArray& spRadii,
	//	const RealArray& scPosX, const RealArray& scPosY, const RealArray& scPosZ,
	//	const RealArray& scVelX, const RealArray& scVelY, const RealArray& scVelZ,
	//	const RealArray& cbPosX, const RealArray& cbPosY, const RealArray& cbPosZ,
	//	const RealArray& cbVelX, const RealArray& cbVelY, const RealArray& cbVelZ,
	//	const ColorMap orbitColourMap,
	//	bool solving, Integer solverOption, bool updateCanvas,
	//	bool drawing, bool inFunction = false);

	void WriteToJson(
		const std::string& jsonFileName, const std::string& jsonFullPathFileName,
		std::ofstream& jstream,
		const Integer& scCount, const Integer& cbCount,
		const StringArray scNames, const StringArray cbNames,
		const RealArray spRadii,
		const ColorMap orbitColourMap, const Integer& maxData);

protected:

	// static bool maxDataExceeded;
	static bool areBuffersCleared;

	static RealArray2D storedSpPosX; // [numSp][RealArray maxData]
	static RealArray2D storedSpPosY; // [numSp][RealArray maxData]
	static RealArray2D storedSpPosZ; // [numSp][RealArray maxData]
	static RealArray2D storedSpVelX; // [numSp][RealArray maxData]
	static RealArray2D storedSpVelY; // [numSp][RealArray maxData]
	static RealArray2D storedSpVelZ; // [numSp][RealArray maxData]

	// Quat arrays here

	static RealArray storedTime; // [maxData]

};

// implementations for methods to prevent unresolved externals 