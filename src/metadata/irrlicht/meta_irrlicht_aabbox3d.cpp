// Auto generated file, don't modify.

#include "irrlicht.h"
#include "aabbox3d.h"

#include "cpgf/metadata/irrlicht/meta_irrlicht_aabbox3d.h"

using namespace cpgf;

namespace meta_irrlicht { 


GDefineMetaInfo createMetaClass_Global_aabbox3d()
{
    GDefineMetaGlobalDangle _d = GDefineMetaGlobalDangle::dangle();
    buildMetaClass_Global_aabbox3d(0, _d);
    return _d.getMetaInfo();
}


GDefineMetaInfo createMetaClass_Aabbox3d()
{
    GDefineMetaGlobalDangle _d = GDefineMetaGlobalDangle::dangle();
    {
        GDefineMetaClass<aabbox3d<f32> > _nd = GDefineMetaClass<aabbox3d<f32> >::declare("aabbox3d_f32");
        buildMetaClass_Aabbox3d<GDefineMetaClass<aabbox3d<f32> >, f32 >(0, _nd);
        _d._class(_nd);
    }
    return _d.getMetaInfo();
}


} // namespace meta_irrlicht

