#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct ReviewEntry {
    std::string imageName;
    std::string label = "pending";
    std::string notes;
};

struct AppConfig {
    std::filesystem::path imagesDir;
    std::filesystem::path csvPath;
    int windowWidth = 1400;
};

bool isFlag(const std::string& value, const std::string& shortName, const std::string& longName) {
    return value == shortName || value == longName;
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    fields.push_back(current);
    return fields;
}

std::string escapeCsv(const std::string& value) {
    if (value.find_first_of(",\"\n\r") == std::string::npos) {
        return value;
    }

    std::string escaped = "\"";
    for (char ch : value) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped.push_back(ch);
        }
    }
    escaped.push_back('"');
    return escaped;
}

AppConfig parseArguments(int argc, char** argv) {
    AppConfig config;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            throw std::runtime_error(
                "Uso: review_labels --images-dir DIR --csv FILE [--window-width N]");
        }

        if (isFlag(arg, "-i", "--images-dir")) {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para " + arg);
            }
            config.imagesDir = argv[++i];
        } else if (isFlag(arg, "-c", "--csv")) {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para " + arg);
            }
            config.csvPath = argv[++i];
        } else if (arg == "--window-width") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Faltou valor para --window-width");
            }
            config.windowWidth = std::max(800, std::stoi(argv[++i]));
        } else {
            throw std::runtime_error("Argumento desconhecido: " + arg);
        }
    }

    if (config.imagesDir.empty() || config.csvPath.empty()) {
        throw std::runtime_error("Informe --images-dir e --csv.");
    }

    return config;
}

std::vector<ReviewEntry> loadEntries(const std::filesystem::path& csvPath) {
    std::ifstream file(csvPath);
    if (!file) {
        throw std::runtime_error("Nao foi possivel abrir: " + csvPath.string());
    }

    std::vector<ReviewEntry> entries;
    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (firstLine) {
            firstLine = false;
            continue;
        }

        const std::vector<std::string> fields = splitCsvLine(line);
        if (fields.empty()) {
            continue;
        }

        ReviewEntry entry;
        entry.imageName = fields[0];
        if (fields.size() > 1 && !fields[1].empty()) {
            entry.label = fields[1];
        }
        if (fields.size() > 2) {
            entry.notes = fields[2];
            for (std::size_t i = 3; i < fields.size(); ++i) {
                entry.notes += ",";
                entry.notes += fields[i];
            }
        }

        entries.push_back(std::move(entry));
    }

    std::sort(entries.begin(), entries.end(), [](const ReviewEntry& a, const ReviewEntry& b) {
        return a.imageName < b.imageName;
    });

    return entries;
}

void saveEntries(const std::filesystem::path& csvPath, const std::vector<ReviewEntry>& entries) {
    std::ofstream file(csvPath);
    if (!file) {
        throw std::runtime_error("Nao foi possivel escrever: " + csvPath.string());
    }

    file << "image,visual_class,notes\n";
    for (const ReviewEntry& entry : entries) {
        file << escapeCsv(entry.imageName) << ','
             << escapeCsv(entry.label) << ','
             << escapeCsv(entry.notes) << '\n';
    }
}

std::map<std::string, int> summarize(const std::vector<ReviewEntry>& entries) {
    std::map<std::string, int> counts = {
        {"correct", 0},
        {"partial", 0},
        {"incorrect", 0},
        {"pending", 0},
    };

    for (const ReviewEntry& entry : entries) {
        auto it = counts.find(entry.label);
        if (it == counts.end()) {
            counts["pending"] += 1;
        } else {
            it->second += 1;
        }
    }

    return counts;
}

void writeSummary(const std::filesystem::path& csvPath, const std::vector<ReviewEntry>& entries) {
    const std::map<std::string, int> counts = summarize(entries);
    std::ofstream file(csvPath.parent_path() / "classification_summary.txt");
    if (!file) {
        return;
    }

    file << "correct=" << counts.at("correct") << '\n';
    file << "partial=" << counts.at("partial") << '\n';
    file << "incorrect=" << counts.at("incorrect") << '\n';
    file << "pending=" << counts.at("pending") << '\n';
    file << "total=" << entries.size() << '\n';
}

int findFirstPending(const std::vector<ReviewEntry>& entries) {
    for (std::size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].label != "correct" &&
            entries[i].label != "partial" &&
            entries[i].label != "incorrect") {
            return static_cast<int>(i);
        }
    }
    return 0;
}

