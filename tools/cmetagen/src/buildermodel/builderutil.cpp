#include "builderutil.h"
#include "buildercontext.h"
#include "buildersection.h"
#include "model/cppitem.h"
#include "model/cppcontainer.h"
#include "model/cppclass.h"
#include "model/cppinvokable.h"
#include "model/cppenum.h"
#include "codewriter/codeblock.h"
#include "util.h"
#include "project.h"

#include "cpgf/gassert.h"

#include "Poco/Format.h"

using namespace std;


namespace metagen {

string getTextOfVisibility(ItemVisibility visibility)
{
	switch(visibility) {
		case ivPrivate:
			return "private";

		case ivProtected:
			return "protected";

		default:
			return "public";
	}
}

std::string getReflectionClassName(const Project * project)
{
	return project->getMetaDefineParamName() + "::ClassType";
}

std::string getReflectionScope(const CppItem * item)
{
	if(item->isGlobal()) {
		return "";
	}
	else {
		return getReflectionClassName(item->getProject()) + "::";
	}
}

bool isVisibilityAllowed(ItemVisibility visibility, const Project * project)
{
	if((visibility == ivPublic) != project->doesAllowPublic()
		|| (visibility == ivProtected) != project->doesAllowProtected()
		|| (visibility == ivPrivate) != project->doesAllowPrivate()
		) {
		return false;
	}

	return true;
}

size_t getCppItemPayload(const CppItem * item)
{
	if(item->isFile() || item->isNamespace()) {
		return 0;
	}
	int payload = 1;
	if(item->isInvokable()) {
		const CppInvokable * invokable = static_cast<const CppInvokable *>(item);
		for(size_t i = 0; i < invokable->getArity(); ++i) {
			if(invokable->paramHasDefaultValue(i)) {
				++payload;
			}
		}
	}
	else if(item->isEnum()) {
		payload += static_cast<const CppEnum *>(item)->getValueList()->size();
	}
	return payload;
}

string getContainerQualifiedName(const BuilderContext * builderContext, const CppContainer * cppContainer)
{
	string result;

	if(cppContainer->isClass() || cppContainer->isNamespace()) {
		result = normalizeSymbolName(cppContainer->getQualifiedName());
	}
	else {
		result = builderContext->getSourceBaseFileName() + "_Global";
	}

	return result;
}

std::string getClassWrapperClassQualifiedName(const BuilderContext * builderContext, const CppContainer * cppContainer)
{
	return cppContainer->getQualifiedName() + builderContext->getProject()->getClassWrapperPostfix();
}

std::string getClassWrapperClassName(const BuilderContext * builderContext, const CppContainer * cppContainer)
{
	return cppContainer->getName() + builderContext->getProject()->getClassWrapperPostfix();
}

std::string getContainerOrClassWrapperClassName(CodeNameType nameType, const BuilderContext * builderContext, const CppContainer * cppContainer)
{
	switch(nameType) {
		case cntNormal:
			return cppContainer->getName();

		case cntClassWrapper:
			return getClassWrapperClassName(builderContext, cppContainer);
	}
	return "";
}

string getContainerOrClassWrapperQualifiedName(const BuilderContext * builderContext, BuilderSection * section)
{
	GASSERT(section->getCppItem()->isContainer());

	const CppContainer * cppContainer = static_cast<const CppContainer *>(section->getCppItem());

	switch(getNameTypeFromBuilderSection(section)) {
		case cntNormal:
			return getContainerQualifiedName(builderContext, cppContainer);

		case cntClassWrapper:
			return getClassWrapperClassQualifiedName(builderContext, cppContainer);
	}
	return "";
}

string getSectionIndexName(int sectionIndex)
{
	string result;

	if(sectionIndex > 0) {
		Poco::format(result, "_%d", sectionIndex);
	}

	return result;
}

std::string getPartialCreationFunctionName(const BuilderContext * builderContext, BuilderSection * section)
{
	GASSERT(section->getCppItem()->isContainer());

	int sectionIndex = section->getIndex();

	return normalizeSymbolName(Poco::format("partial_%s_%s%s",
		builderContext->getProject()->getCreationFunctionPrefix(),
		getContainerOrClassWrapperQualifiedName(builderContext, section),
		getSectionIndexName(sectionIndex)
		)
	);
}

std::string getPartialCreationFunctionPrototype(const BuilderContext * builderContext, BuilderSection * section)
{
	string creationName = getPartialCreationFunctionName(builderContext, section);
	return Poco::format("void %s(cpgf::GDefineMetaInfo metaInfo)", creationName);
}

std::string getCreationFunctionName(const BuilderContext * builderContext, BuilderSection * section)
{
	return normalizeSymbolName(builderContext->getProject()->getCreationFunctionPrefix()
		+ "_" + getContainerOrClassWrapperQualifiedName(builderContext, section));
}

std::string getCreationFunctionPrototype(const BuilderContext * builderContext, BuilderSection * section)
{
	string creationName = getCreationFunctionName(builderContext, section);
	return Poco::format("cpgf::GDefineMetaInfo %s()", creationName);
}

std::string getReflectionFunctionName(const BuilderContext * builderContext, BuilderSection * section)
{
	GASSERT(section->getCppItem()->isContainer());

	int sectionIndex = section->getIndex();

	return normalizeSymbolName(Poco::format("%s_%s%s",
		builderContext->getProject()->getReflectionFunctionPrefix(),
		getContainerOrClassWrapperQualifiedName(builderContext, section),
		getSectionIndexName(sectionIndex)
		)
	);
}

CodeNameType getNameTypeFromBuilderSection(BuilderSection * section)
{
	if(section->isClassWrapper()) {
		return cntClassWrapper;
	}
	else {
		return cntNormal;
	}
}

void initializeReflectionFunctionOutline(const BuilderContext * builderContext, BuilderSection * section)
{
	GASSERT(section->getCppItem()->isContainer());

	const CppContainer * cppContainer = static_cast<const CppContainer *>(section->getCppItem());
	CodeBlock * codeBlock = section->getCodeBlock();
	string functionName = getReflectionFunctionName(builderContext, section);

	const std::string & D = builderContext->getProject()->getMetaDefineParamName();
	CodeBlock * headerBlock = codeBlock->getNamedBlock(CodeBlockName_FunctionHeader);
	const CppClass * cppClass = cppContainer->isClass() ? static_cast<const CppClass *>(cppContainer) : NULL;

	string s = "template <typename " + D;
	if(cppClass != NULL && cppClass->isChainedTemplate()) {
		s.append(", ");
		s.append(cppClass->getTextOfChainedTemplateParamList(itoWithArgType | itoWithArgName | itoWithDefaultValue));
	}
	s.append(" >");
	headerBlock->appendLine(s);

	s = Poco::format("void %s(%s & _d)", functionName, D);
	headerBlock->appendLine(s);
	
	CodeBlock * bodyBlock = codeBlock->getNamedBlock(CodeBlockName_FunctionBody);
	bodyBlock->appendLine("using namespace cpgf;");
	bodyBlock->appendBlankLine();
	
	// force the block order for each kind of items
	for(ItemCategory ic = icFirst; ic < icCount; ic = ItemCategory(int(ic) + 1)) {
		bodyBlock->getNamedBlock(ItemNames[ic]);
	}
}

void initializePartialCreationFunction(const BuilderContext * builderContext, BuilderSection * section,
		BuilderTemplateInstantiation * templateInstantiation)
{
	GASSERT(section->getCppItem()->isContainer());

	const CppContainer * cppContainer = static_cast<const CppContainer *>(section->getCppItem());
	CodeBlock * codeBlock = section->getCodeBlock();

	const CppClass * cppClass = NULL;
	if(cppContainer->isClass()) {
		cppClass = static_cast<const CppClass *>(cppContainer);
	}

	string prototype = getPartialCreationFunctionPrototype(builderContext, section);

	string s;

	if(cppClass != NULL && cppClass->isTemplate()) {
		if(templateInstantiation == NULL) {
			return;
		}

		s = Poco::format("template <%s >", cppClass->getTextOfChainedTemplateParamList(itoWithArgType | itoWithArgName));
		codeBlock->appendLine(s);
	}
	codeBlock->appendLine(prototype);

	CodeBlock * bodyBlock = codeBlock->appendBlock(cbsBracketAndIndent);
	string metaType;
	if(cppClass != NULL) {
		string className = getContainerOrClassWrapperQualifiedName(builderContext, section);
		if(cppClass->isTemplate()) {
			metaType = Poco::format("cpgf::GDefineMetaClass<%s<%s > >", className,
				cppClass->getTextOfChainedTemplateParamList(itoWithArgName));
		}
		else {
			metaType = Poco::format("cpgf::GDefineMetaClass<%s >", className);
		}
	}
	else {
		metaType = "GDefineMetaGlobal";
	}
	s = Poco::format("%s meta = %s::fromMetaClass(metaInfo.getMetaClass());", metaType, metaType);
	bodyBlock->appendLine(s);
	s = Poco::format("%s(meta);",
		getReflectionFunctionName(builderContext, section));
	bodyBlock->appendLine(s);
}

void initializeClassWrapperOutline(const BuilderContext * builderContext, BuilderSection * section)
{
	GASSERT(section->getCppItem()->isClass());

	const CppContainer * cppContainer = static_cast<const CppContainer *>(section->getCppItem());
	CodeBlock * codeBlock = section->getCodeBlock();

	const CppClass * cppClass = static_cast<const CppClass *>(cppContainer);
	string s;
	
	if(cppClass->isTemplate()) {
		s = Poco::format("template <%s >", cppClass->getTextOfChainedTemplateParamList(itoWithArgType | itoWithArgName));
		codeBlock->appendLine(s);
	}

	s = Poco::format("class %s : public %s, public cpgf::GScriptWrapper",
		getClassWrapperClassName(builderContext, cppClass),
		cppClass->getQualifiedName()
	);
	codeBlock->appendLine(s);

	CodeBlock * bodyBlock = codeBlock->getNamedBlock(CodeBlockName_ClassBody, cbsBracket);
	bodyBlock->appendLine("private:");
	CodeBlock * superBlock = bodyBlock->appendBlock(cbsIndent | cbsTailEmptyLine);
	s = Poco::format("typedef %s super;", cppClass->getQualifiedName());
	superBlock->appendLine(s);
	for(int i = ivFirst; i < ivCount; ++i) {
		s = getTextOfVisibility(ItemVisibility(i));
		CodeBlock * block = bodyBlock->getNamedBlock(s, cbsTailEmptyLine);
		block->appendLine(s + ":");
		block->getNamedBlock(CodeBlockName_Customize, cbsIndent | cbsTailEmptyLine);
	}
}



} // namespace metagen

