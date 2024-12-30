#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <random>
#include <filesystem>

// Объявляем std::filesystem как fs
namespace fs = std::filesystem;

class FileProcessor {
private:
    std::shared_ptr<spdlog::logger> logger;

public:
    FileProcessor() {
        logger = spdlog::get("file_processor_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("file_processor_logger");
        }
        spdlog::set_level(spdlog::level::info);
    }

    // Method to create a directory
    void createDirectory(const std::string& path) {
        if (!fs::exists(path)) {
            fs::create_directory(path);
            logger->info("Directory created: {}", path);
        }
        else {
            logger->info("Directory already exists: {}", path);
        }
    }

    // Method to create and fill file with random integers
    void createAndFillFile(const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            logger->error("Failed to create file: {}", filePath);
            throw std::runtime_error("Failed to create file.");
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 1000);

        for (int i = 0; i < 100; ++i) {
            int randomNumber = distrib(gen);
            file << randomNumber << "\n";
            logger->info("Written number: {}", randomNumber);
        }

        file.close();
        logger->info("File created and filled with random numbers: {}", filePath);
    }

    // Method to sort the contents of a file
    void sortFileContents(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", filePath);
            throw std::runtime_error("Failed to open file.");
        }

        std::vector<int> numbers;
        int number;
        while (file >> number) {
            numbers.push_back(number);
        }
        file.close();

        logger->info("Read numbers from file.");
        std::sort(numbers.begin(), numbers.end());
        logger->info("Sorted the numbers.");

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            logger->error("Failed to open file for writing: {}", filePath);
            throw std::runtime_error("Failed to open file for writing.");
        }

        for (int num : numbers) {
            outFile << num << "\n";
            logger->info("Written sorted number: {}", num);
        }

        outFile.close();
        logger->info("Sorted numbers written back to file: {}", filePath);
    }
};

// Test class
class FileProcessorTest : public ::testing::Test {
protected:
    FileProcessor* processor;
    const std::string dirPath = "./output";
    const std::string filePath = dirPath + "/test_numbers.txt";

    void SetUp() override {
        processor = new FileProcessor();
        processor->createDirectory(dirPath);
        processor->createAndFillFile(filePath);
    }

    void TearDown() override {
        delete processor;
        processor = nullptr;

        if (fs::exists(filePath)) {
            fs::remove(filePath);
        }
        if (fs::exists(dirPath)) {
            fs::remove_all(dirPath);
        }
    }
};

// Test creating and filling the file
TEST_F(FileProcessorTest, CreateAndFillFileTest) {
    ASSERT_TRUE(fs::exists(filePath)) << "File should exist after creation.";
    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "File should open successfully.";
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        ++count;
    }
    ASSERT_EQ(count, 100) << "File should contain 100 lines.";
}

// Test sorting the file contents
TEST_F(FileProcessorTest, SortFileContentsTest) {
    processor->sortFileContents(filePath);

    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "File should open successfully.";
    std::vector<int> numbers;
    int number;
    while (file >> number) {
        numbers.push_back(number);
    }
    file.close();

    ASSERT_TRUE(std::is_sorted(numbers.begin(), numbers.end())) << "Numbers should be sorted.";
}

// Additional test cases
TEST_F(FileProcessorTest, DirectoryCreationTest) {
    const std::string newDirPath = "./new_output";
    processor->createDirectory(newDirPath);
    ASSERT_TRUE(fs::exists(newDirPath)) << "Directory should be created.";
    fs::remove_all(newDirPath);
}

TEST_F(FileProcessorTest, FileNotFoundTest) {
    const std::string nonExistentFile = "./non_existent_file.txt";
    EXPECT_THROW({
        processor->sortFileContents(nonExistentFile);
        }, std::runtime_error);
}

TEST_F(FileProcessorTest, CreateEmptyFileTest) {
    const std::string emptyFilePath = dirPath + "/empty_file.txt";
    std::ofstream emptyFile(emptyFilePath);
    emptyFile.close();
    ASSERT_TRUE(fs::exists(emptyFilePath)) << "Empty file should be created.";
    fs::remove(emptyFilePath);
}

TEST_F(FileProcessorTest, EmptyFileSortingTest) {
    const std::string emptyFilePath = dirPath + "/empty_file.txt";
    std::ofstream emptyFile(emptyFilePath);
    emptyFile.close();
    ASSERT_NO_THROW({
        processor->sortFileContents(emptyFilePath);
        });
    fs::remove(emptyFilePath);
}

TEST_F(FileProcessorTest, CorrectNumberOfLinesTest) {
    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "File should open successfully.";
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        ++count;
    }
    ASSERT_EQ(count, 100) << "File should contain 100 lines.";
}

TEST_F(FileProcessorTest, SortingOrderTest) {
    processor->sortFileContents(filePath);

    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "File should open successfully.";
    std::vector<int> numbers;
    int number;
    while (file >> number) {
        numbers.push_back(number);
    }
    file.close();

    ASSERT_TRUE(std::is_sorted(numbers.begin(), numbers.end())) << "Numbers should be sorted.";
}

TEST_F(FileProcessorTest, FileContentAfterSortingTest) {
    processor->sortFileContents(filePath);

    std::ifstream file(filePath);
    ASSERT_TRUE(file.is_open()) << "File should open successfully.";
    std::vector<int> numbers;
    int number;
    while (file >> number) {
        numbers.push_back(number);
    }
    file.close();

    int prev = numbers[0];
    for (size_t i = 1; i < numbers.size(); ++i) {
        ASSERT_LE(prev, numbers[i]) << "Previous number should be less than or equal to current number.";
        prev = numbers[i];
    }
}

TEST_F(FileProcessorTest, RecreateFileAfterRemovalTest) {
    if (fs::exists(filePath)) {
        fs::remove(filePath);
    }
    ASSERT_FALSE(fs::exists(filePath)) << "File should be removed.";

    processor->createAndFillFile(filePath);
    ASSERT_TRUE(fs::exists(filePath)) << "File should be recreated.";
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int testResult = RUN_ALL_TESTS();

    FileProcessor processor;
    const std::string dirPath = "./output";
    const std::string filePath = dirPath + "/random_numbers.txt";

    try {
        processor.createDirectory(dirPath);
        processor.createAndFillFile(filePath);
        processor.sortFileContents(filePath);
    }
    catch (const std::exception& ex) {
        spdlog::get("file_processor_logger")->error("Exception caught: {}", ex.what());
    }

    return testResult;
}
