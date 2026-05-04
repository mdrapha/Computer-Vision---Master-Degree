#pragma once

#include "config.hpp"
#include "plate_detector.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

std::vector<std::filesystem::path> listJpgImages(const std::filesystem::path& inputDir);
void ensureDirectory(const std::filesystem::path& dir);
cv::Mat drawDetection(const cv::Mat& image, const DetectionResult& result, const DetectionParams& params);
void writeDetectionsCsv(const std::filesystem::path& csvPath, const std::vector<DetectionResult>& results);
void writeClassificationTemplate(const std::filesystem::path& csvPath,
                                 const std::vector<DetectionResult>& results);
