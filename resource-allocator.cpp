#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

// Structure for project information
struct Project {
    std::string name;
    std::string domain;
    std::string startDate;
    std::string endDate;
    int commission;
    std::string companyName;
    std::string contactPerson;
    std::string contactPersonPhone;
};

// Structure for resource information
struct Resource {
    std::string firstName;
    std::string lastName;
    std::string speciality;
    std::string availability;
    std::string phone;
    std::string email;
    std::string location;
};

// Function to trim leading and trailing whitespace from a string
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return s;
    }
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}

// Function to convert a string to lowercase. Needed for specialty - domain comparison
std::string toLower(const std::string& s) {
    std::string lower_s = s;
    std::transform(lower_s.begin(), lower_s.end(), lower_s.begin(), ::tolower);
    return lower_s;
}

// Function to parse JSON data
std::vector<Project> parseProjects(const std::string& jsonFile) {
    std::vector<Project> projects;
    std::ifstream inputFile(jsonFile);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening JSON file: " << jsonFile << std::endl;
        return projects;
    }

    std::string line;
    bool inObject = false;
    Project currentProject;

    while (std::getline(inputFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.find('{') != std::string::npos) {
            inObject = true;
            currentProject = Project();
        } else if (line.find('}') != std::string::npos && inObject) {
            projects.push_back(currentProject);
            inObject = false;
        } else if (inObject) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(1, colonPos - 2));
                std::string value;
                size_t valueStart = line.find('"', colonPos + 1);
                if (valueStart != std::string::npos) {
                    size_t valueEnd = line.find('"', valueStart + 1);
                    if (valueEnd != std::string::npos) {
                        value = trim(line.substr(valueStart + 1, valueEnd - valueStart - 1));
                    }
                } else {
                    valueStart = line.find(' ', colonPos + 1);
                    if (valueStart != std::string::npos) {
                        size_t commaPos = line.find(',', valueStart + 1);
                        if (commaPos != std::string::npos) {
                            value = trim(line.substr(valueStart + 1, commaPos - valueStart - 1));
                        } else {
                            value = trim(line.substr(valueStart + 1));
                        }
                    }
                }
                if (key == "Project Name") currentProject.name = value;
                else if (key == "Domain") currentProject.domain = value;
                else if (key == "Start Date") currentProject.startDate = value;
                else if (key == "End Date") currentProject.endDate = value;
                else if (key == "Commission") currentProject.commission = std::stoi(value);
                else if (key == "Company Name") currentProject.companyName = value;
                else if (key == "Contact Person") currentProject.contactPerson = value;
                else if (key == "Contact Person Phone") currentProject.contactPersonPhone = value;
            }
        }
    }
    inputFile.close();
    return projects;
}

// Function to parse CSV data
std::vector<Resource> parseResources(const std::string& csvFile) {
    std::vector<Resource> resources;
    std::ifstream inputFile(csvFile);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening CSV file: " << csvFile << std::endl;
        return resources;
    }

    std::string line;
    std::getline(inputFile, line); // Read CSV row

    while (std::getline(inputFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> row;
        while (std::getline(ss, segment, ',')) { // Changed delimiter to ',' due to comma-separated file
            row.push_back(trim(segment));
        }

        if (row.size() == 7) {
            resources.push_back({row[0], row[1], row[2], row[3], row[4], row[5], row[6]});
        } else if (!line.empty()) {
            std::cerr << "Warning: Incorrect number of columns in row (" << row.size() << "). Line: \"" << line << "\"" << std::endl;
        }
    }
    inputFile.close();
    return resources;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <projects.json> <resources.csv>" << std::endl;
        return 1;
    }

    std::string jsonFile = argv[1];
    std::string csvFile = argv[2];

    std::vector<Project> projects = parseProjects(jsonFile);
    std::vector<Resource> resources = parseResources(csvFile);

    if (projects.empty()) {
        std::cout << "No projects found." << std::endl;
        return 0;
    }

    std::cout << "Available Projects:" << std::endl;
    for (size_t i = 0; i < projects.size(); ++i) {
        std::cout << i + 1 << ". " << projects[i].name << " (" << projects[i].domain << ")" << std::endl;
    }

    int choice;
    std::cout << "Enter the number of the project to allocate resources for (or 0 to exit): ";
    std::cin >> choice;

    if (choice > 0 && choice <= projects.size()) {
        Project selectedProject = projects[choice - 1];
        std::cout << "\nAllocating resources for: " << selectedProject.name << " (Domain: " << selectedProject.domain << ")" << std::endl;

        bool resourceFound = false;
        for (const auto& resource : resources) {
            if (toLower(resource.speciality) == toLower(selectedProject.domain)) {
                std::cout << "\nMatching Resource Found:" << std::endl;
                std::cout << "  First Name: " << resource.firstName << std::endl;
                std::cout << "  Last Name: " << resource.lastName << std::endl;
                std::cout << "  Speciality: " << resource.speciality << std::endl;
                std::cout << "  Availability: " << resource.availability << std::endl;
                std::cout << "  Phone: " << resource.phone << std::endl;
                std::cout << "  Email: " << resource.email << std::endl;
                std::cout << "  Location: " << resource.location << std::endl;
                resourceFound = true;
            }
        }

        if (!resourceFound) {
            std::cout << "No resources found with the speciality: " << selectedProject.domain << std::endl;
        }

    } else if (choice != 0) {
        std::cout << "Invalid choice." << std::endl;
    }

    return 0;
}