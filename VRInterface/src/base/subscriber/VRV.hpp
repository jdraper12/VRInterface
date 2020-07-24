//$Id$
//------------------------------------------------------------------------------
//                                  VRV v1
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
/**
 * Class implementation for the VRV subscriber
 */
//------------------------------------------------------------------------------


#ifndef VRV_hpp
#define VRV_hpp

#include "VRInterfaceDefs.hpp"
#include "Subscriber.hpp"
//#include "GmatCommand.hpp"
//#include "CoordinateSystem.hpp"
//#include "SpacePoint.hpp"
//#include <map>
//#include <fstream>

class VRInterface_API VRV : public Subscriber
{
public:
	VRV(const std::string &type, const std::string &name);
	// VRV(const std::string &name) or with 3 arguments if using files. Append as necessary

	VRV(const VRV &ts);
	VRV& operator=(const VRV &ts); // ts omitted in OrbitPlot.hpp??
	virtual ~VRV(void);

	virtual GmatBase*    Clone(void) const;
	// virtual void         Copy(const GmatBase* orig); // if implementing copying? should probably check this

	//virtual GmatBase*    GetRefObject(const UnsignedInt type,
	//	const std::string &name);
	//virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
	//	const std::string &name = "");
};


#endif