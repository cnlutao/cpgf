// Auto generated file, don't modify.

#include "Box2D/Box2D.h"
#include "Box2D/Dynamics/Joints/b2FrictionJoint.h"

#include "cpgf/metadata/box2d/meta_b2FrictionJoint.h"

#include "cpgf/gmetapolicy.h"
#include "cpgf/goutmain.h"

extern const char * _mEta_nS_box2d;

using namespace cpgf;

namespace {

G_AUTO_RUN_BEFORE_MAIN()
{
    {
    GDefineMetaNamespace _ns = GDefineMetaNamespace::define(_mEta_nS_box2d);
    GDefineMetaClass<b2FrictionJoint, b2Joint> _d = GDefineMetaClass<b2FrictionJoint, b2Joint>::Policy<MakePolicy<GMetaRuleDefaultConstructorAbsent> >::declare("b2FrictionJoint");
    _ns._class(_d);
    buildMetaClass_B2FrictionJoint(0, _d, NULL, GMetaPolicyCopyAllConstReference());

    }
    

    {
    GDefineMetaNamespace _ns = GDefineMetaNamespace::define(_mEta_nS_box2d);
    GDefineMetaClass<b2FrictionJointDef, b2JointDef> _d = GDefineMetaClass<b2FrictionJointDef, b2JointDef>::declare("b2FrictionJointDef");
    _ns._class(_d);
    buildMetaClass_B2FrictionJointDef(0, _d, NULL, GMetaPolicyCopyAllConstReference());

    }
    

}

} // unnamed namespace

