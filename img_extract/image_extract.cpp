#include "image_extract.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include <memory>

using namespace std;



void drawLine(cv::Vec2f line, cv::Mat& img, cv::Scalar rgb = CV_RGB(0, 0, 255))
{
	if (line[1] != 0)
	{
		float m = -1 / tan(line[1]);

		float c = line[0] / sin(line[1]);

		cv::line(img, cv::Point(0, c), cv::Point(img.size().width, m * img.size().width + c), rgb);
	}
	else
	{
		cv::line(img, cv::Point(line[0], 0), cv::Point(line[0], img.size().height), rgb);
	}

}

void mergeRelatedLines(vector<cv::Vec2f>* lines, cv::Mat& img)
{
	using namespace cv;
	vector<Vec2f>::iterator current;
	for (current = lines->begin(); current != lines->end(); current++)
	{
		if ((*current)[0] == 0 && (*current)[1] == -100) continue;
		float p1 = (*current)[0];
		float theta1 = (*current)[1];
		Point pt1current, pt2current;
		if (theta1 > CV_PI * 45 / 180 && theta1 < CV_PI * 135 / 180)
		{
			pt1current.x = 0;

			pt1current.y = p1 / sin(theta1);

			pt2current.x = img.size().width;
			pt2current.y = -pt2current.x / tan(theta1) + p1 / sin(theta1);
		}
		else
		{
			pt1current.y = 0;

			pt1current.x = p1 / cos(theta1);

			pt2current.y = img.size().height;
			pt2current.x = -pt2current.y / tan(theta1) + p1 / cos(theta1);

		}
		vector<Vec2f>::iterator    pos;
		for (pos = lines->begin(); pos != lines->end(); pos++)
		{
			if (*current == *pos) continue;
			if (fabs((*pos)[0] - (*current)[0]) < 20 && fabs((*pos)[1] - (*current)[1]) < CV_PI * 10 / 180)
			{
				float p = (*pos)[0];
				float theta = (*pos)[1];
				Point pt1, pt2;
				if ((*pos)[1] > CV_PI * 45 / 180 && (*pos)[1] < CV_PI * 135 / 180)
				{
					pt1.x = 0;
					pt1.y = p / sin(theta);
					pt2.x = img.size().width;
					pt2.y = -pt2.x / tan(theta) + p / sin(theta);
				}
				else
				{
					pt1.y = 0;
					pt1.x = p / cos(theta);
					pt2.y = img.size().height;
					pt2.x = -pt2.y / tan(theta) + p / cos(theta);
				}
				if (((double)(pt1.x - pt1current.x) * (pt1.x - pt1current.x) + (pt1.y - pt1current.y) * (pt1.y - pt1current.y) < 64 * 64) &&
					((double)(pt2.x - pt2current.x) * (pt2.x - pt2current.x) + (pt2.y - pt2current.y) * (pt2.y - pt2current.y) < 64 * 64))
				{
					// Merge the two
					(*current)[0] = ((*current)[0] + (*pos)[0]) / 2;

					(*current)[1] = ((*current)[1] + (*pos)[1]) / 2;

					(*pos)[0] = 0;
					(*pos)[1] = -100;
				}
			}
		}
	}
}

int main_2()
{
	cout << "Hello CMake." << endl;
	cv::Mat img = cv::imread(RES_PATH"sudoku.jpg", 0);

	if (img.data == nullptr)
		std::cout << "couldnt load." << std::endl;

	cv::GaussianBlur(img, img, cv::Size(5, 5), 0);
	cv::Mat outerBox = cv::Mat(img.size(), CV_8UC1);
	cv::adaptiveThreshold(img, outerBox, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 5, 2);
	cv::bitwise_not(outerBox, outerBox);

	cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	cv::dilate(outerBox, outerBox, kernel);
	
	int count = 0;
	int max = -1;

	cv::Point maxPt;

	for (int y = 0; y < outerBox.size().height; y++)
	{
		uchar* row = outerBox.ptr(y);
		for (int x = 0; x < outerBox.size().width; x++)
		{
			if (row[x] >= 128)
			{

				int area = cv::floodFill(outerBox, cv::Point(x, y), CV_RGB(0, 0, 64));

				if (area > max)
				{
					maxPt = cv::Point(x, y);
					max = area;
				}
			}
		}

	}

	cv::floodFill(outerBox, maxPt, CV_RGB(255, 255, 255));
	for (int y = 0; y < outerBox.size().height; y++)
	{
		uchar* row = outerBox.ptr(y);
		for (int x = 0; x < outerBox.size().width; x++)
		{
			if (row[x] == 64 && x != maxPt.x && y != maxPt.y)
			{
				int area = floodFill(outerBox, cv::Point(x, y), CV_RGB(0, 0, 0));
			}
		}
	}

	vector<cv::Vec2f> lines;
	cv::HoughLines(outerBox, lines, 1, CV_PI / 60, 200);

	mergeRelatedLines(&lines, outerBox); // Add this line

	for (int i = 0; i < lines.size(); i++)
	{
		drawLine(lines[i], outerBox, CV_RGB(0, 0, 128));
	}

	cv::imshow("original", outerBox);
	cv::waitKey();

	return 0;
}


