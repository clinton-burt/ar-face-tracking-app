#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader\tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "utils\stb_image.h"

std::string ModelLoader::GetBaseDir(const std::string &filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}

bool ModelLoader::FileExists(const std::string &abs_filename) {
	bool ret;
	FILE *fp = fopen(abs_filename.c_str(), "rb");
	if (fp) {
		ret = true;
		fclose(fp);
	}
	else {
		ret = false;
	}

	return ret;
}

Vector4f ModelLoader::CalculateQuat(Vector3f vecFrom, Vector3f vecTo)
{
	Vector4f result;
	Vector3f crossProductV = vecFrom.cross(vecTo);

	//crossProductV.normalize();
	float len = crossProductV.norm();
	if (len < 0.05)
	{
		len = 0.05;
	}
	crossProductV /= len;

	float dotProductF = vecFrom.dot(vecTo);

	float lenFrom = vecFrom.norm();
	if (lenFrom < 0.05)
	{
		lenFrom = 0.05;
	}

	float lenTo = vecTo.norm();
	if (lenTo < 0.05)
	{
		lenTo = 0.05;
	}

	float alpha = acos(dotProductF / (lenTo * lenFrom));

	float sinHalfAlpha = sin(alpha / 2);
	float cosHalfAlpha = cos(alpha / 2);

	result << crossProductV * sinHalfAlpha, cosHalfAlpha;

	return result;
}

Vector3f ModelLoader::RotateByQuat(Vector3f vec, Vector4f quat)
{
	Vector3f u;
	u[0] = quat[0];
	u[1] = quat[1];
	u[2] = quat[2];

	// Extract the scalar part of the quaternion
	float s = quat[3];

	Vector3f result;

	float dotuv = u.dot(vec);

	float dotuu = u.dot(u);

	Vector3f crossProductuv = u.cross(vec);

	// Do the math

	result = 2 * dotuv * u + (s*s - dotuu) * vec + 2 * s * crossProductuv;

	return result;
}

