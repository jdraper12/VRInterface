#include "DataManager.hpp"
#include "MessageInterface.hpp"

// #define DEBUG_ADDTOBUFFER


// static data
// bool DataManager::maxDataExceeded = false;

RealArray2D DataManager::storedSpPosX;
RealArray2D DataManager::storedSpPosY;
RealArray2D DataManager::storedSpPosZ;
RealArray2D DataManager::storedSpVelX;
RealArray2D DataManager::storedSpVelY;
RealArray2D DataManager::storedSpVelZ;

RealArray DataManager::storedTime;

bool DataManager::areBuffersCleared = false;


//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
DataManager::DataManager() {
	// nothing to do here
}

//------------------------------------------------------------
// Destructor
//------------------------------------------------------------
DataManager::~DataManager() {
	// clear up, nothing atm
}

//------------------------------------------------------------
// Copy constructor
//------------------------------------------------------------
DataManager::DataManager(const DataManager &dm) {
	storedSpPosX = dm.storedSpPosX;
	storedSpPosY = dm.storedSpPosY;
	storedSpPosZ = dm.storedSpPosZ;
	storedSpVelX = dm.storedSpVelX;
	storedSpVelY = dm.storedSpVelY;
	storedSpVelZ = dm.storedSpVelZ;
	storedTime = dm.storedTime;
}

//------------------------------------------------------------
// Assignment Operator
//------------------------------------------------------------
DataManager& DataManager::operator=(const DataManager& dm) {
	storedSpPosX = dm.storedSpPosX;
	storedSpPosY = dm.storedSpPosY;
	storedSpPosZ = dm.storedSpPosZ;
	storedSpVelX = dm.storedSpVelX;
	storedSpVelY = dm.storedSpVelY;
	storedSpVelZ = dm.storedSpVelZ;
	storedTime = dm.storedTime;
	return *this;
}

//------------------------------------------------------------
// Initialise dynamic buffer arrays 
//------------------------------------------------------------
/*
* @maxData -- sets upper size of arrays
* @numSp -- number of Objects
*/
void DataManager::BuildDynamicBuffers(const Integer numSp) {
	// RealArray numSpCol (numSp);
	for (int i = 0; i < (numSp); i++) {
		storedSpPosX.push_back(RealArray());
		storedSpPosY.push_back(RealArray());
		storedSpPosZ.push_back(RealArray());
		storedSpVelX.push_back(RealArray());
		storedSpVelY.push_back(RealArray());
		storedSpVelZ.push_back(RealArray());
	}
	
	//storedSpPosX.push_back(numSpCol);
	//storedSpPosY.push_back(numSpCol);
	//storedSpPosZ.push_back(numSpCol);
	//storedSpVelX.push_back(numSpCol);
	//storedSpVelY.push_back(numSpCol);
	//storedSpVelZ.push_back(numSpCol);

	//storedSpPosX.resize(numSp);
	//storedSpPosY.resize(numSp);
	//storedSpPosZ.resize(numSp);
	//storedSpVelX.resize(numSp);
	//storedSpVelY.resize(numSp);
	//storedSpVelZ.resize(numSp);
}

//------------------------------------------------------------
// Clear buffers
//------------------------------------------------------------
/*
* Deallocation of memory
*/
void DataManager::ClearDynamicBuffers() {
	storedSpPosX.clear();
	storedSpPosY.clear();
	storedSpPosZ.clear();
	storedSpVelX.clear();
	storedSpVelY.clear();
	storedSpVelZ.clear();
	storedTime.clear();

	areBuffersCleared = true;
}

