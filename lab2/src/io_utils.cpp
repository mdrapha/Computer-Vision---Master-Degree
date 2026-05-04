#include "io_utils.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

std::vector<std::filesystem::path> listJpgImages(const std::filesystem::path& inputDir) {
    if (!std::filesystem::exists(inputDir)) {
        throw std::runtime_error("Diretório de entrada não encontrado: " + inputDir.string());
    }

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto extension = entry.path().extension().string();
        if (extension == ".jpg" || extension == ".jpeg" || extension == ".JPG" || extension == ".JPEG") {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

void ensureDirectory(const std::filesystem::path& dir) {
    std::filesystem::create_directories(dir);
}

cv::Mat drawDetection(const cv::Mat& image, const DetectionResult& result, const DetectionParams& params) {
    cv::Mat annotated = image.clone();

    const cv::Scalar candidateColor(0, 255, 255);
    const cv::Scalar bestColor(0, 0, 255);
    const cv::Scalar centerColor(255, 0, 0);

    for (const CircleCandidate& candidate : result.candidates) {
        const cv::Point center(cvRound(candidate.circle[0]), cvRound(candidate.circle[1]));
        const int radius = cvRound(candidate.circle[2]);
        cv::circle(annotated, center, radius, candidateColor, 1, cv::LINE_AA);
    }

    if (result.detected) {
        const cv::Point center(cvRound(result.bestCandidate.circle[0]), cvRound(result.bestCandidate.circle[1]));
        const int radius = cvRound(result.bestCandidate.circle[2]);
        cv::circle(annotated, center, radius, bestColor, 3, cv::LINE_AA);
        cv::circle(annotated, center, 3, centerColor, -1, cv::LINE_AA);
    }

    std::ostringstream overlay;
    overlay << "preset=" << params.name;
    if (result.detected) {
        overlay << " | r=" << cvRound(result.bestCandidate.circle[2])
                << " | score=" << std::fixed << std::setprecision(3) << result.bestCandidate.score
                << " | candidates=" << result.candidates.size();
    } else {
        overlay << " | no circle detected";
    }

    cv::rectangle(annotated, cv::Rect(10, 10, 420, 30), cv::Scalar(0, 0, 0), cv::FILLED);
    cv::putText(annotated,
                overlay.str(),
                cv::Point(16, 31),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(255, 255, 255),
                1,
                cv::LINE_AA);

    return annotated;
}

void writeDetectionsCsv(const std::filesystem::path& csvPath, const std::vector<DetectionResult>& results) {
    std::ofstream file(csvPath);
    if (!file) {
        throw std::runtime_error("Não foi possível escrever: " + csvPath.string());
    }

    file << "image,detected,candidate_count,center_x,center_y,radius,score\n";
    file << std::fixed << std::setprecision(4);
    for (const DetectionResult& result : results) {
        file << result.imageName << ','
             << (result.detected ? "1" : "0") << ','
             << result.candidates.size();

        if (result.detected) {
            file << ',' << result.bestCandidate.circle[0]
                 << ',' << result.bestCandidate.circle[1]
                 << ',' << result.bestCandidate.circle[2]
                 << ',' << result.bestCandidate.score;
        } else {
            file << ",,,,";
        }

        file << '\n';
    }
}

void writeClassificationTemplate(const std::filesystem::path& csvPath,
                                 const std::vector<DetectionResult>& results) {
    std::ofstream file(csvPath);
    if (!file) {
        throw std::runtime_error("Não foi possível escrever: " + csvPath.string());
    }

    file << "image,visual_class,notes\n";
    for (const DetectionResult& result : results) {
        file << result.imageName << ",pending,\n";
    }
}