bool ModelLoader::LoadTexture(const std::string& texture_filename, GLuint& id)
{
	if (!FileExists(texture_filename)) {
		std::cerr << "Unable to load texture: " << texture_filename << std::endl;
		return false;
	}
	int w, h;
	int comp;
	unsigned char* image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
	if (!image) {
		std::cerr << "Unable to load texture: " << texture_filename << std::endl;
		return false;
	}
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (comp == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	else if (comp == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	return true;

}

void ModelLoader::loadObjectsNames(std::vector<std::string> &objFiles, std::vector<std::string>& objectsNames)
{
	for(auto &file : objFiles)
	{

		ifstream ifs(file);

		if (!ifs.good())
		{
			return;
		}

		string name;
		name.reserve(32);

		string line;
		line.reserve(255);

		while (ifs.good())
		{
			getline(ifs, line);

			switch (line[0])
			{
			case 'g':
			case 'o':
			{
				if (line[1] == ' ')
				{
					auto offset = line.find_first_of(' ', 2);
					std::string name = line.substr(2, offset - 2);
					if (name != "default")
					{
						objectsNames.push_back(name);
					}
				}
				break;
			}
			}
		}

		ifs.close();

	}
}

void ModelLoader::loadMTLnames(std::vector<std::string> &objFiles, std::vector<DrawObjectsModel>& drawObjects)
{
	for (auto &file : objFiles)
	{

		ifstream ifs(file);

		if (!ifs.good())
		{
			return;
		}

		string name;
		name.reserve(32);

		string line;
		line.reserve(255);

		string lastMeshName;

		while (ifs.good())
		{
			getline(ifs, line);

			switch (line[0])
			{
				case 'g':
				case 'o':
				{
					if (line[1] == ' ')
					{
						auto offset = line.find_first_of(' ', 2);
						lastMeshName = line.substr(2, offset - 2);
					}
					break;
				}

				case 'u':
				{
					if (line.substr(0, 7) == "usemtl " && !lastMeshName.empty())
					{
						auto iter = std::find_if(drawObjects.begin(), drawObjects.end(), [lastMeshName](DrawObjectsModel& E) -> bool
						{
							return E.name == lastMeshName;
						});

						if (iter != drawObjects.end())
						{
							iter->material = line.substr(7);
						}
					}
					break;
				}
			}
		}

		ifs.close();

	}
}

void ModelLoader::LoadObj(std::vector<std::string> &objFiles, std::vector<DrawObjectsModel>& drawObjects,
	std::array<float, 3>& bmin, std::array<float, 3>& bmax, float extraScale, Vector3f extraShift, Matrix3f extraRotateMatrix)
{
	bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
	bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

	for (auto &file : objFiles)
	{

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;

		std::string base_dir = GetBaseDir(file);
		// this should work both windows and linux/unix
		base_dir += "/";


		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.data(), base_dir.c_str());
		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		if (!ret) {
			std::cerr << "Failed to load " << file << std::endl;
			continue;
		}

		//printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
		//printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
		//printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
		//printf("# of shapes    = %d\n", (int)shapes.size());

		for (size_t s = 0; s < shapes.size(); s++)
		{
			DrawObjectsModel o;

			o.name = shapes[s].name;

			float objectZmin = std::numeric_limits<float>::max();

			std::vector<Vector3f> vb;
			std::vector<Vector3f> cb;
			std::vector<Vector3f> nb;
			std::vector<Vector2f> tb;

			std::vector<Vector3f> tangentb;
			std::vector<Vector3f> bitangentb;

			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
				tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				float tc[3][2];
				if (attrib.texcoords.size() > 0) {
					assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);
					int index1 = 2 * idx0.texcoord_index;
					int index2 = 2 * idx1.texcoord_index;
					int index3 = 2 * idx2.texcoord_index;
					tc[0][0] = attrib.texcoords[index1];
					tc[0][1] = 1.0f - attrib.texcoords[index1 + 1];
					tc[1][0] = attrib.texcoords[index2];
					tc[1][1] = 1.0f - attrib.texcoords[index2 + 1];
					tc[2][0] = attrib.texcoords[index3];
					tc[2][1] = 1.0f - attrib.texcoords[index3 + 1];

				}
				else {
					tc[0][0] = 0.0f;
					tc[0][1] = 0.0f;
					tc[1][0] = 0.0f;
					tc[1][1] = 0.0f;
					tc[2][0] = 0.0f;
					tc[2][1] = 0.0f;
				}

				Vector3f v[3];
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);

				Eigen::Vector3f A = extraRotateMatrix * Eigen::Vector3f(
					attrib.vertices[3 * f0 + 0],
					attrib.vertices[3 * f0 + 1],
					attrib.vertices[3 * f0 + 2]) * extraScale + extraShift;

				Eigen::Vector3f B = extraRotateMatrix * Eigen::Vector3f(
					attrib.vertices[3 * f1 + 0],
					attrib.vertices[3 * f1 + 1],
					attrib.vertices[3 * f1 + 2]) * extraScale + extraShift;

				Eigen::Vector3f C = extraRotateMatrix * Eigen::Vector3f(
					attrib.vertices[3 * f2 + 0],
					attrib.vertices[3 * f2 + 1],
					attrib.vertices[3 * f2 + 2]) * extraScale + extraShift;

				for (int k = 0; k < 3; k++)
				{
					v[0][k] = A[k];
					v[1][k] = B[k];
					v[2][k] = C[k];

					bmin[k] = std::min(v[0][k], bmin[k]);
					bmin[k] = std::min(v[1][k], bmin[k]);
					bmin[k] = std::min(v[2][k], bmin[k]);
					bmax[k] = std::max(v[0][k], bmax[k]);
					bmax[k] = std::max(v[1][k], bmax[k]);
					bmax[k] = std::max(v[2][k], bmax[k]);
				}

				objectZmin = std::min(objectZmin, v[0][2]);
				objectZmin = std::min(objectZmin, v[1][2]);
				objectZmin = std::min(objectZmin, v[2][2]);

				Vector3f n[3];
				if (attrib.normals.size() > 0) {
					int f0 = idx0.normal_index;
					int f1 = idx1.normal_index;
					int f2 = idx2.normal_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);
					for (int k = 0; k < 3; k++) {
						n[0][k] = attrib.normals[3 * f0 + k];
						n[1][k] = attrib.normals[3 * f1 + k];
						n[2][k] = attrib.normals[3 * f2 + k];
					}
				}
				else {
					n[0] = v[0].normalized();
					n[1] = v[1].normalized();
					n[2] = v[2].normalized();
				}



				//This should be the right code to calculate tangent and binormal, but for Baseball Hat, this code produce wrong result -- Vladislav Khorev vladislav.khorev@fishrungames.com

				//Calculate tangent and bitangent vectors begin
				/*
				//float flatNormal[3];
				std::array<float, 3> flatNormal;

				CalcNormal(&flatNormal[0], v[0], v[1], v[2]);

				//Code from here: http://www.opengl-tutorial.org/ru/intermediate-tutorials/tutorial-13-normal-mapping/

				// Edges of the triangle : postion delta
				std::array<float, 3> deltaPos1 = { v[1][0] - v[0][0], v[1][1] - v[0][1], v[1][2] - v[0][2] }; //v1-v0;
				std::array<float, 3> deltaPos2 = { v[2][0] - v[0][0], v[2][1] - v[0][1], v[2][2] - v[0][2] };//v2 - v0;

				// UV delta
				std::array<float, 2> deltaUV1 = { tc[1][0] - tc[0][0], tc[1][1] - tc[0][1] };//uv1 - uv0;
				std::array<float, 2> deltaUV2 = { tc[2][0] - tc[0][0], tc[2][1] - tc[0][1] };//uv2 - uv0;

				float r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);
				std::array<float, 3> tangent;
				tangent[0] = (deltaPos1[0] * deltaUV2[1] - deltaPos2[0] * deltaUV1[1])*r;
				tangent[1] = (deltaPos1[1] * deltaUV2[1] - deltaPos2[1] * deltaUV1[1])*r;
				tangent[2] = (deltaPos1[2] * deltaUV2[1] - deltaPos2[2] * deltaUV1[1])*r;
				std::array<float, 3> bitangent;
				bitangent[0] = (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0])*r;
				bitangent[1] = (deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0])*r;
				bitangent[2] = (deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0])*r;
				*/



				//Calculate tangent and bitangent vectors end



				for (int k = 0; k < 3; k++) {
					vb.push_back({ v[k][0], v[k][1], v[k][2] });

					nb.push_back({ n[k][0], n[k][1], n[k][2] });

					tb.push_back({ tc[k][0], tc[k][1] });

					Vector3f realNormal = n[k];


					//This is kind of hacky way to calculate tangent vector, but it works fine for Baseball Hat -- Vladislav Khorev vladislav.khorev@fishrungames.com
					Vector3f rotatedTangent = Vector3f(0.1f, 1.f, 0.2f).cross(realNormal);

					float len = rotatedTangent.norm();
					if (len < 0.05)
					{
						//In case if our normal vector is very near to { 0.1f, 1.f, 0.2f }, we choose another vector
						rotatedTangent = Vector3f(1.f, 0.2f, 0.1f).cross(realNormal);
					}

					Vector3f rotatedBitangent = realNormal.cross(rotatedTangent);


					tangentb.push_back(rotatedTangent);

					bitangentb.push_back(rotatedBitangent);

				}

			}


			if (vb.size() > 0) {
				glGenBuffers(1, &o.vb);
				glBindBuffer(GL_ARRAY_BUFFER, o.vb);
				glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(Vector3f), vb.data(),
					GL_STATIC_DRAW);
				o.numTriangles = vb.size() / 3;
				printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
					o.numTriangles);
			}

			if (cb.size() > 0) {
				glGenBuffers(1, &o.cb);
				glBindBuffer(GL_ARRAY_BUFFER, o.cb);
				glBufferData(GL_ARRAY_BUFFER, cb.size() * sizeof(Vector3f), cb.data(), GL_STATIC_DRAW);

			}

			if (nb.size() > 0) {
				glGenBuffers(1, &o.nb);
				glBindBuffer(GL_ARRAY_BUFFER, o.nb);
				glBufferData(GL_ARRAY_BUFFER, nb.size() * sizeof(Vector3f), nb.data(), GL_STATIC_DRAW);

			}

			if (tangentb.size() > 0) {
				glGenBuffers(1, &o.tangentb);
				glBindBuffer(GL_ARRAY_BUFFER, o.tangentb);
				glBufferData(GL_ARRAY_BUFFER, tangentb.size() * sizeof(Vector3f), tangentb.data(), GL_STATIC_DRAW);

			}

			if (bitangentb.size() > 0) {
				glGenBuffers(1, &o.bitangentb);
				glBindBuffer(GL_ARRAY_BUFFER, o.bitangentb);
				glBufferData(GL_ARRAY_BUFFER, bitangentb.size() * sizeof(Vector3f), bitangentb.data(), GL_STATIC_DRAW);

			}


			if (tb.size() > 0) {
				glGenBuffers(1, &o.tb);
				glBindBuffer(GL_ARRAY_BUFFER, o.tb);
				glBufferData(GL_ARRAY_BUFFER, tb.size() * sizeof(Vector2f), tb.data(), GL_STATIC_DRAW);

			}

			drawObjects.push_back(o);
			o.minimumZ = objectZmin;
		}

		/*
		bmin[0] = bmin[0] * extraScale;
		bmin[1] = bmin[1] * extraScale;
		bmin[2] = bmin[2] * extraScale;

		bmax[0] = bmax[0] * extraScale;
		bmax[1] = bmax[1] * extraScale;
		bmax[2] = bmax[2] * extraScale;*/

		printf("bmin = %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
		printf("bmax = %f, %f, %f\n", bmax[0], bmax[1], bmax[2]);

	}
}