//------------------------------------------------------------
// buffer orbit data
//------------------------------------------------------------
/*
* Called instead of UpdateGlPlot. Miror logic flow in here
* Consider renaming and implementing in other .cpp file

*/
void DataManager::AddToBuffer(
	const Real time,
	const Integer scCount, const Integer cbCount,
	const StringArray scNames, const StringArray cbNames,
	const RealArray scPosX, const RealArray scPosY, const RealArray scPosZ,
	const RealArray scVelX, const RealArray scVelY, const RealArray scVelZ,
	const RealArray cbPosX, const RealArray cbPosY, const RealArray cbPosZ,
	const RealArray cbVelX, const RealArray cbVelY, const RealArray cbVelZ,
	bool solving, Integer solverOption, bool updateCanvas,
	bool drawing, const Integer maxData, bool inFunction)
{
	areBuffersCleared = false;

	#ifdef DEBUG_ADDTOBUFFER
	MessageInterface::ShowMessage
	("===========================================================================\n");
	MessageInterface::ShowMessage
		("DataManager::AddToBuffer(): time=%0.11f, scCount=%d, cbCount=%d",time,scCount,cbCount);
	MessageInterface::ShowMessage
	("\nscNames:	|posX:		|posY:		|posZ:		|velX:		|velY:		|velZ:	");
	
	for (int i = 0; i<scCount; i++) {
		MessageInterface::ShowMessage
		("\n%s	%0.2f		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f",
		scNames[i].c_str(),scPosX[i],scPosY[i],scPosZ[i],scVelX[i],scVelY[i],scVelZ[i]);
	}
	MessageInterface::ShowMessage
	("\ncbNames:	|posX:		|posY:		|posZ:		|velX:		|velY:		|velZ:	");

	for (int i = 0; i < cbCount; i++) {
		MessageInterface::ShowMessage
		("\n%s		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f",
			cbNames[i].c_str(), cbPosX[i], cbPosY[i], cbPosZ[i], cbVelX[i], cbVelY[i], cbVelZ[i]);
	}
	MessageInterface::ShowMessage(
		"\ntargetColourMap, solving=%d, solverOption=%d,\n",
		solving, solverOption,
		"\nupdateCanvas=%d, drawing=%d, inFunction=%d\n", updateCanvas, drawing, inFunction);
	#endif

	// push back buffers with current sc state
	for (int i = 0; i < scCount; i++) {
		storedSpPosX[i].push_back(scPosX[i]);
		storedSpPosY[i].push_back(scPosY[i]);
		storedSpPosZ[i].push_back(scPosZ[i]);
		storedSpVelX[i].push_back(scVelX[i]);
		storedSpVelY[i].push_back(scVelY[i]);
		storedSpVelZ[i].push_back(scVelZ[i]);
		// quats here
	}
		
	// push back buffers with current cb state
	// maintaining indexing by starting from last sc
	for (int i = 0; i < cbCount; i++) {
		storedSpPosX[scCount + i].push_back(cbPosX[i]);
		storedSpPosY[scCount + i].push_back(cbPosY[i]);
		storedSpPosZ[scCount + i].push_back(cbPosZ[i]);
		storedSpVelX[scCount + i].push_back(cbVelX[i]);
		storedSpVelY[scCount + i].push_back(cbVelY[i]);
		storedSpVelZ[scCount + i].push_back(cbVelZ[i]);
		// quats here
	}

	storedTime.push_back(time);
}

