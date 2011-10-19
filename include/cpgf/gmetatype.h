#ifndef __GMETATYPE_H
#define __GMETATYPE_H

#include "cpgf/gvariant.h"
#include "cpgf/gstdint.h"
#include "cpgf/gassert.h"


#include <typeinfo>

#include <string.h>


namespace cpgf {


#pragma pack(push, 1)
#pragma pack(1)
struct GMetaTypeData
{
	const char * baseName;
	uint32_t flags;
	GVarTypeData typeData;
};
#pragma pack(pop)


class GTypeInfo
{
public:
	GTypeInfo() : typeInfo(NULL) {
	}

	explicit GTypeInfo(const std::type_info & typeInfo) : typeInfo(&typeInfo) {
	}

	GTypeInfo(const GTypeInfo & other) : typeInfo(other.typeInfo) {
	}

	GTypeInfo & operator = (const GTypeInfo & other) {
		this->typeInfo = other.typeInfo;

		return *this;
	}

	bool operator == (const GTypeInfo & other) const {
		return *this->typeInfo == *other.typeInfo;
	}

	bool operator != (const GTypeInfo & other) const {
		return *this->typeInfo != *other.typeInfo;
	}

	bool operator < (const GTypeInfo & other) const {
		return !! this->typeInfo->before(*other.typeInfo);
	}

	bool isEmpty() const {
		return this->typeInfo == NULL;
	}

private:
	const std::type_info * typeInfo;
};

template <typename T>
GTypeInfo gTypeId()
{
	return GTypeInfo(typeid(T));
}


namespace meta_internal {

template <typename T>
struct RemoveAllPointers
{
	typedef T Result;
};

template <typename T>
struct RemoveAllPointers <T *>
{
	typedef typename RemoveAllPointers<T>::Result Result;
};

template <typename T>
struct StripBaseType
{
	typedef
		typename RemoveConstVolatile<
			typename RemoveAllPointers<
				typename RemoveReference<
					typename RemoveConstVolatile<T>::Result
				>::Result
			>::Result
		>::Result
	Result;
};

template <typename T>
struct StripNonPointer
{
	typedef
		typename RemoveConstVolatile<
			typename RemoveReference<
				typename RemoveConstVolatile<T>::Result
			>::Result
		>::Result
	Result;
};


const unsigned int mtFlagIsConst = 1 << 0;
const unsigned int mtFlagIsVolatile = 1 << 1;
const unsigned int mtFlagIsConstVolatile = 1 << 2;
const unsigned int mtFlagIsReference = 1 << 3;
const unsigned int mtFlagIsPointer = 1 << 4;
const unsigned int mtFlagIsPointerToConst = 1 << 5;
const unsigned int mtFlagIsPointerToVolatile = 1 << 6;
const unsigned int mtFlagIsPointerToConstVolatile = 1 << 7;
const unsigned int mtFlagIsFunction = 1 << 8;
const unsigned int mtFlagIsConstFunction = 1 << 9;
const unsigned int mtFlagIsVolatileFunction = 1 << 10;
const unsigned int mtFlagIsConstVolatileFunction = 1 << 11;
const unsigned int mtFlagBaseIsClass = 1 << 12;

template <typename T>
struct GMetaTypeDeduce
{
private:
	typedef typename StripBaseType<T>::Result BaseType;
	typedef typename StripNonPointer<T>::Result NoCV;

public:
	enum {
		Flags = 0
			| (IsConst<T>::Result ? mtFlagIsConst : 0)
			| (IsVolatile<T>::Result ? mtFlagIsVolatile : 0)
			| (IsConstVolatile<T>::Result ? mtFlagIsConstVolatile : 0)
			| (IsConst<typename RemovePointer<NoCV>::Result>::Result ? mtFlagIsPointerToConst : 0)
			| (IsVolatile<typename RemovePointer<NoCV>::Result>::Result ? mtFlagIsPointerToVolatile : 0)
			| (IsConstVolatile<typename RemovePointer<NoCV>::Result>::Result ? mtFlagIsPointerToConstVolatile : 0)
			| (IsClass<BaseType>::Result ? mtFlagBaseIsClass : 0)
			| (IsFunction<T>::Result ? mtFlagIsFunction : 0)
			| (GFunctionTraits<T>::IsConst ? mtFlagIsConstFunction : 0)
			| (GFunctionTraits<T>::IsVolatile ? mtFlagIsVolatileFunction : 0)
			| (GFunctionTraits<T>::IsConstVolatile ? mtFlagIsConstVolatileFunction : 0)
			| (IsReference<T>::Result ? mtFlagIsReference : 0)
			| (PointerDimension<NoCV>::Result > 0 ? mtFlagIsPointer : 0)
	};

