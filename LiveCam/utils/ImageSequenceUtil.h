#ifndef IMAGE_SEQ_OBJECT
#define IMAGE_SEQ_OBJECT

#include <fstream>

class ImageSequenceUtil
{
public:
	ImageSequenceUtil();
	~ImageSequenceUtil();
	
	static bool load(std::string folderpath, int amountToLoad, std::vector<std::array<cv::Mat, 2>>& sequence){
		int i = 0;
		sequence.clear();
		for (i; i < amountToLoad; i++)
		{
			std::string idString = std::to_string(i);
			std::string ext = ".png";
			cv::Mat temp = cv::imread(folderpath + idString + ext, -1);
			cv::Mat targetBGR(temp.rows, temp.cols, CV_8UC3);       // create BGR mat
			cv::Mat targetAlpha(temp.rows, temp.cols, CV_8UC1);     // create alpha mat
			cv::Mat out[] = { targetBGR, targetAlpha };                   // create array of matrices
			int from_to[] = { 0, 0, 1, 1, 2, 2, 3, 3 };                 // create array of index pairs 
			cv::mixChannels(&temp, 1, out, 2, from_to, 4);


			//sequence[i][0] = targetBGR;
			//sequence[i][1] = targetAlpha;
			
			sequence.push_back({ targetBGR , targetAlpha });
		}
		return (i > 0);
	}

	static bool load2(std::string folderpath, int amountToLoad, cv::Mat sequence[], std::string ext){
		int i = 0;
		for (i; i < amountToLoad; i++)
		{
			std::string idString = std::to_string(i);
			cv::Mat temp = cv::imread(folderpath + idString + ext, 1);
			sequence[i] = temp;
		}
		return (i > 0);
	}

	static void loadImagesToOpenGLTextures(std::string folderpath, int imagesCount, std::vector<GLuint> &textureIDs)
	{
		for (int i = 0; i < imagesCount; ++i)
		{
			std::string path = folderpath + std::to_string(i) + ".png";

			GLuint texID = 0;

			auto image = cv::imread(path, cv::IMREAD_UNCHANGED);

			if (!image.empty())
			{
				glGenTextures(1, &texID);
				glBindTexture(GL_TEXTURE_2D, texID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				ModelUtil::addGlTexture(image);
			}

			textureIDs.push_back(texID);
		}
	}
};

#endif 
