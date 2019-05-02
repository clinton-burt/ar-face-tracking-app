#include "Uniforms.h"

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform1i i) const
{
	usedShader->setUniform1i(const_cast<char*>(uniformName._Myptr()), i);
}

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform1f f) const
{
	usedShader->setUniform1f(const_cast<char*>(uniformName._Myptr()), f);
}

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform2f f2) const
{
	usedShader->setUniform2fv(const_cast<char*>(uniformName._Myptr()), 1, f2.data());
}

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform3f f3) const
{
	usedShader->setUniform3fv(const_cast<char*>(uniformName._Myptr()), 1, f3.data());
}

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform4f f4) const
{
	usedShader->setUniform4fv(const_cast<char*>(uniformName._Myptr()), 1, f4.data());
}

void ObjectRenderParamVisitor_GLSetter::operator()(TUniform16 M16) const
{
	usedShader->setUniformMatrix4fv(const_cast<char*>(uniformName._Myptr()), 1, false, M16.data());
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform1i i) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;
	
	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", i);
	result.push_back(make_pair("", resultChild));

	return result;
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform1f f) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;

	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f);
	result.push_back(make_pair("", resultChild));

	return result;
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform2f f2) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;

	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f2[0]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f2[1]);
	result.push_back(make_pair("", resultChild));

	return result;
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform3f f3) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;

	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f3[0]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f3[1]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f3[2]);
	result.push_back(make_pair("", resultChild));

	return result;
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform4f f4) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;

	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f4[0]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f4[1]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f4[2]);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	resultChild.put("", f4[3]);
	result.push_back(make_pair("", resultChild));

	return result;
}

boost::property_tree::ptree ObjectRenderParamVisitor_toPTree::operator()(TUniform16 M16) const
{
	boost::property_tree::ptree result;
	boost::property_tree::ptree resultChild;

	resultChild.put("", uniformName);
	result.push_back(make_pair("", resultChild));
	resultChild.clear();

	for (int i = 0; i < 16; ++i)
	{
		resultChild.put("", M16(i));
		result.push_back(make_pair("", resultChild));
		resultChild.clear();
	}

	return result;
}