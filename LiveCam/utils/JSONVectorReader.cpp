#include "JSONVectorReader.h"

Eigen::Vector2f JSONVectorReader::readVector2f(boost::property_tree::ptree& JSONvector)
{
	auto vector = readVector<float, 2>(JSONvector);
	return{ vector[0], vector[1] };
}

Eigen::Vector3f JSONVectorReader::readVector3f(boost::property_tree::ptree& JSONvector)
{
	auto vector = readVector<float, 3>(JSONvector);
	return{ vector[0], vector[1], vector[2] };
}

Eigen::Vector4f JSONVectorReader::readVector4f(boost::property_tree::ptree& JSONvector)
{
	auto vector = readVector<float, 4>(JSONvector);
	return{ vector[0], vector[1], vector[2], vector[3] };
}

Eigen::Matrix3f JSONVectorReader::readMatrix3f(boost::property_tree::ptree& JSONvector)
{
	auto vector = readVector<float, 9>(JSONvector);
	Eigen::Matrix3f result;
	result <<
		vector[0], vector[3], vector[6],
		vector[1], vector[4], vector[7],
		vector[2], vector[5], vector[8];
	return result;
}

Eigen::Matrix4f JSONVectorReader::readMatrix4f(boost::property_tree::ptree& JSONvector)
{
	auto vector = readVector<float, 16>(JSONvector);
	Eigen::Matrix4f result;
	result <<
		vector[0], vector[4], vector[8], vector[11],
		vector[1], vector[5], vector[9], vector[12],
		vector[2], vector[6], vector[10], vector[13],
		vector[3], vector[7], vector[11], vector[14];
	return result;
}

std::pair<std::string, TUniformVariant> JSONVectorReader::readUniform(boost::property_tree::ptree& JSONvector)
{
	std::vector<std::string> input;
	for (auto& element : JSONvector)
	{
		input.push_back(element.second.get_value<std::string>());
	}

	std::string name = input.front();

	if (input.size() == 1 && input[1].find('.') != -1)
	{
		return { name, TUniform1i(std::stoi(input[1])) };
	}
	else
	{
		std::vector<float> output;
		for (auto &element = input.begin() + 1; element != input.end(); ++element)
		{
			output.push_back(std::stof(*element));
		}
		switch (output.size())
		{
		case 1: return { name, TUniform1f(output[0]) };
		case 2: return { name, TUniform2f(output[0], output[1]) };
		case 3: return { name, TUniform3f(output[0], output[1], output[2]) };
		case 4: return{ name, TUniform4f(output[0], output[1], output[2], output[3]) };
		case 16: 
			Eigen::Matrix4f result;
			result << output[0], output[1], output[2], output[3],
					  output[4], output[5], output[6], output[7],
					  output[8], output[9], output[10], output[11],
					  output[12], output[13], output[14], output[15];
			return { name, TUniform16(result) };
		}
	}
}