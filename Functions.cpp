// Includes used
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> // Add this line
#include <cctype>    // Add this line
#include <map>

using namespace std;

struct Match {
    string team1;
    string team2;
};

void generateFixtures() {
    // Opening  the Teams file originally called teams_csv
    ifstream ip(R"(../Teams.csv)");

    //if the file is not found or if there is an issue reading it
    if (!ip.is_open()) {
        cout << "There is an issue with the Teams file, check to ensure that you have it" << endl;
        return;
    }

    // Declaring variables to be used
    string teamName;
    string localTown;
    string teamStadium;

    // Skip the first line as it ios the header the title
    string line;
    getline(ip, line);

    // Get the number of teams
    int noOfTeams = 0;
    while (getline(ip, line)) {
        noOfTeams++;
    }

    // Clear the flags that are raised so tht we can read later woithout any issues
    ip.clear();
    //Reset the header pointer which at this point is at th elast allowing us to start at the beginning aain when we want to read again
    ip.seekg(0, ios::beg);

    // Skip header again
    getline(ip, line);

    // Getting the games to be played by each team
    int gamesPTeam = noOfTeams - 1;

    // Total number of games in the league (round-robin, no repeat matches)
    int gamesTPlay = (gamesPTeam * noOfTeams) / 2;

    // Random number generator
    mt19937 rng(5142); // Using a fixed seed for uniform random numbers
    uniform_int_distribution<int> frm(1, noOfTeams);

    // Create stringstream to store the teams in memory
    stringstream teamsStream;
    string currentLine;
    while (getline(ip, currentLine)) {
        teamsStream << currentLine << endl;
    }
    ip.close();

    // Open the teamsGamesP file which is a file just to help in the tracking of the games played so that each team in the last list of all games will have played wihtout repetition
    ofstream teamsGamesP(R"(../teamsGamesP.csv)");
    if (!teamsGamesP.is_open()) {
        cout << "Error: Could not open the team counts file for writing." << endl;
        return;
    }

    // Open the fixturedGames file which sill have a list of all games now matched
    ofstream fixturedGames(R"(../fixturedGames.csv)");
    if (!fixturedGames.is_open()) {
        cout << "Error: Could not open the fixtured games file for writing." << endl;
        return;
    }

    // Initialize the teamsGamesP file with team names and an initial game count of 0
    stringstream teamsStreamForUpdate(teamsStream.str());
    stringstream updatedContent;
    //Loop to now initlize all games at 0 at first
    while (getline(teamsStreamForUpdate, currentLine)) {
        stringstream lineStream(currentLine);
        string name;
        getline(lineStream, name, ',');
        updatedContent << name << ",0" << endl;
    }

    //Writing the updated content to now this teamsGamesP stream...
    teamsGamesP << updatedContent.str();

    int totalGamesPlayed = 0;//Counter to track the total number of games played
    int gamesPlayedPerTeam[noOfTeams + 1][noOfTeams + 1] = {0};  // Track whether two teams have already played by using this 2dimentional array that allows to track

    // Array to store team names and their local towns
    string teamData[noOfTeams + 1][2];
    stringstream teamsStreamForTeamData(teamsStream.str());
    int teamIndex = 1;
    while (getline(teamsStreamForTeamData, currentLine)) {
        stringstream lineStream(currentLine);
        getline(lineStream, teamData[teamIndex][0], ','); // Team name
        getline(lineStream, teamData[teamIndex][1], ','); // Local town
        teamIndex++;
    }

    // Function to check if two teams are from the same town
    auto sameLocalTown = [&](int team1, int team2) {
        return teamData[team1][1] == teamData[team2][1];
    };

    // Loop until all teams have played the required number of games
    while (totalGamesPlayed < gamesTPlay) {
        int team1 = frm(rng);
        int team2 = frm(rng);

        // Ensure team1 is not the same as team2 and also that they haven't played each other before
        if (team1 != team2 && gamesPlayedPerTeam[team1][team2] == 0 && gamesPlayedPerTeam[team2][team1] == 0) {
            // Check if both teams are from different towns or if it's near the end of fixtures
            if (!sameLocalTown(team1, team2) || totalGamesPlayed >= gamesTPlay - noOfTeams / 2) {
                // Open to read the current game date of teamsGamesP
                ifstream teamsGamesPRead(R"(../teamsGamesP.csv)");
                stringstream teamsGamesPStream;//Stream to hold the contents of the file
                teamsGamesPStream << teamsGamesPRead.rdbuf();//Using the rdbuf() function to allow us read the entire contents into theteamsGamesPRead stream
                teamsGamesPRead.close();

                //Update the game count for the selected teams
                stringstream updatedContent;
                stringstream teamsGamesPStreamForUpdate(teamsGamesPStream.str());

                //Initialize variables to track whether team1 and team2 have played their required number of games
                int rowNumber = 0;
                bool team1Played = false;
                bool team2Played = false;

                // Loop through the lines of the teamsGamesP file, updating the game counts for team1 and team2
                while (getline(teamsGamesPStreamForUpdate, currentLine)) {
                    rowNumber++;//Increment the row number to ensure that it matches the teams positoon
                    stringstream lineStream(currentLine);
                    string name;
                    int gamesPlayed;

                    //Read the team name andthe number of games played from the line
                    getline(lineStream, name, ',');
                    lineStream >> gamesPlayed;

                    //Update the game count for team1 if the current row matches team1 and team1 hasn't reached its maximum games
                    if (rowNumber == team1 && gamesPlayed < gamesPTeam) {
                        gamesPlayed++;
                        team1Played = true;
                    }
                    if (rowNumber == team2 && gamesPlayed < gamesPTeam) {
                        gamesPlayed++;
                        team2Played = true;
                    }

                    updatedContent << name << "," << gamesPlayed << endl;
                }

                //Writing the updated content back to teamsGamesP
                teamsGamesP.seekp(0);
                teamsGamesP << updatedContent.str();

                //Record the fixture in fixturedGames if both teams can still play
                if (team1Played && team2Played) {
                    fixturedGames << teamData[team1][0] << " vs " << teamData[team2][0] << " " << endl;
                    totalGamesPlayed++;

                    // Mark that the two teams have now played each other
                    gamesPlayedPerTeam[team1][team2] = 1;
                    gamesPlayedPerTeam[team2][team1] = 1;
                }
            }
        }
    }

    teamsGamesP.close();
    fixturedGames.close();
}
void scheduleMatches() {
    // Opening the fixturedGames file which contains the generated fixtures
    ifstream fixtureFile(R"(../fixturedGames.csv)");
    if (!fixtureFile.is_open()) {
        cout << "Error: Could not open the fixtured games file to schedule weekend matches." << endl;
        return;
    }

    // Read the fixtures into a vector of Match structures
    vector<Match> matches;
    string match;
    while (getline(fixtureFile, match)) {
        stringstream ss(match);
        string team1, team2;
        getline(ss, team1, ' ');
        ss.ignore(3);
        getline(ss, team2,' ');
        matches.push_back({team1, team2});
    }
    fixtureFile.close();

    // Create file for the scheduled weekends matches
    ofstream scheduleFile(R"(../scheduledWeekends.csv)");
    if (!scheduleFile.is_open()) {
        cout << "Error: Failed to create the file to store the weekend games." << endl;
        return;
    }

    int weekendCount = 1;
    int matchesThisWeekend = 0;

    // First half: schedule as (team1 Home vs team2 Away)
    for (size_t i = 0; i < matches.size(); ++i) {
        Match& match = matches[i];

        if (matchesThisWeekend == 0) {
            if (weekendCount > 1) {
                scheduleFile << "\n\n";
            }
            scheduleFile << "\t\t\tWeekend " << weekendCount << ":\n";
        }

        scheduleFile << "\t" << match.team1 << " (Home) vs " << match.team2 << " (Away)\n";

        matchesThisWeekend++;
        if (matchesThisWeekend == 2) {
            weekendCount++;
            matchesThisWeekend = 0;
        }
    }

    // Check for the last weekend in the first half
    if (matchesThisWeekend < 2 && weekendCount > 1) {
        // Add matches from the second half to the previous weekend
        int secondHalfStart = matches.size() / 2; // Starting index for the second half
        for (size_t i = secondHalfStart; i < matches.size() && matchesThisWeekend < 2; ++i) {
            Match& match = matches[i];
            scheduleFile << "\t" << match.team2 << " (Home) vs " << match.team1 << " (Away)\n";
            matchesThisWeekend++;
        }
        weekendCount++;
    }

    // Second half: continue scheduling matches (team2 Home vs team1 Away)
    matchesThisWeekend = 0;
    for (size_t i = 0; i < matches.size(); ++i) {
        Match& match = matches[i];
        if (matchesThisWeekend == 0) {
            scheduleFile << "\n\n";
            scheduleFile << "\t\t\tWeekend " << weekendCount << ":\n";
        }

        scheduleFile << "\t" << match.team2 << " (Home) vs " << match.team1 << " (Away)\n";

        matchesThisWeekend++;
        if (matchesThisWeekend == 2) {
            weekendCount++;
            matchesThisWeekend = 0;
        }
    }

    scheduleFile.close();
    cout << "\nMatches scheduled successfully on weekends!" << endl;
}
void addStadiumInfo() {
    // Open the existing scheduledWeekends file
    ifstream scheduleFile(R"(../scheduledWeekends.csv)");
    if (!scheduleFile.is_open()) {
        cout << "Error: Could not open the scheduled weekends file." << endl;
        return;
    }

    // Open the Teams file to get stadium information
    ifstream teamsFile(R"(../Teams.csv)");
    if (!teamsFile.is_open()) {
        cout << "Error: Could not open the Teams file." << endl;
        return;
    }

    // Create a map to store team names and their corresponding stadiums
    map<string, string> teamStadiums;
    string line;
    getline(teamsFile, line); // Skip header
    while (getline(teamsFile, line)) {
        stringstream ss(line);
        string teamName, localTown, stadium;
        getline(ss, teamName, ',');
        getline(ss, localTown, ',');
        getline(ss, stadium, ',');
        // Trim whitespace from team name and stadium
        teamName.erase(0, teamName.find_first_not_of(" \t\n\r\f\v"));
        teamName.erase(teamName.find_last_not_of(" \t\n\r\f\v") + 1);
        stadium.erase(0, stadium.find_first_not_of(" \t\n\r\f\v"));
        stadium.erase(stadium.find_last_not_of(" \t\n\r\f\v") + 1);
        teamStadiums[teamName] = stadium;
    }
    teamsFile.close();

    // Create a new file for the updated schedule
    ofstream updatedScheduleFile(R"(../updatedScheduledWeekends.csv)");
    if (!updatedScheduleFile.is_open()) {
        cout << "Error: Could not create the updated scheduled weekends file." << endl;
        return;
    }

    // Process the scheduledWeekends file and add stadium information
    while (getline(scheduleFile, line)) {
        if (line.find("Weekend") != string::npos) {
            updatedScheduleFile << line << endl;
        } else if (!line.empty()) {
            size_t vsPos = line.find(" vs ");
            if (vsPos != string::npos) {
                string homeTeam = line.substr(1, vsPos - 7); // Extract home team name
                // Trim whitespace from home team name
                homeTeam.erase(0, homeTeam.find_first_not_of(" \t\n\r\f\v"));
                homeTeam.erase(homeTeam.find_last_not_of(" \t\n\r\f\v") + 1);
                if (teamStadiums.find(homeTeam) != teamStadiums.end()) {
                    string stadium = teamStadiums[homeTeam];
                    updatedScheduleFile << "Playing at: " << stadium << endl;
                } else {
                    cout << "Warning: Stadium not found for team: " << homeTeam << endl;
                }
                updatedScheduleFile << line << endl;
            } else {
                updatedScheduleFile << line << endl;
            }
        } else {
            updatedScheduleFile << line << endl;
        }
    }

    scheduleFile.close();
    updatedScheduleFile.close();

    cout << "Stadium information added successfully to the scheduled matches!" << endl;
}
