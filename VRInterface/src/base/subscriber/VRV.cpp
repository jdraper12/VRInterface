//$Id$
//------------------------------------------------------------------------------
//                                  VRV
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
/**
 * Class implementation for the VRV Subscriber
 */
//------------------------------------------------------------------------------

//#include <sstream>

#include "VRV.hpp"
#include "MessageInterface.hpp"
//#include "FileManager.hpp"      // for GetPathname() -- may end up needing this
//#include "GmatGlobal.hpp"       // for GetDataPrecision()

// DEBUG flags here
#define DEBUG_VRV
//#define VRV_DEBUG_STATE_MACHINE
//#define DEBUG_VRV_INIT
//#define DEBUG_VRV_EXEC
//#define DEBUG_VRV_OUTPUT

//------------------------------------------------------------------------------
// VRV(const std::string name)
//------------------------------------------------------------------------------
/**
 * Default extended constructor
 *
 * This is called by the factory in order to create a new instance of 
 * VRV
 *
 * @param name The name of the new instance
 * @param type Type of new instance
 */
 //------------------------------------------------------------------------------
VRV::VRV(const std::string &type, const std::string &name) //mind the pointers! 
	: Subscriber(type, name)
{
	objectTypes.push_back(GmatType::GetTypeId(type));
	objectTypeNames.push_back(name);

	// trying out the debugging 
	#ifdef DEBUG_VRV
		MessageInterface::ShowMessage("VRV default constructor called");
	#endif
}

//------------------------------------------------------------------------------
// OrbitView& operator=(const OrbitView&)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * Sets variables to match those of the input (e.g. through GUI?)
 * returns reference to this instance
 */
 //------------------------------------------------------------------------------
VRV& VRV::operator=(const VRV& ts)
{
	if (this == &ts) // if class members are already set up correctly???
		return *this;

	VRV::operator=(ts);

	// member = ts.member;
	return *this;
}


//------------------------------------------------------------------------------
// VRV(const VRV &ts)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 */
 //------------------------------------------------------------------------------
VRV::VRV(const VRV &ts)
	: VRV(ts)
{
	// member = ts.member;
}

//------------------------------------------------------------------------------
// ~VRV()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
VRV::~VRV()
{
	//do stuff like clear objects, emergency saves, close panels etc. 
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of VRV.
 *
 * @return clone of VRV.
 */
 //------------------------------------------------------------------------------
GmatBase* VRV::Clone() const
{
	return (new VRV(*this));
}


// VRV implementation here



//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------s
//GmatBase* VRV::GetRefObject(const UnsignedInt type,
//	const std::string &name)
//{
//	return VRV::GetRefObject(type, name);
//}


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
//bool VRV::SetRefObject(GmatBase *obj, const UnsignedInt type,
//	const std::string &name)
//{
//	std::string realName = name;
//	if (name == "")
//		realName = obj->GetName();
//
//	return VRV::SetRefObject(obj, type, realName);
//}

