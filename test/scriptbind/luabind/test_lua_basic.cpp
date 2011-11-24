#include "test_lua_common.h"


namespace {


GTEST(MethodIndirectInvoke)
{
	GScopedPointer<TestLuaContext> context(createLuaContext());

	QDO(a = TestObject())
	QDO(a.value = 1)
	QASSERT(a.value == 1)

	QDO(b = TestObject())
	QDO(b.value = 8)
	QASSERT(b.value == 8)

	QDO(f = a.add)
	QASSERT(f(2) == 3)

	QDO(f = b.add)
	QASSERT(f(2) == 10)
}


GTEST(OverloadMethodIndirectInvoke)
{
	GScopedPointer<TestLuaContext> context(createLuaContext());

	QDO(a = TestObject())
	QDO(a.value = 1)
	QASSERT(a.value == 1)

	QDO(b = TestObject())
	QDO(b.value = 8)
	QASSERT(b.value == 8)

	QDO(f = a.methodOverload)
	QASSERT(f(2, 3) == 2 * 3)
	QASSERT(f(TestObject(2), 3) == 2 + 3)
	QASSERT(f(3, TestObject(2)) == 2 + 3 + 1)
	QASSERT(f("ab", 3) == 2 + 3)
	QASSERT(f(3, "ab") == 2 + 3 + 1)

	QDO(g = b.methodOverload)
	QDO(f = g)
	QASSERT(f(5, 3) == 5 * 3)
	QASSERT(f(TestObject(5), 3) == 5 + 3)
	QASSERT(f(3, TestObject(2)) == 2 + 3 + 1)
	QASSERT(f("ab", 3) == 2 + 3)
	QASSERT(f(3, "ab") == 2 + 3 + 1)
}




}