#ifndef DRAW_UTIL_OBJECT
#define DRAW_UTIL_OBJECT

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>

class DrawUtil
{

public:
	DrawUtil();
	~DrawUtil();

	static void rotateAndSize(double angle, double width)
	{
		
	}

	//=============================================================================
	//function to draw points with antialiasing
	static void drawPoint(const float xc, const float yc, const float rad,
		cv::Mat &I, const cv::Scalar &color, const bool full) {
		int x0 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(floor(xc - rad))));
		int x1 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(ceil(xc + rad))));
		int y0 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(floor(yc - rad))));
		int y1 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(ceil(yc + rad))));
		cv::Vec3b colorB(color[0], color[1], color[2]);
		if (!full) {
			for (int y = y0; y <= y1; y++) {
				for (int x = x0; x <= x1; x++) {
					float vx = x - xc, vy = y - yc;
					float v = fabsf(sqrtf(vx * vx + vy * vy) - rad);
					if (v <= 1.0f) {
						float w = 1.0f - v;
						if (y <= 1 || y >= I.rows || x <= 1 || x >= I.cols) return;
						I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
					}
				}
			}
		}
		else {
			for (int y = y0; y <= y1; y++) {
				for (int x = x0; x <= x1; x++) {
					float vx = x - xc, vy = y - yc;
					float v = sqrtf(vx * vx + vy * vy) - rad;
					if (v <= 0) {
						I.at<cv::Vec3b>(y, x) = colorB;
					}
					else if (v <= 1.0f) {
						float w = 1.0f - v;
						if (y <= 1 || y >= I.rows || x <= 1 || x >= I.cols) return;
						I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
					}
				}
			}
		}
	}
	//=============================================================================
	//function to draw points with antialiasing
	static void drawPointAA(const float xc, const float yc, const float rad,
		cv::Mat &I, const cv::Scalar &color, const bool full) {
		int x0 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(floor(xc - rad))));
		int x1 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(ceil(xc + rad))));
		int y0 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(floor(yc - rad))));
		int y1 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(ceil(yc + rad))));
		cv::Vec3b colorB(color[0], color[1], color[2]);
		if (!full) {
			for (int y = y0; y <= y1; y++) {
				for (int x = x0; x <= x1; x++) {
					float vx = x - xc, vy = y - yc;
					float v = fabsf(sqrtf(vx * vx + vy * vy) - rad);
					if (v <= 1.0f) {
						float w = 1.0f - v;
						if (y <= 1 || y >= I.rows || x <= 1 || x >= I.cols) return;
						I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
					}
				}
			}
		}
		else {
			for (int y = y0; y <= y1; y++) {
				for (int x = x0; x <= x1; x++) {
					float vx = x - xc, vy = y - yc;
					float v = sqrtf(vx * vx + vy * vy) - rad;
					if (v <= 0) {
						I.at<cv::Vec3b>(y, x) = colorB;
					}
					else if (v <= 1.0f) {
						float w = 1.0f - v;
						if (y <= 1 || y >= I.rows || x <= 1 || x >= I.cols) return;
						I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
					}
				}
			}
		}
	}

	
	static void plot(int x, int y, float w, cv::Mat &I, const cv::Scalar &color, const float alpha = 0.5f) 
	{
		
			if (w > 1) {
				std::cout << "too large: " << w << std::endl; exit(0);
			}
			float wa = w * alpha;
			int c = I.cols; float v = 1.0f - wa;
			cv::Vec3b colorB(color[0], color[1], color[2]);
			for (int k = 0; k < 3; k++) {
				if (y <= 1 || y >= I.rows || x <= 1 || x >= I.cols) return;

				I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + wa * colorB;
			}
		}
		static int ipart(float x) { return static_cast<int>(floor(x)); }
		static int round(float x) { return ipart(x + 0.5f); }
		static float fpart(float x) { return x - ipart(x); }
		static float rfpart(float x) { return 1.0f - fpart(x); }
		static void drawLine(float x0, float y0, float x1, float y1,
			cv::Mat &I, const cv::Scalar &c) {
			if ((x0 < 0) || (x0 >= I.cols) ||
				(x1 < 0) || (x1 >= I.cols) ||
				(y0 < 0) || (y0 >= I.rows) ||
				(y1 < 0) || (y1 >= I.rows)) {
				return;
			}

			bool steep = fabs(y1 - y0) > fabs(x1 - x0);
			if (steep) {
				{ float v = x0; x0 = y0; y0 = v; }
	  { float v = x1; x1 = y1; y1 = v; }
			}
			if (x0 > x1) {
				{ float v = x0; x0 = x1; x1 = v; }
	  { float v = y0; y0 = y1; y1 = v; }
			}
			float dx = x1 - x0;
			float dy = y1 - y0;
			float gradient = dy / dx;

			// handle first endpoint
			int xend = round(x0);
			float yend = y0 + gradient * (xend - x0);
			float xgap = rfpart(x0 + 0.5f);
			int xpxl1 = xend;   //this will be used in the main loop
			int ypxl1 = ipart(yend);
			if (steep) {
				plot(ypxl1, xpxl1, rfpart(yend) * xgap, I, c);
				plot(ypxl1 + 1, xpxl1, fpart(yend) * xgap, I, c);
			}
			else {
				plot(xpxl1, ypxl1, rfpart(yend) * xgap, I, c);
				plot(xpxl1, ypxl1 + 1, fpart(yend) * xgap, I, c);
			}
			float intery = yend + gradient; // first y-intersection for the main loop

			// handle second endpoint
			xend = round(x1);
			yend = y1 + gradient * (xend - x1);
			xgap = fpart(x1 + 0.5f);
			int xpxl2 = xend; //this will be used in the main loop
			int ypxl2 = ipart(yend);
			if (steep) {
				plot(ypxl2, xpxl2, rfpart(yend) * xgap, I, c);
				plot(ypxl2 + 1, xpxl2, fpart(yend) * xgap, I, c);
			}
			else {
				plot(xpxl2, ypxl2, rfpart(yend) * xgap, I, c);
				plot(xpxl2, ypxl2 + 1, fpart(yend) * xgap, I, c);
			}
			// main loop
			for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
				if (steep) {
					plot(ipart(intery), x, rfpart(intery),
						I, c);
					plot(ipart(intery) + 1, x, fpart(intery),
						I, c);
				}
				else {
					plot(x, ipart(intery), rfpart(intery),
						I, c);
					plot(x, ipart(intery) + 1, fpart(intery),
						I, c);
				}
				intery = intery + gradient;
			}
		}
};

#endif 