	static GTypeInfo getBaseType() {
		return gTypeId<BaseType>();
	}
};

template <typename T>
void deduceMetaTypeData(GMetaTypeData * data)
{
	data->baseName = NULL;
	data->flags = GMetaTypeDeduce<T>::Flags;
	deduceVariantType<T>(data->typeData);
}

} // namespace meta_internal


class GMetaType
{
//	GASSERT_STATIC(sizeof(GMetaTypeData) == 8);

public:
	GMetaType() : baseName(NULL), flags(0), baseType() {
		vtInit(typeData);
	}

	explicit GMetaType(const GMetaTypeData & data)
		: baseName(data.baseName), flags(data.flags), typeData(data.typeData), baseType() {
	}

	GMetaType(const GMetaTypeData & data, const GTypeInfo & baseType)
		: baseName(data.baseName), flags(data.flags), typeData(data.typeData), baseType(baseType) {
	}

	GMetaType(const GMetaType & other)
		: baseName(other.baseName), flags(other.flags), typeData(other.typeData), baseType(other.baseType) {
	}

	~GMetaType() {
	}

	GMetaType & operator = (const GMetaType & other) {
		if(this != &other) {
			this->baseName = other.baseName;
			this->flags = other.flags;
			this->typeData = other.typeData;
			this->baseType = other.baseType;
		}

		return *this;
	}

	bool operator == (const GMetaType & other) const;

	bool operator != (const GMetaType & other) const {
		return ! this->operator == (other);
	}

	bool isEmpty() const {
		return vtIsEmpty(vtGetType(this->typeData));
	}

	const char * getBaseName() const {
		return this->baseName;
	}

	const GTypeInfo & getBaseType() const {
		return this->baseType;
	}

	bool baseIsClass() const {
		return this->hasFlag(meta_internal::mtFlagBaseIsClass);
	}
	
	bool isFundamental() const {
		return vtIsFundamental(this->getVariantType());
	}

	bool isFunction() const {
		return this->hasFlag(meta_internal::mtFlagIsFunction);
	}

	bool isConstFunction() const {
		return this->hasFlag(meta_internal::mtFlagIsConstFunction);
	}

	bool isVolatileFunction() const {
		return this->hasFlag(meta_internal::mtFlagIsVolatileFunction);
	}

	bool isConstVolatileFunction() const {
		return this->hasFlag(meta_internal::mtFlagIsConstVolatileFunction);
	}

	bool isConst() const {
		return this->hasFlag(meta_internal::mtFlagIsConst)
			&& !this->isConstVolatile();
	}

	bool isVolatile() const {
		return this->hasFlag(meta_internal::mtFlagIsVolatile)
			&& !this->isConstVolatile();
	}

	bool isConstVolatile() const {
		return this->hasFlag(meta_internal::mtFlagIsConstVolatile);
	}

	bool isPointerToConst() const {
		return this->hasFlag(meta_internal::mtFlagIsPointer | meta_internal::mtFlagIsPointerToConst)
			&& !this->isPointerToConstVolatile();
	}

	bool isPointerToVolatile() const {
		return this->hasFlag(meta_internal::mtFlagIsPointer | meta_internal::mtFlagIsPointerToVolatile)
			&& !this->isPointerToConstVolatile();
	}

	bool isPointerToConstVolatile() const {
		return this->hasFlag(meta_internal::mtFlagIsPointer | meta_internal::mtFlagIsPointerToConstVolatile);
	}

	bool isPointer() const {
		return this->hasFlag(meta_internal::mtFlagIsPointer);
	}

	bool isReference() const {
		return this->hasFlag(meta_internal::mtFlagIsReference);
	}

	unsigned int getPointerDimension() const {
		return vtGetPointers(this->typeData);
	}

	GVariantType getVariantType() const {
		return vtGetType(this->typeData);
	}

	GMetaTypeData getData() const;

	void addPointer();

private:
	bool hasFlag(unsigned int flag) const {
		return (this->flags & flag) == flag;
	}

private:
	const char * baseName;
	int flags;
	GVarTypeData typeData;
	GTypeInfo baseType;

private:
	friend void fixupMetaType(GMetaType * type);
};

template <typename T>
GMetaType createMetaType()
{
	GMetaTypeData data;
	meta_internal::deduceMetaTypeData<T>(&data);
	return GMetaType(data, meta_internal::GMetaTypeDeduce<T>::getBaseType());
}

inline GMetaType createMetaTypeWithName(const GMetaType & type, const char * name)
{
	GMetaTypeData data = type.getData();
	data.baseName = name;
	return GMetaType(data, type.getBaseType());
}

inline void initializeMetaType(GMetaTypeData * data)
{
	vtInit(data->typeData);
	data->flags = 0;
	data->baseName = NULL;
}

void fixupMetaType(GMetaType * type);


} // namespace cpgf



#endif