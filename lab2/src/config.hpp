#pragma once

#include <string>
#include <vector>

struct DetectionParams {
    std::string name = "balanced";
    double dp = 1.2;
    double minDist = 90.0;
    double cannyThreshold = 120.0;
    double accumulatorThreshold = 36.0;
    int minRadius = 90;
    int maxRadius = 240;
    int medianKernel = 5;
    int gaussianKernel = 7;
    double gaussianSigma = 1.5;
    bool useClahe = false;
};

struct AppConfig {
    std::string inputDir = "sel_data";
    std::string outputDir = "results";
    bool runSweep = false;
    std::vector<DetectionParams> experiments;
};

DetectionParams makeBalancedPreset();
DetectionParams makeSensitivePreset();
DetectionParams makeStrictPreset();
std::vector<DetectionParams> makeDefaultSweep();
AppConfig parseArguments(int argc, char** argv);
