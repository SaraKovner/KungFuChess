#include "OpenCvImg.hpp"

#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>

struct OpenCvImg::Impl {
	cv::Mat mat;
};

OpenCvImg::OpenCvImg() : impl(std::make_unique<Impl>()) {}
OpenCvImg::~OpenCvImg() = default;
ImgPtr OpenCvImg::clone() const
{
	auto res = std::make_shared<OpenCvImg>();
	res->impl->mat = this->impl->mat.clone();
	return res;
}

void OpenCvImg::read(const std::string& path, const std::pair<int, int>& size) {
	impl->mat = cv::imread(path, cv::IMREAD_UNCHANGED);
	if (impl->mat.empty()) throw std::runtime_error("Cannot load image: " + path);
	if (size.first > 0 && size.second > 0) {
		cv::resize(impl->mat, impl->mat, cv::Size(size.first, size.second));
	}
}

std::pair<int,int> OpenCvImg::size() const {
	return {impl->mat.cols, impl->mat.rows};
}

void OpenCvImg::create_blank(int w, int h) {
	impl->mat = cv::Mat(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
}

void OpenCvImg::draw_on(Img& dst, int x, int y) {
	auto* cvDst = dynamic_cast<OpenCvImg*>(&dst);
	if (!cvDst) return;
	if (impl->mat.empty()) return;
	if (cvDst->impl->mat.empty()) return;
	
	// Check bounds to avoid crashes
	int dst_width = cvDst->impl->mat.cols;
	int dst_height = cvDst->impl->mat.rows;
	int src_width = impl->mat.cols;
	int src_height = impl->mat.rows;
	
	if (x >= dst_width || y >= dst_height || x + src_width <= 0 || y + src_height <= 0) {
		return; // Completely outside bounds
	}
	
	// Calculate the region to copy
	int copy_x = std::max(0, x);
	int copy_y = std::max(0, y);
	int copy_width = std::min(src_width, dst_width - copy_x);
	int copy_height = std::min(src_height, dst_height - copy_y);
	
	if (copy_width > 0 && copy_height > 0) {
		cv::Rect dst_rect(copy_x, copy_y, copy_width, copy_height);
		cv::Rect src_rect(copy_x - x, copy_y - y, copy_width, copy_height);
		
		// Convert source to match destination format if needed
		cv::Mat src_roi = impl->mat(src_rect);
		if (src_roi.channels() != cvDst->impl->mat.channels()) {
			if (src_roi.channels() == 4 && cvDst->impl->mat.channels() == 3) {
				cv::cvtColor(src_roi, src_roi, cv::COLOR_BGRA2BGR);
			} else if (src_roi.channels() == 3 && cvDst->impl->mat.channels() == 4) {
				cv::cvtColor(src_roi, src_roi, cv::COLOR_BGR2BGRA);
			}
		}
		
		src_roi.copyTo(cvDst->impl->mat(dst_rect));
	}
}

void OpenCvImg::put_text(const std::string& txt, int x, int y, double font_size) {
	if (impl->mat.empty()) return;
	cv::putText(impl->mat, txt, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, font_size, cv::Scalar(255, 255, 255, 255));
}

void OpenCvImg::show() const {
	if (impl->mat.empty()) return;
	cv::namedWindow("KungFu Chess", cv::WINDOW_AUTOSIZE);
	cv::imshow("KungFu Chess", impl->mat);
	cv::waitKey(1); // Just refresh the window
}

void OpenCvImg::draw_rect(int x, int y, int width, int height, const std::vector<uint8_t> & color) {
	if (impl->mat.empty()) return;
	cv::Scalar cvColor = color.size() == 3 ? cv::Scalar(color[0], color[1], color[2]) : cv::Scalar(color[0], color[1], color[2], color[3]);
	cv::rectangle(impl->mat, cv::Rect(x, y, width, height), cvColor, 3); // 3 = border thickness
}

void OpenCvImg::close_all_windows() {
	cv::destroyAllWindows();
}