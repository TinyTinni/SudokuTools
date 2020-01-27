#include "image_extract.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "classifier/classifier.h"

#include <memory>
#include <cassert>

using namespace std;


cv::Mat dilate_image_step(const cv::Mat& colored_input_image)
{
	// todo: find better blur/threshold hyperparameters?
	cv::Mat outerBox = cv::Mat(colored_input_image.size(), CV_8UC1);
	cv::cvtColor(colored_input_image, outerBox, cv::ColorConversionCodes::COLOR_BGR2GRAY);

	//todo: find right parameters
	cv::GaussianBlur(outerBox, outerBox, cv::Size(5, 5), 0);
	cv::adaptiveThreshold(outerBox, outerBox, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 7, 2);
	cv::bitwise_not(outerBox, outerBox);

	cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	cv::dilate(outerBox, outerBox, kernel);
	return outerBox;
}


std::array<cv::Point2f, 4> get_grid_edge_points(const cv::Mat& dil_image)
{
	std::vector<std::vector<cv::Point> > contour_points;
	cv::findContours(dil_image, contour_points, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);

	//todo: error if no contour was found

	// sort by contour Area, from max to min
	std::sort(std::begin(contour_points), std::end(contour_points), [](const auto& lhs, const auto& rhs)
		{
			auto A_l = std::abs(cv::contourArea(lhs));
			auto A_r = std::abs(cv::contourArea(rhs));
			return A_l > A_r;
		}
	);
	// contout_points[0] is the contour with the biggest area

	const unsigned width = dil_image.cols;
	const unsigned height = dil_image.rows;

	std::array<cv::Point2f, 4> corners{ contour_points[0][0], contour_points[0][1], contour_points[0][2], contour_points[0][3] };

	for (const auto& p2 : contour_points[0])
	{
		auto p = cv::Point2f(p2);
		if (cv::norm(corners[2] - cv::Point2f(0, 0)) < cv::norm(p - cv::Point2f(0, 0)))
			corners[2] = p;
		if (cv::norm(corners[1] - cv::Point2f(0, height)) < cv::norm(p - cv::Point2f(0, height)))
			corners[1] = p;

		if (cv::norm(corners[3] - cv::Point2f(width, 0)) < cv::norm(p - cv::Point2f(width, 0)))
			corners[3] = p;
		if (cv::norm(corners[0] - cv::Point2f(width, height)) < cv::norm(p - cv::Point2f(width, height)))
			corners[0] = p;
	}
	return corners;
}

std::vector< cv::Mat > get_tiles_with_numbers(const cv::Mat& unwraped_dil_img, const cv::Mat& unwraped_col_img)
{
	assert(unwraped_dil_img.rows == unwraped_dil_img.cols);

	const auto approx_box_size = unwraped_dil_img.rows / 9.0;
	const auto max_A = approx_box_size * approx_box_size * 0.8;
	const auto min_A = approx_box_size * approx_box_size * 0.15;


	std::vector<cv::Mat> result;


	for (uint8_t i = 0; i < 81; ++i)
	{
		uint8_t c = i / 9;
		uint8_t r = i % 9;
		const double tile_width = approx_box_size;
		cv::Mat tile = unwraped_dil_img.colRange(c * tile_width, (c + 1) * tile_width).rowRange(r * tile_width, (r + 1) * tile_width);
		cv::Rect start(c * tile_width, (c + 1) * tile_width, r * tile_width, (r + 1) * tile_width);

		std::vector<std::vector<cv::Point> > contour_points;
		cv::findContours(tile, contour_points, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
		// get biggest contout

		for (const auto& contour : contour_points)
		{
			cv::Rect bbox = cv::boundingRect(contour);
			auto A = bbox.area();
			bbox.x += c * tile_width;
			bbox.y += r * tile_width;
			if (A <= max_A && min_A <= A)
			{
				//cv::imshow("bla", tile);
				//cv::waitKey();
				//cv::rectangle(unwraped_dil_img, bbox, cv::Scalar(0, 0, 255), 2);
				result.emplace_back(cv::Mat{ unwraped_dil_img, bbox });
				//result.emplace_back(tile);

			}


		}
	}
	return result;
}

int main()
{
	cv::Mat img = cv::imread(RES_PATH"sudoku.jpg");

	if (img.data == nullptr)
		std::cout << "couldnt load." << std::endl;

	cv::Mat dilated_image = dilate_image_step(img);
	const auto grid_corners = get_grid_edge_points(dilated_image);


	// try to remove the grid
	cv::floodFill(dilated_image, grid_corners[0], cv::Scalar(0));
	cv::floodFill(dilated_image, grid_corners[1], cv::Scalar(0));
	cv::floodFill(dilated_image, grid_corners[2], cv::Scalar(0));
	cv::floodFill(dilated_image, grid_corners[3], cv::Scalar(0));
	
	// wrap
	const std::array edge_distances =
	{
		cv::norm(grid_corners[0] - grid_corners[1]),
		cv::norm(grid_corners[0] - grid_corners[2]),
		cv::norm(grid_corners[1] - grid_corners[3]),
		cv::norm(grid_corners[2] - grid_corners[3])
	};

	auto max_edge_distance = *std::max_element(std::cbegin(edge_distances), std::cend(edge_distances));

	std::array dst_points =
	{
		cv::Point2f(0,0),
		cv::Point2f(max_edge_distance-1 , 0),
		cv::Point2f(max_edge_distance-1, max_edge_distance-1),
		cv::Point2f(0, max_edge_distance-1),
	};

	const auto perspective_transform = cv::getPerspectiveTransform(grid_corners, dst_points);
	const auto undistorted_size = cv::Size(max_edge_distance, max_edge_distance);

	cv::Mat undistorted_colored = cv::Mat(undistorted_size, img.type());
	cv::warpPerspective(img, undistorted_colored, perspective_transform, undistorted_size);

	cv::Mat undistorted_dil = cv::Mat(undistorted_size, dilated_image.type());
	cv::warpPerspective(dilated_image, undistorted_dil, perspective_transform, undistorted_size);

	const auto number_tiles = get_tiles_with_numbers(undistorted_dil, undistorted_colored);


	// classify number tiles
	classifier classi;
	for (int i = 0; i < number_tiles.size(); ++ i)
	{
		
		cv::Mat dst = cv::Mat(cv::Size(28, 28), number_tiles[i].type());
		cv::resize(number_tiles[i], dst, dst.size(), 0, 0);



		const auto o = classi.classify(dst);
		std::cout << "classified img_" << i << "\tnum: " << o.number << "\tconfidence: " << o.confidence << std::endl;
		cv::imshow("tile", dst);
		cv::waitKey();
		//cv::imwrite(std::string("img_") + std::to_string(i) + std::string(".jpg"), dst);
	}

	//cv::imshow("original", undistorted_colored);
	//cv::waitKey();

	//cv::imwrite("undistorted.png", undistorted_colored);

	return 0;

}