int main()
{
	cv::Mat img = cv::imread(RES_PATH"sudoku.jpg");

	if (img.data == nullptr)
		std::cout << "couldnt load." << std::endl;

	cv::Mat outerBox = cv::Mat(img.size(), CV_8UC1);
	cv::cvtColor(img, outerBox, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	cv::GaussianBlur(outerBox, outerBox, cv::Size(9, 9), 0);
	cv::adaptiveThreshold(outerBox, outerBox, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
	cv::bitwise_not(outerBox, outerBox);

	cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	cv::dilate(outerBox, outerBox, kernel);

	std::vector<std::vector<cv::Point> > contour_points;
	cv::findContours(outerBox, contour_points, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

	// sort by contour Area, from max to min
	std::sort(std::begin(contour_points), std::end(contour_points), [](const std::vector<cv::Point>& rhs, const std::vector<cv::Point>& lhs)
		{
			auto A_r = std::abs(cv::contourArea(rhs));
			auto A_l = std::abs(cv::contourArea(lhs));
			return A_r > A_l;
		}
	);

	std::vector<cv::Point2f> max_corners{ contour_points[0][0], contour_points[0][1], contour_points[0][2], contour_points[0][3] };
	for (const auto& p2 : contour_points[0])
	{
		auto p = cv::Point2f(p2);
		if (cv::norm(max_corners[2]-cv::Point2f(0,0)) < cv::norm(p-cv::Point2f(0,0)))
			max_corners[2] = p;	   
		if (cv::norm(max_corners[1] - cv::Point2f(0, 1000)) < cv::norm(p - cv::Point2f(0, 1000)))
			max_corners[1] = p;	   
								   
		if (cv::norm(max_corners[3] - cv::Point2f(1000, 0)) < cv::norm(p - cv::Point2f(1000, 0)))
			max_corners[3] = p;	   
		if (cv::norm(max_corners[0] - cv::Point2f(1000, 1000)) < cv::norm(p - cv::Point2f(1000, 1000)))
			max_corners[0] = p;
	}

	//for (const auto& p : max_corners)
	//{
	//	circle(img, p, 1, cv::Scalar(0, 0, 255), 8);
	//}


	// wrap

	std::array edge_distances =
	{
		cv::norm(max_corners[0] - max_corners[1]),
		cv::norm(max_corners[0] - max_corners[2]),
		cv::norm(max_corners[1] - max_corners[3]),
		cv::norm(max_corners[2] - max_corners[3])
	};

	auto max_edge_distance = *std::max_element(std::cbegin(edge_distances), std::cend(edge_distances));

	std::array dst_points =
	{
		cv::Point2f(0,0),
		cv::Point2f(max_edge_distance-1 , 0),
		cv::Point2f(max_edge_distance-1, max_edge_distance-1),
		cv::Point2f(0, max_edge_distance-1),
	};

	cv::Mat undistorted = cv::Mat(cv::Size(max_edge_distance, max_edge_distance), img.type());
	cv::warpPerspective(img, undistorted, cv::getPerspectiveTransform(max_corners, dst_points), cv::Size(max_edge_distance, max_edge_distance));


	const auto approx_box_size = max_edge_distance / 9.0;
	const auto max_A = approx_box_size * approx_box_size * 0.8;
	const auto min_A = approx_box_size * approx_box_size * 0.15;

	cv::Mat undistorted_proc = cv::Mat(undistorted.size(), CV_8UC1);
	cv::cvtColor(undistorted, undistorted_proc, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	cv::GaussianBlur(undistorted_proc, undistorted_proc, cv::Size(5, 5), 0);
	cv::adaptiveThreshold(undistorted_proc, undistorted_proc, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 7, 2);
	cv::bitwise_not(undistorted_proc, undistorted_proc);

	//cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	cv::dilate(undistorted_proc, undistorted_proc, kernel);
	cv::floodFill(undistorted_proc, cv::Point(3, 3), cv::Scalar(0));

	for (uint8_t i = 0; i < 81; ++i)
	{
		uint8_t c = i / 9;
		uint8_t r = i % 9;
		const double tile_width = approx_box_size;
		cv::Mat tile = undistorted_proc.colRange(c * tile_width, (c + 1) * tile_width).rowRange(r * tile_width, (r + 1) * tile_width);
		cv::Rect start(c * tile_width, (c + 1) * tile_width, r * tile_width, (r + 1) * tile_width);

		cv::findContours(tile, contour_points, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
		for (const auto& contour : contour_points)
		{
			cv::Rect bbox = cv::boundingRect(contour);
			auto A = bbox.area();
			bbox.x += c * tile_width;
			bbox.y += r * tile_width;
			if (A <= max_A && min_A <= A)
			{
				cv::rectangle(undistorted, bbox, cv::Scalar(0, 0, 255), 2);
			}


		}
	}

	cv::imshow("original", undistorted);
	cv::waitKey();

	return 0;

}

