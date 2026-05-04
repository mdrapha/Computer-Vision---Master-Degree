#include "config.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace {

bool isFlag(const std::string& value, const std::string& shortName, const std::string& longName) {
    return value == shortName || value == longName;
}

DetectionParams presetByName(const std::string& name) {
    if (name == "balanced") {
        return makeBalancedPreset();
    }
    if (name == "sensitive") {
        return makeSensitivePreset();
    }
    if (name == "strict") {
        return makeStrictPreset();
    }

    throw std::runtime_error("Preset desconhecido: " + name);
}

void applyOverride(DetectionParams& params, const std::string& key, const std::string& value) {
    if (key == "--dp") {
        params.dp = std::stod(value);
    } else if (key == "--min-dist") {
        params.minDist = std::stod(value);
    } else if (key == "--canny") {
        params.cannyThreshold = std::stod(value);
    } else if (key == "--acc") {
        params.accumulatorThreshold = std::stod(value);
    } else if (key == "--min-radius") {
        params.minRadius = std::stoi(value);
    } else if (key == "--max-radius") {
        params.maxRadius = std::stoi(value);
    } else if (key == "--median") {
        params.medianKernel = std::stoi(value);
    } else if (key == "--gaussian") {
        params.gaussianKernel = std::stoi(value);
    } else if (key == "--sigma") {
        params.gaussianSigma = std::stod(value);
    } else {
        throw std::runtime_error("Parâmetro desconhecido: " + key);
    }
}

}  // namespace

DetectionParams makeBalancedPreset() {
    DetectionParams params;
    params.name = "balanced";
    params.dp = 1.2;
    params.minDist = 90.0;
    params.cannyThreshold = 120.0;
    params.accumulatorThreshold = 36.0;
    params.minRadius = 90;
    params.maxRadius = 240;
    params.medianKernel = 5;
    params.gaussianKernel = 7;
    params.gaussianSigma = 1.5;
    params.useClahe = false;
    return params;
}

DetectionParams makeSensitivePreset() {
    DetectionParams params;
    params.name = "sensitive";
    params.dp = 1.1;
    params.minDist = 75.0;
    params.cannyThreshold = 100.0;
    params.accumulatorThreshold = 28.0;
    params.minRadius = 80;
    params.maxRadius = 250;
    params.medianKernel = 5;
    params.gaussianKernel = 9;
    params.gaussianSigma = 1.8;
    params.useClahe = true;
    return params;
}

DetectionParams makeStrictPreset() {
    DetectionParams params;
    params.name = "strict";
    params.dp = 1.3;
    params.minDist = 100.0;
    params.cannyThreshold = 140.0;
    params.accumulatorThreshold = 42.0;
    params.minRadius = 100;
    params.maxRadius = 220;
    params.medianKernel = 5;
    params.gaussianKernel = 7;
    params.gaussianSigma = 1.2;
    params.useClahe = false;
    return params;
}

std::vector<DetectionParams> makeDefaultSweep() {
    return {makeBalancedPreset(), makeSensitivePreset(), makeStrictPreset()};
}

AppConfig parseArguments(int argc, char** argv) {
    AppConfig config;
    DetectionParams params = makeBalancedPreset();
    bool customPresetChosen = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            throw std::runtime_error(
                "Uso: ./lab02_hough_pratos [--input-dir DIR] [--output-dir DIR] "
                "[--preset balanced|sensitive|strict] [--sweep] [--clahe|--no-clahe] "
                "[--dp V] [--min-dist V] [--canny V] [--acc V] "
                "[--min-radius N] [--max-radius N] [--median N] [--gaussian N] [--sigma V]");
        }

        if (isFlag(arg, "-i", "--input-dir")) {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para " + arg);
            }
            config.inputDir = argv[++i];
        } else if (isFlag(arg, "-o", "--output-dir")) {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para " + arg);
            }
            config.outputDir = argv[++i];
        } else if (arg == "--preset") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para --preset");
            }
            params = presetByName(argv[++i]);
            customPresetChosen = true;
        } else if (arg == "--sweep") {
            config.runSweep = true;
        } else if (arg == "--clahe") {
            params.useClahe = true;
        } else if (arg == "--no-clahe") {
            params.useClahe = false;
        } else if (arg.rfind("--", 0) == 0) {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para " + arg);
            }
            applyOverride(params, arg, argv[++i]);
        } else {
            throw std::runtime_error("Argumento desconhecido: " + arg);
        }
    }

    if (config.runSweep) {
        config.experiments = makeDefaultSweep();
    } else {
        config.experiments = {params};
    }

    if (!config.runSweep && !customPresetChosen) {
        config.experiments.front().name = "balanced";
    }

    return config;
}