bool ModelLoader::LoadObjAndConvert(std::string &filename, std::vector<DrawObjectsModel>& drawObjects,
	std::vector<tinyobj::material_t>& materials, std::map<std::string, GLuint>& textures,
		std::array<float, 3>& bmin, std::array<float, 3>& bmax, float extraScale, Vector3f extraShift)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;

	std::string base_dir = GetBaseDir(filename);
#ifdef _WIN32
	base_dir += "\\";
#else
	base_dir += "/";
#endif

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.data(), base_dir.c_str());
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		std::cerr << "Failed to load " << filename << std::endl;
		return false;
	}

	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	// Load diffuse textures
	
		for (size_t m = 0; m < materials.size(); m++) {
			tinyobj::material_t* mp = &materials[m];

			if (mp->diffuse_texname.length() > 0) {
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end()) {
					GLuint texture_id;
					int w, h;
					int comp;

					std::string texture_filename = mp->diffuse_texname;
					if (!FileExists(texture_filename)) {
						// Append base dir.
						texture_filename = base_dir + mp->diffuse_texname;
						if (!FileExists(texture_filename)) {
							std::cerr << "Unable to find file: " << mp->diffuse_texname << std::endl;
							exit(1);
						}
					}

					unsigned char* image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
					if (!image) {
						std::cerr << "Unable to load texture: " << texture_filename << std::endl;
						exit(1);
					}
					glGenTextures(1, &texture_id);
					glBindTexture(GL_TEXTURE_2D, texture_id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					if (comp == 3) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
					}
					else if (comp == 4) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
					}
					glBindTexture(GL_TEXTURE_2D, 0);
					stbi_image_free(image);
					textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
				}
			}
		}

	bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
	bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

	
	for (size_t s = 0; s < shapes.size(); s++)
	{
		DrawObjectsModel o;
		std::vector<Vector3f> vb;
		std::vector<Vector3f> cb;
		std::vector<Vector3f> nb;
		std::vector<Vector2f> tb;

		for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {

			int current_material_id = shapes[s].mesh.material_ids[f];

			if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
				// Invaid material ID. Use default material.
				// Default material is added to the last item in `materials`.
				current_material_id = materials.size() - 1;
			}

			Vector3f diffuse;
			for (size_t i = 0; i < 3; i++) {
				diffuse[i] = materials[current_material_id].diffuse[i];
			}

			tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
			tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
			tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

			float tc[3][2];
			if (attrib.texcoords.size() > 0) {
				assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
				assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
				assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);
				int index1 = 2 * idx0.texcoord_index;
				int index2 = 2 * idx1.texcoord_index;
				int index3 = 2 * idx2.texcoord_index;
				tc[0][0] = attrib.texcoords[index1];
				tc[0][1] = 1.0f - attrib.texcoords[index1 + 1];
				tc[1][0] = attrib.texcoords[index2];
				tc[1][1] = 1.0f - attrib.texcoords[index2 + 1];
				tc[2][0] = attrib.texcoords[index3];
				tc[2][1] = 1.0f - attrib.texcoords[index3 + 1];
			}
			else {
				tc[0][0] = 0.0f;
				tc[0][1] = 0.0f;
				tc[1][0] = 0.0f;
				tc[1][1] = 0.0f;
				tc[2][0] = 0.0f;
				tc[2][1] = 0.0f;
			}

			Vector3f v[3];
			for (int k = 0; k < 3; k++) {
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);

				v[0][k] = attrib.vertices[3 * f0 + k] * extraScale + extraShift[k];
				v[1][k] = attrib.vertices[3 * f1 + k] * extraScale + extraShift[k];
				v[2][k] = attrib.vertices[3 * f2 + k] * extraScale + extraShift[k];
				bmin[k] = std::min(v[0][k], bmin[k]);
				bmin[k] = std::min(v[1][k], bmin[k]);
				bmin[k] = std::min(v[2][k], bmin[k]);
				bmax[k] = std::max(v[0][k], bmax[k]);
				bmax[k] = std::max(v[1][k], bmax[k]);
				bmax[k] = std::max(v[2][k], bmax[k]);
			}

			Vector3f n[3];
			if (attrib.normals.size() > 0) {
				int f0 = idx0.normal_index;
				int f1 = idx1.normal_index;
				int f2 = idx2.normal_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);
				for (int k = 0; k < 3; k++) {
					n[0][k] = attrib.normals[3 * f0 + k];
					n[1][k] = attrib.normals[3 * f1 + k];
					n[2][k] = attrib.normals[3 * f2 + k];
				}
			}
			else {
				n[0] = v[0].normalized();
				n[1] = v[1].normalized();
				n[2] = v[2].normalized();
			}

			for (int k = 0; k < 3; k++) {

				vb.push_back(v[k]);

				nb.push_back(n[k]);

				// Combine normal and diffuse to get color.
				float normal_factor = 0.2;
				float diffuse_factor = 1 - normal_factor;

				Vector3f c = n[k] * normal_factor + diffuse * diffuse_factor;
				c.normalize();

				cb.push_back(c * 0.5 + Vector3f(0.5, 0.5, 0.5));

				tb.push_back( { tc[k][0], tc[k][1] } );
			}
		}

		o.vb = 0;
		o.numTriangles = 0;

		// OpenGL viewer does not support texturing with per-face material.
		if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s) {
			// Base case
			o.material_id = shapes[s].mesh.material_ids[s];
		}
		else {
			o.material_id = materials.size() - 1; // = ID for default material.
		}

		if (vb.size() > 0) {
			glGenBuffers(1, &o.vb);
			glBindBuffer(GL_ARRAY_BUFFER, o.vb);
			glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(Vector3f), vb.data(), GL_STATIC_DRAW);
			o.numTriangles = vb.size() / 3;
			printf("shape[%d] # of triangles = %d\n", static_cast<int>(s), o.numTriangles);
		}

		if (cb.size() > 0) {
			glGenBuffers(1, &o.cb);
			glBindBuffer(GL_ARRAY_BUFFER, o.cb);
			glBufferData(GL_ARRAY_BUFFER, cb.size() * sizeof(Vector3f), cb.data(), GL_STATIC_DRAW);

		}

		if (nb.size() > 0) {
			glGenBuffers(1, &o.nb);
			glBindBuffer(GL_ARRAY_BUFFER, o.nb);
			glBufferData(GL_ARRAY_BUFFER, nb.size() * sizeof(Vector3f), nb.data(), GL_STATIC_DRAW);

		}


		if (tb.size() > 0) {
			glGenBuffers(1, &o.tb);
			glBindBuffer(GL_ARRAY_BUFFER, o.tb);
			glBufferData(GL_ARRAY_BUFFER, tb.size() * sizeof(Vector2f), tb.data(), GL_STATIC_DRAW);

		}

		drawObjects.push_back(o);
	}

	printf("bmin = %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
	printf("bmax = %f, %f, %f\n", bmax[0], bmax[1], bmax[2]);

	return true;
}