//------------------------------------------------------------
// Write buffers to json file
//------------------------------------------------------------
/*
* @maxData passed onto here for final data control
* Writes buffers from mission to json file
* Triggers at end of mission run
* TODO:
	- implememt check where size of arrays does not exceed maxData
	- if yes, then 'thin' out the arrays
	- can also deallocate space if there are stationary space points
*/
void DataManager::WriteToJson(
	const std::string& jsonFileName, const std::string& jsonFullPathFileName,
	std::ofstream& jstream, 
	const Integer& scCount, const Integer& cbCount,
	const StringArray scNames, const StringArray cbNames,
	const RealArray spRadii,
	const ColorMap orbitColourMap, const Integer& maxData) {

	if (areBuffersCleared == false) {	
	// prevent out-of-bounds exception due to Distribute being called twice
	// at EndOfRun
		bool maxDataExceeded = false;

		// max data check
		// consider making more robust 
		if (storedSpPosX[0].size() >= 100) {
			maxDataExceeded = true;
			// print to message window
			// HandeStationary();
		}
		if (storedSpPosX[0].size() >= maxData) {
			maxDataExceeded = true;
			// print to message window
			// ThinOut();
		}
		else {
			maxDataExceeded = false;
		}

	// initialise JSON file

	// open file and clear contents
	// OR flag used as out overridden by trunc
		if (!jstream.is_open()) {
			jstream.open(jsonFullPathFileName, std::ofstream::out | std::ofstream::trunc);
		}
		
		// Close the stream 
		if (jstream.is_open()) {
			jstream.close();
		}

		// open file stream in append mode
		// CAREFUL -- does IF condition need to be used again?
		// at this point, json should constructed and cleared

		if (!jstream.is_open()) {
			// open in append mode
			jstream.open(jsonFullPathFileName, std::ofstream::app);
		}

		std::ostringstream jsonBuilder;
		jsonBuilder << "{";
		jsonBuilder << "\t" << "\"info\": {\n";
		jsonBuilder << "\t\t" << "\"coordinates\": \"cartesian\",\n";
		jsonBuilder << "\t\t" << "\"units\": \"km\"\n";
		jsonBuilder << "\t" << "},\n";

		jsonBuilder << "\t" << "\"orbits\": [\n";
		for (int i = 0; i < scCount; i++) {	// spacecraft done first

			jsonBuilder << "\t\t" << "{\n";
			jsonBuilder << "\t\t\t" << "\"name\": \"" << scNames[i] << "\",\n";
			// for now, just draw all objects as line,display
			// and default colour schemes
			jsonBuilder << "\t\t\t" << "\"display\": \"" << "line,point" << "\",\n";
			jsonBuilder << "\t\t\t" << "\"radius\": " << spRadii[i] << ",\n";
			jsonBuilder << "\t\t\t" << "\"color\": \"" << "" << "\",\n";

			jsonBuilder << "\t\t\t" << "\"eph\": [\n";
			for (int j = 0; j < storedSpPosX[0].size(); j++) {
				jsonBuilder << "\t\t\t\t" << "[" << std::setprecision(10)
					<< std::setw(14) << storedSpPosX[i][j] << ","
					<< std::setw(14) << storedSpPosY[i][j] << ","
					<< std::setw(14) << storedSpPosZ[i][j] << ","
					<< std::setw(14) << storedSpVelX[i][j] << ","
					<< std::setw(14) << storedSpVelY[i][j] << ","
					<< std::setw(14) << storedSpVelZ[i][j] << "],\n";
				// quats here
			}

			jsonBuilder << "\t\t\t" << "],\n";

			jsonBuilder << "\t\t\t" << "\"time\": [";
			for (int k = 0; k < storedTime.size(); k++) {
				jsonBuilder << std::setprecision(10) << storedTime[k] << ",";
			}
			jsonBuilder << "]\n";
			jsonBuilder << "\t\t" << "},\n";

		}
		for (int i = 0; i < cbCount; i++) {	// celestial bodies done second

			jsonBuilder << "\t\t" << "{\n";
			jsonBuilder << "\t\t\t" << "\"name\": \"" << cbNames[i] << "\",\n";
			// for now, just draw all objects as line,display
			// and default colour schemes
			jsonBuilder << "\t\t\t" << "\"display\": \"" << "line,point" << "\",\n";
			jsonBuilder << "\t\t\t" << "\"radius\": " << spRadii[i + scCount] << ",\n";
			jsonBuilder << "\t\t\t" << "\"color\": \"" << "" << "\",\n";

			jsonBuilder << "\t\t\t" << "\"eph\": [\n";
			for (int j = 0; j < storedSpPosX[0].size(); j++) {
				jsonBuilder << "\t\t\t\t" << "[" << std::setprecision(10)
					<< std::setw(12) << storedSpPosX[i + scCount][j] << ","
					<< std::setw(12) << storedSpPosY[i + scCount][j] << ","
					<< std::setw(12) << storedSpPosZ[i + scCount][j] << ","
					<< std::setw(12) << storedSpVelX[i + scCount][j] << ","
					<< std::setw(12) << storedSpVelY[i + scCount][j] << ","
					<< std::setw(12) << storedSpVelZ[i + scCount][j] << "],\n";
				// quats here
			}

			jsonBuilder << "\t\t\t" << "],\n";

			jsonBuilder << "\t\t\t" << "\"time\": [";
			for (int k = 0; k < storedTime.size(); k++) {
				jsonBuilder << std::setprecision(10) << storedTime[k] << ",";
			}
			jsonBuilder << "]\n";
			jsonBuilder << "\t\t" << "},\n";

		}

		jsonBuilder << "\t" << "]\n";
		jsonBuilder << "}";
		// jsonBuilder << std::ends;


		jstream << jsonBuilder.str();
		// << std::endl;	// added in .str()

		// Close the stream 
		if (jstream.is_open()) {
			jstream.close();
		}
	}


	ClearDynamicBuffers();
}