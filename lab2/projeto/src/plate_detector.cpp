#include "plate_detector.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

#include <opencv2/imgproc.hpp>

PlateDetector::PlateDetector(DetectionParams params) : params_(std::move(params)) {}

DetectionResult PlateDetector::detect(const cv::Mat& bgrImage, const std::string& imageName) const {
    if (bgrImage.empty()) {
        throw std::runtime_error("Imagem vazia em detect(): " + imageName);
    }

    DetectionResult result;
    result.imageName = imageName;
    result.preprocessedGray = preprocess(bgrImage);
    result.candidates = detectCandidates(result.preprocessedGray);
    result.detected = !result.candidates.empty();

    if (result.detected) {
        result.bestCandidate = selectBestCandidate(result.candidates, bgrImage.size());
    }

    return result;
}

cv::Mat PlateDetector::preprocess(const cv::Mat& bgrImage) const {
    cv::Mat gray;
    cv::cvtColor(bgrImage, gray, cv::COLOR_BGR2GRAY);

    if (params_.useClahe) {
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        clahe->apply(gray, gray);
    }

    int medianKernel = std::max(3, params_.medianKernel | 1);
    int gaussianKernel = std::max(3, params_.gaussianKernel | 1);

    cv::medianBlur(gray, gray, medianKernel);
    cv::GaussianBlur(gray, gray, cv::Size(gaussianKernel, gaussianKernel), params_.gaussianSigma);
    return gray;
}

cv::Mat PlateDetector::buildEdgeMap(const cv::Mat& gray) const {
    cv::Mat edges;
    const double lowThreshold = std::max(20.0, params_.cannyThreshold * 0.5);
    cv::Canny(gray, edges, lowThreshold, params_.cannyThreshold, 3);
    return edges;
}