DrawObjectsModel ModelLoader::CreateQuadModel()
{
	DrawObjectsModel obj;

	std::array<Vector3f, 6> vb =
	{
		Vector3f(-1, -1, 0), Vector3f(-1, 1, 0), Vector3f(1, 1, 0),
		Vector3f(1, 1, 0), Vector3f(1, -1, 0), Vector3f(-1, -1, 0)
	};
	std::array<Vector2f, 6> tb =
	{
		Vector2f(0, 0), Vector2f(0, 1), Vector2f(1, 1),
		Vector2f(1, 1), Vector2f(1, 0), Vector2f(0, 0)
	};

	glGenBuffers(1, &obj.vb);
	glBindBuffer(GL_ARRAY_BUFFER, obj.vb);
	glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(Vector3f), vb.data(), GL_STATIC_DRAW);
	obj.numTriangles = 2;

	glGenBuffers(1, &obj.tb);
	glBindBuffer(GL_ARRAY_BUFFER, obj.tb);
	glBufferData(GL_ARRAY_BUFFER, tb.size() * sizeof(Vector2f), tb.data(), GL_STATIC_DRAW);

	return obj;
}

bool ModelLoader::LoadCubemapTexture(const std::array<std::string, 6>& texture_filename, GLuint& id)
{
	std::array<unsigned char*, 6> imageData;

	int width = 0;
	int height = 0;

	for (size_t i = 0; i < 6; i++)
	{

		int w, h;
		int comp;
		imageData[i] = stbi_load(texture_filename[i].c_str(), &w, &h, &comp, STBI_default);

		if (i == 0)
		{
			width = w;
			height = h;
		}
		else
		{
			if (w != width || h != height)
			{
				std::cerr << "Texture in cubemap have different size: " << texture_filename[i] << std::endl;
				return false;
			}
		}

		if (!imageData[i]) {
			std::cerr << "Unable to load texture: " << texture_filename[i] << std::endl;
			return false;
		}

		if (comp != 3)
		{
			std::cerr << "Texture should not contain alpha channel: " << texture_filename[i] << std::endl;
			return false;
		}
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

#ifdef _WIN32

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

#endif

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[5]);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	for (size_t i = 0; i < 6; i++)
	{
		stbi_image_free(imageData[i]);
	}

	return true;
}

