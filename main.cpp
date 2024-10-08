#include <iostream>
#include <fstream>
#include <random>
#include <string>

using namespace std;

// Function to get the number of weekends available in the file
int getNumberOfWeekends(const string& filePath) {
    ifstream scheduleFile(filePath);
    if (!scheduleFile.is_open()) {
        cout << "Error: Failed to open the file to read weekends." << endl;
        return 0;
    }

    int weekendCount = 0;
    string line;
    while (getline(scheduleFile, line)) {
        if (line.find("Weekend") != string::npos) {
            weekendCount++;
        }
    }

    scheduleFile.close();
    return weekendCount;
}

// Function to display all weekend matches or specific weekend matches
void displayScheduledWeekendMatches(bool displayAll, int selectedWeekend = -1) {
    ifstream scheduleFile(R"(../updatedScheduledWeekends.csv)");
    if (!scheduleFile.is_open()) {
        cout << "Error: Failed to open the file to display the weekend games." << endl;
        return;
    }

    string line;
    int currentWeekend = 0;
    bool display = displayAll;
    cout << "_________________________________";
    cout << "\n\t\t\t\tScheduled Weekend Match: \n";
    cout << "_________________________________\n";

    // Read and display based on user selection
    while (getline(scheduleFile, line)) {
        if (line.find("Weekend") != string::npos) {
            currentWeekend++;
        }
        if (displayAll || currentWeekend == selectedWeekend) {
            cout << line << endl;
        }
    }
    cout << "_________________________________\n\n";
    scheduleFile.close();
}

int main()
{
    //Main function run here
}