std::vector<CircleCandidate> PlateDetector::detectCandidates(const cv::Mat& gray) const {
    const cv::Mat edges = buildEdgeMap(gray);

    cv::Mat gradX;
    cv::Mat gradY;
    cv::Sobel(gray, gradX, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gradY, CV_32F, 0, 1, 3);

    std::vector<cv::Point> edgePoints;
    edgePoints.reserve(static_cast<std::size_t>(gray.rows * gray.cols / 8));
    for (int y = 0; y < edges.rows; ++y) {
        const uchar* edgeRow = edges.ptr<uchar>(y);
        for (int x = 0; x < edges.cols; ++x) {
            if (edgeRow[x] != 0) {
                edgePoints.emplace_back(x, y);
            }
        }
    }

    if (edgePoints.empty()) {
        return {};
    }

    const double accumulatorScale = std::max(1.0, params_.dp);
    const int accCols = std::max(1, static_cast<int>(std::ceil(gray.cols / accumulatorScale)));
    const int accRows = std::max(1, static_cast<int>(std::ceil(gray.rows / accumulatorScale)));
    const int radiusStep = params_.name == "sensitive" ? 2 : 3;
    const int peakWindow = std::max(1, static_cast<int>(std::round(params_.minDist / accumulatorScale / 2.0)));
    const int maxCandidates = params_.name == "sensitive" ? 18 : 12;
    const double voteThreshold = std::max(8.0, params_.accumulatorThreshold * 0.35);

    auto edgeSupportRatio = [&edges](const cv::Vec3f& circle) -> double {
        const double centerX = circle[0];
        const double centerY = circle[1];
        const int radius = std::max(1, cvRound(circle[2]));
        const int samples = std::max(48, radius);
        int supported = 0;

        for (int i = 0; i < samples; ++i) {
            const double angle = 2.0 * CV_PI * static_cast<double>(i) / static_cast<double>(samples);
            const double cosA = std::cos(angle);
            const double sinA = std::sin(angle);
            bool foundEdge = false;

            for (int delta = -2; delta <= 2; ++delta) {
                const int x = cvRound(centerX + (radius + delta) * cosA);
                const int y = cvRound(centerY + (radius + delta) * sinA);
                if (x < 0 || y < 0 || x >= edges.cols || y >= edges.rows) {
                    continue;
                }
                if (edges.at<uchar>(y, x) != 0) {
                    foundEdge = true;
                    break;
                }
            }

            if (foundEdge) {
                ++supported;
            }
        }

        return static_cast<double>(supported) / static_cast<double>(samples);
    };

    std::vector<CircleCandidate> candidates;
    for (int radius = params_.minRadius; radius <= params_.maxRadius; radius += radiusStep) {
        cv::Mat accumulator = cv::Mat::zeros(accRows, accCols, CV_32S);

        for (const cv::Point& point : edgePoints) {
            const float gx = gradX.at<float>(point.y, point.x);
            const float gy = gradY.at<float>(point.y, point.x);
            const float magnitude = std::sqrt(gx * gx + gy * gy);
            if (magnitude < 1e-3f) {
                continue;
            }

            const float dirX = gx / magnitude;
            const float dirY = gy / magnitude;

            for (int sign : {-1, 1}) {
                const float centerX = static_cast<float>(point.x) + sign * radius * dirX;
                const float centerY = static_cast<float>(point.y) + sign * radius * dirY;

                if (centerX < 0.0f || centerY < 0.0f ||
                    centerX >= static_cast<float>(gray.cols) || centerY >= static_cast<float>(gray.rows)) {
                    continue;
                }

                const int accX = static_cast<int>(centerX / accumulatorScale);
                const int accY = static_cast<int>(centerY / accumulatorScale);
                for (int oy = -1; oy <= 1; ++oy) {
                    const int yy = accY + oy;
                    if (yy < 0 || yy >= accumulator.rows) {
                        continue;
                    }
                    for (int ox = -1; ox <= 1; ++ox) {
                        const int xx = accX + ox;
                        if (xx < 0 || xx >= accumulator.cols) {
                            continue;
                        }
                        accumulator.at<int>(yy, xx) += (ox == 0 && oy == 0) ? 2 : 1;
                    }
                }
            }
        }

        for (int accY = 0; accY < accumulator.rows; ++accY) {
            for (int accX = 0; accX < accumulator.cols; ++accX) {
                const int votes = accumulator.at<int>(accY, accX);
                if (votes < static_cast<int>(voteThreshold)) {
                    continue;
                }

                bool isLocalMaximum = true;
                const int y0 = std::max(0, accY - peakWindow);
                const int y1 = std::min(accumulator.rows - 1, accY + peakWindow);
                const int x0 = std::max(0, accX - peakWindow);
                const int x1 = std::min(accumulator.cols - 1, accX + peakWindow);
                for (int ny = y0; ny <= y1 && isLocalMaximum; ++ny) {
                    for (int nx = x0; nx <= x1; ++nx) {
                        if (ny == accY && nx == accX) {
                            continue;
                        }
                        if (accumulator.at<int>(ny, nx) > votes) {
                            isLocalMaximum = false;
                            break;
                        }
                    }
                }

                if (!isLocalMaximum) {
                    continue;
                }

                CircleCandidate candidate;
                candidate.circle = cv::Vec3f((accX + 0.5f) * static_cast<float>(accumulatorScale),
                                             (accY + 0.5f) * static_cast<float>(accumulatorScale),
                                             static_cast<float>(radius));
                const double supportRatio = edgeSupportRatio(candidate.circle);
                if (supportRatio < 0.16) {
                    continue;
                }

                candidate.score = static_cast<double>(votes) * supportRatio;
                candidates.push_back(candidate);
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const CircleCandidate& a, const CircleCandidate& b) {
        return a.score > b.score;
    });

    std::vector<CircleCandidate> filtered;
    filtered.reserve(std::min<std::size_t>(candidates.size(), static_cast<std::size_t>(maxCandidates)));
    for (const CircleCandidate& candidate : candidates) {
        bool tooClose = false;
        for (const CircleCandidate& kept : filtered) {
            const float dx = candidate.circle[0] - kept.circle[0];
            const float dy = candidate.circle[1] - kept.circle[1];
            const float dr = candidate.circle[2] - kept.circle[2];
            const float centerDistance = std::sqrt(dx * dx + dy * dy);
            if (centerDistance < static_cast<float>(params_.minDist) &&
                std::abs(dr) < static_cast<float>(radiusStep * 3)) {
                tooClose = true;
                break;
            }
        }

        if (!tooClose) {
            filtered.push_back(candidate);
        }

        if (filtered.size() >= static_cast<std::size_t>(maxCandidates)) {
            break;
        }
    }

    return filtered;
}

CircleCandidate PlateDetector::selectBestCandidate(const std::vector<CircleCandidate>& candidates,
                                                   const cv::Size& imageSize) const {
    std::vector<CircleCandidate> ranked = candidates;
    for (CircleCandidate& candidate : ranked) {
        const double geometryScore = computeScore(candidate.circle, imageSize);
        candidate.score = 0.75 * geometryScore + 0.25 * std::log1p(candidate.score);
    }

    const auto bestIt = std::max_element(
        ranked.begin(), ranked.end(), [](const CircleCandidate& a, const CircleCandidate& b) {
            return a.score < b.score;
        });

    return *bestIt;
}

double PlateDetector::computeScore(const cv::Vec3f& circle, const cv::Size& imageSize) const {
    const double x = circle[0];
    const double y = circle[1];
    const double radius = circle[2];

    const double imageCenterX = imageSize.width / 2.0;
    const double imageCenterY = imageSize.height / 2.0;
    const double dx = x - imageCenterX;
    const double dy = y - imageCenterY;
    const double centerDistance = std::sqrt(dx * dx + dy * dy);
    const double maxCenterDistance = std::sqrt(imageCenterX * imageCenterX + imageCenterY * imageCenterY);

    const double normalizedRadius = radius / (0.5 * std::min(imageSize.width, imageSize.height));
    const double centrality = 1.0 - std::min(1.0, centerDistance / maxCenterDistance);

    const double borderMargin = std::min({x - radius,
                                          y - radius,
                                          imageSize.width - (x + radius),
                                          imageSize.height - (y + radius)});
    const double borderPenalty = borderMargin < 0.0 ? 0.35 : 0.0;

    return 0.60 * normalizedRadius + 0.40 * centrality - borderPenalty;
}
