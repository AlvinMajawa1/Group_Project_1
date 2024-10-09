#include <iostream>
#include <fstream>
#include <random>
#include <string>

#include "functions.h"

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

int main() {
    generateFixtures();
    scheduleMatches();
    addStadiumInfo();
    string filePath = "../updatedScheduledWeekends.csv";

    int totalWeekends = getNumberOfWeekends(filePath);

    if (totalWeekends == 0) {
        cout << "No weekends available to display." << endl;
        return 1;
    }

    while (true) {
        cout << "\nWould you like to see all weekend matches or choose a specific weekend? \n(Enter 'a or A' For 'All' and 's or S' for 'Specific'): ";
        string userChoice;
        cin >> userChoice;

        if (userChoice == "A" || userChoice == "a" ) {
            displayScheduledWeekendMatches(true);
        } else if (userChoice == "S" || userChoice == "s" ) {
            cout << "\nThere are " << totalWeekends << " weekends available. ";
            cout << "Please enter a weekend number between 1 and " << totalWeekends << ".\nInput: ";
            int weekendNumber;
            cin >> weekendNumber;

            if (weekendNumber >= 1 && weekendNumber <= totalWeekends) {
                displayScheduledWeekendMatches(false, weekendNumber);
            } else {
                cout << "Invalid weekend number. Please try again.";
                continue;
            }
        }
        else {
            cout << "Invalid choice. Please enter 'a or A' For 'All' and 's or S' for 'Specific'.";
            continue;
        }

        cout << "\nWould you like to pick another weekend, see all weekends, or end the program? \n(Please enter any number or letter to conntinue or Enter 'end - To close Program')\nInput: ";
        string nextAction;
        cin >> nextAction;
        cout << "\n___________________________________________________________________________________________________";

        if (nextAction == "end") {
            cout << "\n\t\tExiting the program. Thank you for accessing our system, Goodbye!\n";
            cout << "___________________________________________________________________________________________________";

            break;
        }
    }

    return 0;
}
