#ifndef __TESTSCRIPTBINDMETADATA_H
#define __TESTSCRIPTBINDMETADATA_H


#include "testscriptbind.h"
#include "cpgf/scriptbind/gscriptbind.h"
#include "cpgf/scriptbind/gscriptbindapi.h"


#include <string.h>

#include <string>


namespace testscript {

const int Magic1 = 0x1999;
const int Magic2 = 0xbeef38;
const int Magic3 = 0xf00d;

struct TestData
{
	int x;
	std::string name;
};

class TestObject
{
public:
	TestObject() : value(Magic1) {
	}
	
	TestObject(const TestObject & other) : value(other.value) {
		(void)other;
	}

	TestObject(int value) : value(value) {
	}
	
	TestObject(int a, const std::string & s) : value(a + s.length()) {
	}
	
	TestObject & operator = (const TestObject & other) {
		(void)other;

		this->value = Magic3;

		return *this;
	}
	
	TestObject * self() {
		return this;
	}
	
	const TestObject * selfConst() {
		return this;
	}
	
	volatile TestObject * selfVolatile() {
		return this;
	}
	
	const volatile TestObject * selfConstVolatile() {
		return this;
	}
	
	int methodConst() {
		return 0;
	}
	
	int methodConst() const {
		return 1;
	}
	
	int methodVolatile() {
		return 0;
	}
	
	int methodVolatile() volatile {
		return 1;
	}
	
	int methodConstVolatile() {
		return 0;
	}
	
	int methodConstVolatile() const volatile {
		return 1;
	}
	
	int methodOverload(const TestObject & obj, int n) const {
		return obj.value + n;
	}
	
	int methodOverload(int n, const TestObject & obj) const {
		return obj.value + n + 1;
	}
	
	int methodOverload(int a, int b) const {
		return a * b;
	}
	
	int methodOverload(const char * s, int n) const {
		return strlen(s) + n;
	}
	
	int methodOverload(int n, const char * s) const {
		return strlen(s) + n + 1;
	}
	
	int methodOverload(const std::string & s, int n) const {
		return s.length() * n;
	}
	
	int methodOverload(int n, const std::string & s) const {
		return s.length() * n + 1;
	}
	
public:
	int	value;
};


void bindBasicData(cpgf::GScriptObject * script, cpgf::IMetaService * service);
void bindBasicData(cpgf::IScriptObject * script, cpgf::IMetaService * service);



}


#endif