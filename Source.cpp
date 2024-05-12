#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;
struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int turnAroundTime;
    int waitingTime;
    int remainingTime;
    int finishTime;
    int startTime;
};

void FCFS(vector<Process>& processes, int contextSwitch) {
    int currentTime = 0;

    for (int i = 0; i < processes.size(); ++i) {
        currentTime = max(processes[i].arrivalTime, currentTime);
        processes[i].startTime = currentTime;
        processes[i].finishTime = currentTime + processes[i].burstTime;
        currentTime = processes[i].finishTime + contextSwitch;
    }

    for (int i = 0; i < processes.size(); ++i) {
        processes[i].turnAroundTime = processes[i].finishTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnAroundTime - processes[i].burstTime;
    }
}

void roundRobin(vector<Process>& processes, int quantum, int contextSwitch) {
    for (int i=0; i < processes.size(); ++i) {
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].startTime = -1;
    }

    int currentTime = 0;
    bool allDone = false;

    while (!allDone) {
        allDone = true;

        for (int i = 0; i < processes.size(); ++i) {
            if (processes[i].remainingTime > 0) {
                allDone = false;

                if (processes[i].startTime == -1) {
                    processes[i].startTime = currentTime;
                }

                if (processes[i].remainingTime > quantum) {
                    currentTime += quantum;
                    processes[i].remainingTime -= quantum;
                    currentTime += contextSwitch;
                }
                else {
                    currentTime += processes[i].remainingTime;
                    processes[i].remainingTime = 0;
                    processes[i].finishTime = currentTime;
                    currentTime += contextSwitch;
                }
            }
        }
    }

    for (int i = 0; i < processes.size(); ++i) {
        processes[i].turnAroundTime = processes[i].finishTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnAroundTime - processes[i].burstTime;
    }
}

void SRT(vector<Process>& processes, int contextSwitch) {
    for (size_t i = 0; i < processes.size(); ++i) {
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].startTime = -1;
    }

    int complete = 0;
    int currentTime = 0;
    int previousProcess = -1;

    while (complete < processes.size()) {
        int shortest = -1;
        int minRemainingTime = INT_MAX;

        for (int i = 0; i < processes.size(); ++i) {
            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                processes[i].remainingTime < minRemainingTime) {
                minRemainingTime = processes[i].remainingTime;
                shortest = i;
            }
        }

        if (shortest == -1) {
            currentTime++;
            continue;
        }

        if (previousProcess != -1 && previousProcess != shortest) {
            currentTime += contextSwitch;
        }

        if (processes[shortest].startTime == -1) {
            processes[shortest].startTime = currentTime;
        }

        processes[shortest].remainingTime--;
        currentTime++;

        if (processes[shortest].remainingTime == 0) {
            complete++;
            processes[shortest].finishTime = currentTime;
            processes[shortest].waitingTime = processes[shortest].finishTime - processes[shortest].burstTime - processes[shortest].arrivalTime;
            processes[shortest].turnAroundTime = processes[shortest].waitingTime + processes[shortest].burstTime;
        }

        previousProcess = shortest;
    }
}

void readProcessData(const string& fileLocation, vector<Process>& processes, int& contextSwitch, int& quantum) {
    ifstream fin(fileLocation);
    if (!fin.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        exit(EXIT_FAILURE);
    }

    fin >> contextSwitch >> quantum;
    Process temp;
    while (fin >> temp.arrivalTime >> temp.burstTime) {
        temp.id = processes.size() + 1;
        processes.push_back(temp);
    }
    fin.close();
}

void printProcessInfo(const vector<Process>& processes, int quantum, int contextSwitch) {
    cout << "Process ID\tArrival Time\tBurst Time\tFinish Time\tTurnaround Time\tWaiting Time" << endl;
    float avgTurnaround = 0, avgWaitingTime = 0;
    cout << setfill('-') << setw(100) << "-" << endl;
    cout << setfill(' ');
    for (int i = 0; i < processes.size(); ++i) {
        cout << processes[i].id << "\t" << setw(12) << processes[i].arrivalTime << "\t" << processes[i].burstTime << "\t" << processes[i].finishTime << "\t" << setw(15) << processes[i].turnAroundTime << "\t" << setw(12) << processes[i].waitingTime << endl;
        avgTurnaround += processes[i].turnAroundTime;
        avgWaitingTime += processes[i].waitingTime;
    }
    avgTurnaround /= processes.size();
    avgWaitingTime /= processes.size();
    cout << setfill('-') << setw(100) << "-" << endl;
    cout << "Average Turnaround Time: " << avgTurnaround << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
}

int main() {
    string fileLocation = "C:\\Users\\omar.DESKTOP-4KRKGQT\\Desktop\\process.txt";
    int contextSwitch, quantum;
    vector<Process> processes;

    readProcessData(fileLocation, processes, contextSwitch, quantum);

    int choice;

    cout << "1. First Come First Serve" << endl;
    cout << "2. Round Robin" << endl;
    cout << "3. Shortest Remaining Time" << endl;
    cout << "4. Exit" << endl;
    cout << "Choose your algorithm: ";
    cin >> choice;

    switch (choice) {
    case 1:
        FCFS(processes, contextSwitch);
        printProcessInfo(processes, quantum, contextSwitch);
        break;
    case 2:
        roundRobin(processes, quantum, contextSwitch);
        printProcessInfo(processes, quantum, contextSwitch);
        break;
    case 3:
        SRT(processes, contextSwitch);
        printProcessInfo(processes, quantum, contextSwitch);
        break;
    case 4:
        cout << "Exiting..." << endl;
        break;
    default:
        cout << "Invalid choice. Please try again." << endl;
    }

    return 0;
}
