// Auto generated file, don't modify.

#ifndef __META_IRRLICHT_DRIVERCHOICE_H
#define __META_IRRLICHT_DRIVERCHOICE_H


#include "gmetaobjectlifemanager_irrlicht_ireferencecounted.h"
#include "cpgf/gmetadefine.h"
#include "cpgf/metadata/gmetadataconfig.h"
#include "cpgf/metadata/private/gmetadata_header.h"
#include "cpgf/gmetapolicy.h"


using namespace irr;


namespace meta_irrlicht { 


template <typename D>
void buildMetaClass_Global_driverchoice(const cpgf::GMetaDataConfigFlags & config, D _d)
{
    (void)config; (void)_d; (void)_d;
    using namespace cpgf;
    
    _d.CPGF_MD_TEMPLATE _method("driverChoiceConsole", (irr::video::E_DRIVER_TYPE (*) (bool))&driverChoiceConsole)
        ._default(copyVariantFromCopyable(true))
    ;
}


} // namespace meta_irrlicht




#include "cpgf/metadata/private/gmetadata_footer.h"


#endif
