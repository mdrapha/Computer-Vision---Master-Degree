#include "config.hpp"
#include "io_utils.hpp"
#include "plate_detector.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <vector>

#include <opencv2/imgcodecs.hpp>

namespace {

std::vector<DetectionResult> runExperiment(const std::vector<std::filesystem::path>& imagePaths,
                                           const DetectionParams& params,
                                           const std::filesystem::path& outputDir) {
    PlateDetector detector(params);
    std::vector<DetectionResult> results;
    results.reserve(imagePaths.size());

    const std::filesystem::path imageOutputDir = outputDir / "images";
    ensureDirectory(imageOutputDir);

    for (const auto& imagePath : imagePaths) {
        const std::string imageName = imagePath.filename().string();
        cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "[WARN] Não foi possível ler " << imagePath << '\n';
            continue;
        }

        DetectionResult result = detector.detect(image, imageName);
        cv::Mat annotated = drawDetection(image, result, params);
        cv::imwrite((imageOutputDir / imageName).string(), annotated);
        results.push_back(std::move(result));
    }

    return results;
}

void printSummary(const DetectionParams& params, const std::vector<DetectionResult>& results) {
    std::size_t detectedCount = 0;
    for (const DetectionResult& result : results) {
        if (result.detected) {
            ++detectedCount;
        }
    }

    std::cout << "[INFO] preset=" << params.name
              << " | images=" << results.size()
              << " | detected=" << detectedCount
              << " | not_detected=" << (results.size() - detectedCount)
              << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const AppConfig config = parseArguments(argc, argv);
        const std::vector<std::filesystem::path> imagePaths = listJpgImages(config.inputDir);

        for (const DetectionParams& params : config.experiments) {
            const std::filesystem::path experimentDir = std::filesystem::path(config.outputDir) / params.name;
            ensureDirectory(experimentDir);

            std::vector<DetectionResult> results = runExperiment(imagePaths, params, experimentDir);
            writeDetectionsCsv(experimentDir / "detections.csv", results);
            writeClassificationTemplate(experimentDir / "classification_template.csv", results);
            printSummary(params, results);
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << '\n';
        return 1;
    }
}
