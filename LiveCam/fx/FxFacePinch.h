#include <fx\FxFaceDistortionAncestor.h>
#include <fx\ImgMesh.h>

class FxFacePinch : public FxCore
{
public:
	FxFacePinch();
	~FxFacePinch();

	void transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model) override;

private:
	void transformMeshOne(cv::Mat frame, const FXModel& fxModel, ImgMesh& imgMesh);
};