cv::Mat renderScreen(const cv::Mat& image,
                     const ReviewEntry& entry,
                     int index,
                     int total,
                     const std::map<std::string, int>& counts,
                     int windowWidth) {
    const int sidebarWidth = 360;
    const int targetWidth = std::max(400, windowWidth - sidebarWidth);
    const double scale = std::min(1.0, static_cast<double>(targetWidth) / static_cast<double>(image.cols));
    cv::Mat resized;
    cv::resize(image,
               resized,
               cv::Size(std::max(1, static_cast<int>(image.cols * scale)),
                        std::max(1, static_cast<int>(image.rows * scale))),
               0.0,
               0.0,
               scale < 1.0 ? cv::INTER_AREA : cv::INTER_LINEAR);

    const int canvasHeight = std::max(resized.rows, 720);
    cv::Mat canvas(canvasHeight, resized.cols + sidebarWidth, CV_8UC3, cv::Scalar(30, 30, 30));
    resized.copyTo(canvas(cv::Rect(0, 0, resized.cols, resized.rows)));

    const int x = resized.cols + 20;
    int y = 40;
    const cv::Scalar white(245, 245, 245);
    const cv::Scalar gray(180, 180, 180);
    const cv::Scalar green(90, 220, 120);
    const cv::Scalar yellow(60, 220, 240);
    const cv::Scalar red(80, 80, 255);

    auto put = [&](const std::string& text, double fontScale, const cv::Scalar& color, int thickness = 1) {
        cv::putText(canvas, text, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, color, thickness, cv::LINE_AA);
        y += static_cast<int>(34 * fontScale + 16);
    };

    put("Classificacao Visual", 0.85, white, 2);
    put("Imagem: " + entry.imageName, 0.60, white);
    put("Indice: " + std::to_string(index + 1) + " / " + std::to_string(total), 0.55, gray);
    put("Label atual: " + entry.label, 0.60,
        entry.label == "correct" ? green : entry.label == "partial" ? yellow :
        entry.label == "incorrect" ? red : white, 2);

    y += 10;
    put("Totais", 0.65, white, 2);
    put("1 correct   : " + std::to_string(counts.at("correct")), 0.55, green);
    put("2 partial   : " + std::to_string(counts.at("partial")), 0.55, yellow);
    put("3 incorrect : " + std::to_string(counts.at("incorrect")), 0.55, red);
    put("0 pending   : " + std::to_string(counts.at("pending")), 0.55, gray);

    y += 10;
    put("Controles", 0.65, white, 2);
    put("[1] correct", 0.52, gray);
    put("[2] partial", 0.52, gray);
    put("[3] incorrect", 0.52, gray);
    put("[0] pending", 0.52, gray);
    put("[Right/N] avancar", 0.52, gray);
    put("[Left/P] voltar", 0.52, gray);
    put("[J] proximo pending", 0.52, gray);
    put("[S] salvar", 0.52, gray);
    put("[Q] sair", 0.52, gray);

    if (!entry.notes.empty()) {
        y += 10;
        put("Notas: " + entry.notes, 0.48, gray);
    }

    return canvas;
}

void printSummaryToStdout(const std::vector<ReviewEntry>& entries) {
    const std::map<std::string, int> counts = summarize(entries);
    std::cout << "Resumo final\n";
    std::cout << "correct=" << counts.at("correct") << '\n';
    std::cout << "partial=" << counts.at("partial") << '\n';
    std::cout << "incorrect=" << counts.at("incorrect") << '\n';
    std::cout << "pending=" << counts.at("pending") << '\n';
    std::cout << "total=" << entries.size() << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const AppConfig config = parseArguments(argc, argv);
        std::vector<ReviewEntry> entries = loadEntries(config.csvPath);
        if (entries.empty()) {
            throw std::runtime_error("Nenhuma imagem encontrada no CSV de classificacao.");
        }

        int currentIndex = findFirstPending(entries);
        const std::string windowName = "review_labels";
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

        while (true) {
            const ReviewEntry& entry = entries.at(currentIndex);
            const std::filesystem::path imagePath = config.imagesDir / entry.imageName;
            cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
            if (image.empty()) {
                throw std::runtime_error("Nao foi possivel abrir a imagem: " + imagePath.string());
            }

            const cv::Mat screen = renderScreen(
                image, entry, currentIndex, static_cast<int>(entries.size()), summarize(entries), config.windowWidth);
            cv::imshow(windowName, screen);

            const int key = cv::waitKeyEx(0);
            if (key == '1') {
                entries[currentIndex].label = "correct";
            } else if (key == '2') {
                entries[currentIndex].label = "partial";
            } else if (key == '3') {
                entries[currentIndex].label = "incorrect";
            } else if (key == '0') {
                entries[currentIndex].label = "pending";
            } else if (key == 's' || key == 'S') {
                saveEntries(config.csvPath, entries);
                writeSummary(config.csvPath, entries);
                std::cout << "[INFO] Classificacao salva em " << config.csvPath << '\n';
            } else if (key == 'q' || key == 'Q' || key == 27) {
                saveEntries(config.csvPath, entries);
                writeSummary(config.csvPath, entries);
                break;
            } else if (key == 'j' || key == 'J') {
                currentIndex = findFirstPending(entries);
                continue;
            } else if (key == 2424832 || key == 'p' || key == 'P') {
                currentIndex = (currentIndex - 1 + static_cast<int>(entries.size())) % static_cast<int>(entries.size());
                continue;
            } else if (key == 2555904 || key == 'n' || key == 'N' || key == 13 || key == 32) {
                currentIndex = (currentIndex + 1) % static_cast<int>(entries.size());
                continue;
            }

            if (key == '1' || key == '2' || key == '3' || key == '0') {
                currentIndex = (currentIndex + 1) % static_cast<int>(entries.size());
            }
        }

        cv::destroyAllWindows();
        printSummaryToStdout(entries);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << '\n';
        return 1;
    }
}
