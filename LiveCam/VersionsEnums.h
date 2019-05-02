enum class TransformVersion
{
	V1, // old transform has such bug, that as bigger a face on the image, as it is moved more far away from the camera,
		// so models far away hide models more closer
	V2 // this version fix previous bug and also add model autocenter & autoscale according to face width,
		// so now we do extrashift & extrascale relativly head position and face width
};