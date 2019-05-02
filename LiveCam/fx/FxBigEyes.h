#include <fx\FxFaceDistortionAncestor.h>
#include <fx\ImgMesh.h>

class FxBigEyes : public FxCore
{
public:
	FxBigEyes();
	~FxBigEyes();

	void transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model) override;

private:
	void transformMeshOne(cv::Mat frame, const FXModel& fxModel, ImgMesh& imgMesh);
};