Vector3f loadVector3f(char *dataBegin)
{
	Vector3f result;
	char *begin = dataBegin;
	char *end = dataBegin;

	for (int i = 0; i < 3; ++i)
	{
		while (*end != ' ' && *end != 0) ++end;
		try
		{
			result[i] = stof(string(begin, end));
		}
		catch (const invalid_argument &e)
		{
			throw std::exception (("Invalid OBJ file line: " + string(dataBegin)).c_str());
		}

		begin = ++end; // skip only one whitespace
	}
	return result;
}

boost::property_tree::ptree ModelLoader::convertMTLtoPTree(const std::string &path, const std::string &resourcesRoot)
{
	boost::property_tree::ptree MTLs;

	ifstream ifs(path);

	if (!ifs.good())
	{
		return MTLs;
	}

	fs::path folder = resourcesRoot;

	boost::property_tree::ptree newMTL;

	string name;
	name.reserve(32);

	string line;
	line.reserve(255);

	bool isEmptyMTL = true;

	while (ifs.good())
	{
		getline(ifs, line);

		switch (line[0])
		{
		case 'n':
		{
			if (line.substr(0, 6) == "newmtl")
			{
				if (!isEmptyMTL)
				{
					MTLs.put_child(name, newMTL);
				}
				name = line.substr(7);
				newMTL.clear();
				isEmptyMTL = true;
			}
			break;
		}

		case 'K':
		{
			boost::property_tree::ptree vector3fTree;

			Vector3f vector3f;
			
			try
			{
				vector3f = loadVector3f(line._Myptr() + 3);
			}
			catch (std::exception &e)
			{
				break;
			}

			for (int i = 0; i < 3; ++i)
			{
				boost::property_tree::ptree vector3fTreeChild;
				vector3fTreeChild.put("", vector3f[i]);
				vector3fTree.push_back(make_pair("", vector3fTreeChild));
			}

			switch (line[1])
			{
			case 'a':
			{
				newMTL.put_child("ambientLight", vector3fTree);
				isEmptyMTL = false;
				break;
			}
			case 'd':
			{
				newMTL.put_child("diffuseLight", vector3fTree);
				isEmptyMTL = false;
				break;
			}
			case 's':
			{
				newMTL.put_child("specularLight", vector3fTree);
				isEmptyMTL = false;
				break;
			}
			}
			break;
		}

		case 'N':
		{
			if (line[1] == 's')
			{
				newMTL.put("specularPower", line.substr(3));
				isEmptyMTL = false;
			}
			break;
		}

		case 'm':
		{
			if (line.substr(0, 5) == "map_K")
			{
				switch (line[5])
				{
				case 'a':
				case 'd':
				{
					newMTL.put("Texture", (folder / line.substr(line[7] == '/' || line[7] == '\\' ? 8 : 7)).string());
					isEmptyMTL = false;
					break;
				}
				/*case 's':
				{
					newMTL.put("normalMap", (folder / line.substr(line[7] == '/' || line[7] == '\\' ? 8 : 7)).string());
					isEmptyMTL = false;
					break;
				}*/
				}
			}
			break;
		}

		case 'b':
		{
			if (line.substr(0, 4) == "bump")
			{
				newMTL.put("normalMap", (folder / line.substr(line[5] == '/' || line[5] == '\\' ? 6 : 5)).string());
				isEmptyMTL = false;
			}
			break;
		}

// 		case 'r':
// 		{
// 			if (line.substr(0, 9) == "reflRatio")
// 			{
// 				newMTL.put("reflectionRatio", line.substr(10));
// 				isEmptyMTL = false;
// 			}
// 			break;
// 		}
// 
// 		case 'l':
// 		{
// 			if (line.substr(0, 8) == "lightPos")
// 			{
// 				boost::property_tree::ptree vector3fTree;
// 
// 				Vector3f vector3f;
// 
// 				try
// 				{
// 					vector3f = loadVector3f(line._Myptr() + 9);
// 				}
// 				catch (std::exception &e)
// 				{
// 					break;
// 				}
// 
// 				for (int i = 0; i < 3; ++i)
// 				{
// 					boost::property_tree::ptree vector3fTreeChild;
// 					vector3fTreeChild.put("", vector3f[i]);
// 					vector3fTree.push_back(make_pair("", vector3fTreeChild));
// 				}
// 
// 				newMTL.put_child("lightPos", vector3fTree);
// 				isEmptyMTL = false;
// 			}
// 			break;
// 		}

		}
	}

	ifs.close();

	if (!isEmptyMTL)
	{
		MTLs.put_child(name, newMTL);
	}

	//boost::property_tree::json_parser::write_json("MTLtree.json", MTLs);

	return MTLs;
}