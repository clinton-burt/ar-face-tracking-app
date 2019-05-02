#include "CommonClasses.h"

//const bool ValueSmoother::SLEEP_ENABLE = true;

double ValueSmoother::snapCurve(double x)
{
	double y = 1.0 / (x + 1.0);
	y = (1.0 - y) * 2.0;
	if (y > 1.0) {
		return 1.0;
	}
	return y;
}

double ValueSmoother::responsiveAnalogReadSimple(double newValue)
{
	if (!inited)
	{
		inited = true;
		smoothValue = newValue;
		return smoothValue;
	}

	auto diff = abs(newValue - smoothValue);

	double snap = snapCurve(diff * SNAP_MULTIPLIER_SIMPLE);

	// calculate the exponential moving average based on the snap
	smoothValue += (newValue - smoothValue) * snap;

	return smoothValue;
}

//double ValueSmoother::responsiveAnalogRead(double newValue)
//{
//	using namespace std::chrono;
//
//	auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
//
//
//	auto diff = abs(newValue - smoothValue);
//
//	// measure the difference between the new value and current value over time
//	// to get a more reasonable indication of how far off the current smooth value is
//	// compared to the actual measurements
//	errorEMA += ((newValue - smoothValue) - errorEMA) * 0.4;
//
//	// if sleep has been enabled, keep track of when we're asleep or not by marking the time of last activity and testing to see how much time has passed since then
//	if (SLEEP_ENABLE) {
//		// recalculate sleeping status
//		// (asleep if last activity was over SLEEP_DELAY_MS ago)
//		sleeping = abs(errorEMA) < ACTIVITY_THRESHOLD;
//	}
//
//
//	if (SLEEP_ENABLE && sleeping) {
//		return smoothValue;
//	}
//
//
//
//	auto snap = snapCurve(diff * SNAP_MULTIPLIER);
//
//	// when sleep is enabled, the emphasis is stopping on a responsiveValue quickly, and it's less about easing into position. If sleep is enabled, add a small amount to snap so it'll tend to snap into a more accurate position before sleeping starts.
//	if (SLEEP_ENABLE) {
//		snap = snap*0.5 + 0.5;
//	}
//
//	// calculate the exponential moving average based on the snap
//	smoothValue += (newValue - smoothValue) * snap;
//
//	return smoothValue;
//
//}


std::shared_ptr<cwc::glShader> ShaderManagerWrapper::LoadFromFile(const char* vertexFile, const char* fragmentFile)
{

	std::string key = std::string(vertexFile) + fragmentFile;

	if (cachedShaders.find(key) != cachedShaders.end())
	{
		return cachedShaders[key];
	}

	std::shared_ptr<cwc::glShader> result;

	if (!shaderManager)
	{
		shaderManager = std::make_shared<cwc::glShaderManager>();
	}

	cwc::glShader* shaderPtr = shaderManager->loadfromFile(vertexFile, fragmentFile);

	if (shaderPtr)
	{
		result = std::shared_ptr<cwc::glShader>(shaderPtr);

		cachedShaders[key] = result;
	}
	else
	{
		qDebug() << "Error Loading, compiling or linking shader\n";
	}

	return result;
}

void ShaderManagerWrapper::Clear()
{
	cachedShaders.clear();
	shaderManager.reset();
}