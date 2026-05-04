#pragma once

#include "config.hpp"

#include <opencv2/core.hpp>
#include <string>
#include <vector>

struct CircleCandidate {
    cv::Vec3f circle;
    double score = 0.0;
};

struct DetectionResult {
    std::string imageName;
    bool detected = false;
    CircleCandidate bestCandidate;
    std::vector<CircleCandidate> candidates;
    cv::Mat preprocessedGray;
};

class PlateDetector {
public:
    explicit PlateDetector(DetectionParams params);
    DetectionResult detect(const cv::Mat& bgrImage, const std::string& imageName) const;

private:
    cv::Mat preprocess(const cv::Mat& bgrImage) const;
    std::vector<CircleCandidate> detectCandidates(const cv::Mat& gray) const;
    cv::Mat buildEdgeMap(const cv::Mat& gray) const;
    CircleCandidate selectBestCandidate(const std::vector<CircleCandidate>& candidates,
                                        const cv::Size& imageSize) const;
    double computeScore(const cv::Vec3f& circle, const cv::Size& imageSize) const;

    DetectionParams params_;
};
