// ============================================================
//   Campus Club Membership Management Portal (SFML 3.0 Upgraded GUI)
// ============================================================

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>

using namespace std;

// ============================================================
//   APPLICATION STATES
// ============================================================
enum class AppState {
    LoginScreen,
    MainDashboard
};

// State Machine for handling GUI Text Input sequentially
enum class ActionState {
    None,
    LoggingInUser,
    LoggingInPass,
    RegType,
    RegName,
    RegRoll,
    RegAtt,
    RegRole,
    SearchRoll
};

// ============================================================
//   ABSTRACT BASE CLASS — Member
// ============================================================
class Member {
private:
    string name;        
    int    rollNumber;  

public:
    Member(string n, int roll) : name(n), rollNumber(roll) {}
    virtual ~Member() {}

    string getName()       const { return name; }
    int    getRollNumber() const { return rollNumber; }

    virtual string getDetailsString() const = 0; 
    virtual int getType() const = 0;
};

// ============================================================
//   DERIVED CLASS 1 — RegularMember
// ============================================================
class RegularMember : public Member {
private:
    int attendanceCount;  

public:
    RegularMember(string n, int roll, int attendance)
        : Member(n, roll), attendanceCount(attendance) {}

    string getDetailsString() const override {
        stringstream ss;
        ss << "Type: Regular | Name: " << getName() 
           << " | Roll: " << getRollNumber() 
           << " | Attendance: " << attendanceCount << " sessions";
        return ss.str();
    }

    int getType() const override { return 1; }
    int getAttendanceCount() const { return attendanceCount; }
};

// ============================================================
//   DERIVED CLASS 2 — ExecutiveMember
// ============================================================
class ExecutiveMember : public Member {
private:
    string leadershipRole;  

public:
    ExecutiveMember(string n, int roll, string role)
        : Member(n, roll), leadershipRole(role) {}

    string getDetailsString() const override {
        stringstream ss;
        ss << "Type: Executive | Name: " << getName() 
           << " | Roll: " << getRollNumber() 
           << " | Role: " << leadershipRole;
        return ss.str();
    }

    int getType() const override { return 2; }
    string getLeadershipRole() const { return leadershipRole; }
};

// ============================================================
//   GLOBAL STORAGE & STATE
// ============================================================
const int MAX_MEMBERS = 100;
Member* clubList[MAX_MEMBERS];  
int     memberCount = 0;        
string  statusMessage = "System Ready. Please authenticate to continue.";
AppState currentState = AppState::LoginScreen;

// GUI Input Variables
ActionState currentAction = ActionState::None;
string inputBuffer = "";
string tempUsername = "";
string tempName = "";
int tempType = 0;
int tempRoll = 0;

const string ADMIN_USER = "admin";
const string ADMIN_PASS = "1234";

// ============================================================
//   STRUCT FOR GRAPHICAL BUTTONS
// ============================================================
struct GuiButton {
    sf::RectangleShape shape;
    std::optional<sf::Text> text;
    int actionId = 0;
    GuiButton() = default;

    void create(float x, float y, float width, float height, const string& label, sf::Font& font, int id) {
        actionId = id;
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(sf::Vector2f(x, y));
        shape.setFillColor(sf::Color(50, 50, 50));
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color(100, 100, 100));

        text.emplace(font);
        text->setString(label);
        text->setCharacterSize(14);
        text->setFillColor(sf::Color::White);

        sf::FloatRect textBounds = text->getLocalBounds(); 
        text->setPosition(sf::Vector2f(
          x + (width - textBounds.size.x) / 2.f,
          y + (height - textBounds.size.y) / 2.f - 4.f
        ));
    }

    void updateHover(const sf::Vector2f& mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(sf::Color(33, 150, 243)); 
        } else {
            shape.setFillColor(sf::Color(50, 50, 50));   
        }
    }

    bool isClicked(const sf::Vector2f& mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(*text);
    }
};

// ============================================================
//   CORE DATABASE LOGIC
// ============================================================
void saveToFile() {
    ofstream outFile("members.txt"); 
    if (!outFile) {
        statusMessage = "Error: Could not save data out to members.txt";
        return;
    }
    outFile << memberCount << "\n";
    for (int i = 0; i < memberCount; i++) {
        int type = clubList[i]->getType(); 
        outFile << type                              << "\n";  
        outFile << clubList[i]->getName()            << "\n";  
        outFile << clubList[i]->getRollNumber()      << "\n";  

        if (type == 1) {
            RegularMember* rm = (RegularMember*) clubList[i];
            outFile << rm->getAttendanceCount()  << "\n";  
        } else {
            ExecutiveMember* em = (ExecutiveMember*) clubList[i];
            outFile << em->getLeadershipRole()   << "\n";  
        }
    }
    outFile.close();
    statusMessage = "Saved " + to_string(memberCount) + " records to database storage file.";
}

void loadFromFile() {
    ifstream inFile("members.txt");
    if (!inFile) {
        statusMessage = "members.txt database missing. Starting with fresh empty records.";
        return;
    }
    for (int i = 0; i < memberCount; i++) {
        delete clubList[i];    
        clubList[i] = nullptr; 
    }
    memberCount = 0;

    int total;
    inFile >> total;    
    inFile.ignore();    

    for (int i = 0; i < total && memberCount < MAX_MEMBERS; i++) {
        int type;
        inFile >> type;
        inFile.ignore();    

        string name;
        getline(inFile, name);

        int roll;
        inFile >> roll;
        inFile.ignore();    

        if (type == 1) {
            int attendance;
            inFile >> attendance;
            inFile.ignore();    
            clubList[memberCount] = new RegularMember(name, roll, attendance);
        } else {
            string role;
            getline(inFile, role);
            clubList[memberCount] = new ExecutiveMember(name, roll, role);
        }
        memberCount++;
    }
    inFile.close();
}

// ============================================================
//   GUI INPUT PROCESSORS (REPLACES CONSOLE LOGIC)
// ============================================================
void processGuiInput() {
    string input = inputBuffer;
    inputBuffer = ""; 

    switch (currentAction) {
        case ActionState::LoggingInUser:
            tempUsername = input;
            currentAction = ActionState::LoggingInPass;
            statusMessage = "Enter Password: ";
            break;
            
        case ActionState::LoggingInPass:
            if (tempUsername == ADMIN_USER && input == ADMIN_PASS) {
                currentState = AppState::MainDashboard;
                statusMessage = "Access Granted! Welcome Administrator.";
            } else {
                statusMessage = "Access Denied: Invalid Username or Password.";
            }
            currentAction = ActionState::None;
            break;

        case ActionState::SearchRoll:
            try {
                int roll = stoi(input);
                bool found = false;
                for (int i = 0; i < memberCount; i++) {
                    if (clubList[i]->getRollNumber() == roll) {
                        statusMessage = "Found Match: " + clubList[i]->getDetailsString();
                        found = true;
                        break;
                    }
                }
                if (!found) statusMessage = "Search Result: No record matched Roll ID " + to_string(roll);
            } catch (...) {
                statusMessage = "Error: Invalid number format entered.";
            }
            currentAction = ActionState::None;
            break;

        case ActionState::RegType:
            try {
                tempType = stoi(input);
                if (tempType == 1 || tempType == 2) {
                    currentAction = ActionState::RegName;
                    statusMessage = "Enter Full Name: ";
                } else {
                    statusMessage = "Cancelled: Invalid type selection.";
                    currentAction = ActionState::None;
                }
            } catch (...) {
                statusMessage = "Error: Invalid number format.";
                currentAction = ActionState::None;
            }
            break;
            
        case ActionState::RegName:
            tempName = input;
            currentAction = ActionState::RegRoll;
            statusMessage = "Enter Roll Number: ";
            break;
            
        case ActionState::RegRoll:
            try {
                tempRoll = stoi(input);
                bool exists = false;
                for (int i = 0; i < memberCount; i++) {
                    if (clubList[i]->getRollNumber() == tempRoll) {
                        statusMessage = "Error: Roll number " + to_string(tempRoll) + " already exists!";
                        exists = true;
                        currentAction = ActionState::None;
                        break;
                    }
                }
                if (!exists) {
                    if (tempType == 1) {
                        currentAction = ActionState::RegAtt;
                        statusMessage = "Enter Attendance Count: ";
                    } else {
                        currentAction = ActionState::RegRole;
                        statusMessage = "Enter Leadership Role: ";
                    }
                }
            } catch (...) {
                statusMessage = "Error: Invalid number format.";
                currentAction = ActionState::None;
            }
            break;
            
        case ActionState::RegAtt:
            try {
                int att = stoi(input);
                clubList[memberCount++] = new RegularMember(tempName, tempRoll, att);
                statusMessage = "Successfully registered: " + tempName;
                saveToFile();
            } catch (...) {
                statusMessage = "Error: Invalid number format.";
            }
            currentAction = ActionState::None;
            break;
            
        case ActionState::RegRole:
            clubList[memberCount++] = new ExecutiveMember(tempName, tempRoll, input);
            statusMessage = "Successfully registered: " + tempName;
            saveToFile();
            currentAction = ActionState::None;
            break;

        default:
            currentAction = ActionState::None;
            break;
    }
}

// Trigger mechanisms to start the inputs
void guiRegisterMember() {
    if (memberCount >= MAX_MEMBERS) {
        statusMessage = "Error: Registration array limit reached!";
        return;
    }
    currentAction = ActionState::RegType;
    statusMessage = "Register: Select type (1 = Regular, 2 = Executive): ";
    inputBuffer = "";
}

void guiSearchMember() {
    currentAction = ActionState::SearchRoll;
    statusMessage = "Search: Enter Roll Number: ";
    inputBuffer = "";
}

void startLogin() {
    currentAction = ActionState::LoggingInUser;
    statusMessage = "Login: Enter Username: ";
    inputBuffer = "";
}

// ============================================================
//   MAIN ENGINE (SFML 3.0 WINDOW ENVIRONMENT)
// ============================================================
int main() {
    loadFromFile();

    // Fixed Vector2u initialization for SFML 3.0
    sf::RenderWindow window(sf::VideoMode({900, 650}), "Campus Club MemberShip Management Portal");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        cout << "Error loading font! Please place 'arial.ttf' next to the binary.\n";
        return -1;
    }

    sf::RectangleShape headerBackground(sf::Vector2f(900.f, 70.f));
    headerBackground.setFillColor(sf::Color(33, 150, 243)); 

    sf::Text headerText(font);
    headerText.setString("CAMPUS CLUB MEMBERSHIP MANAGEMENT PORTAL");
    headerText.setCharacterSize(22);
    headerText.setFillColor(sf::Color::White);
    headerText.setPosition(sf::Vector2f(20.f, 20.f));

    sf::Text loginTitleText(font);
    loginTitleText.setString("SECURE MANAGEMENT SIGN-IN");
    loginTitleText.setCharacterSize(26);
    loginTitleText.setFillColor(sf::Color::White);
    
    // Fixed Ambiguous SetPosition formatting
    loginTitleText.setPosition(sf::Vector2f(280.f, 220.f));

    GuiButton loginLaunchButton;
    loginLaunchButton.create(300.f, 310.f, 300.f, 50.f, "CLICK HERE TO LOGIN", font, 99);

    GuiButton dashboardButtons[6];
    string menuLabels[] = {
        "1. Register Member",
        "2. Clear Status logs",
        "3. Search by Roll ID",
        "4. Force Save File",
        "5. Force Reload File",
        "6. System Logout"
    };

    float startYPosition = 120.f;
    for (int i = 0; i < 6; ++i) {
        dashboardButtons[i].create(20.f, startYPosition, 280.f, 45.f, menuLabels[i], font, i + 1);
        startYPosition += 60.f;
    }

    while (window.isOpen()) {
        sf::Vector2i pixelMousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePos = window.mapPixelToCoords(pixelMousePos);

        if (currentState == AppState::LoginScreen) {
            loginLaunchButton.updateHover(worldMousePos);
        } else {
            for (int i = 0; i < 6; ++i) {
                dashboardButtons[i].updateHover(worldMousePos);
            }
        }

        // Fixed Event Handling Loop for SFML 3.0
        while (const std::optional event = window.pollEvent()) {
            
            if (event->is<sf::Event::Closed>()) {
                if (currentState == AppState::MainDashboard) saveToFile();
                window.close();
            }
            
            // --- NEW TEXT CAPTURE LOGIC ---
            else if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (currentAction != ActionState::None) {
                    uint32_t unicode = textEvent->unicode;
                    if (unicode == '\b') { // Backspace
                        if (!inputBuffer.empty()) inputBuffer.pop_back();
                    } 
                    else if (unicode == '\r' || unicode == '\n') { // Enter key
                        processGuiInput();
                    } 
                    else if (unicode >= 32 && unicode < 128) { // Standard readable chars
                        inputBuffer += static_cast<char>(unicode);
                    }
                }
            }

            else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    
                    // Prevent button clicks if an input sequence is active
                    if (currentAction != ActionState::None) continue;

                    if (currentState == AppState::LoginScreen) {
                        if (loginLaunchButton.isClicked(worldMousePos)) {
                            startLogin();
                        }
                    } 
                    else if (currentState == AppState::MainDashboard) {
                        for (int i = 0; i < 6; ++i) {
                            if (dashboardButtons[i].isClicked(worldMousePos)) {
                                switch (dashboardButtons[i].actionId) {
                                    case 1: guiRegisterMember(); break;
                                    case 2: statusMessage = "Status Logs Cleared."; break;
                                    case 3: guiSearchMember(); break;
                                    case 4: saveToFile(); break;
                                    case 5: loadFromFile(); statusMessage = "Reloaded clean layout records from disk."; break;
                                    case 6: 
                                        saveToFile(); 
                                        currentState = AppState::LoginScreen; 
                                        statusMessage = "Logged out successfully.";
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        window.draw(headerBackground);
        window.draw(headerText);

        // --- UPDATED GUI INPUT BAR RENDER LOGIC ---
        sf::Text statusDisplay(font);
        if (currentAction != ActionState::None) {
            string displayBuffer = inputBuffer;
            // Mask password natively!
            if (currentAction == ActionState::LoggingInPass) {
                displayBuffer = string(inputBuffer.length(), '*'); 
            }
            statusDisplay.setString(">> INPUT REQUIRED: " + statusMessage + displayBuffer + "_");
            statusDisplay.setFillColor(sf::Color::Cyan); // Cyan to show active typing
        } else {
            statusDisplay.setString("SYSTEM STATUS LOG:\n" + statusMessage);
            statusDisplay.setFillColor(sf::Color(255, 235, 59)); 
        }
        
        statusDisplay.setCharacterSize(14);
        statusDisplay.setPosition(sf::Vector2f(20.f, 560.f));
        window.draw(statusDisplay);

        if (currentState == AppState::LoginScreen) {
            window.draw(loginTitleText);
            loginLaunchButton.draw(window);
        } 
        else if (currentState == AppState::MainDashboard) {
            for (int i = 0; i < 6; ++i) {
                dashboardButtons[i].draw(window);
            }

            sf::Text databaseTitle(font);
            databaseTitle.setString("CURRENT ACTIVE ROSTER RECORDS (" + to_string(memberCount) + "):");
            databaseTitle.setCharacterSize(16);
            databaseTitle.setFillColor(sf::Color::White);
            databaseTitle.setPosition(sf::Vector2f(350.f, 100.f));
            window.draw(databaseTitle);

            float currentVerticalPosition = 135.f;
            if (memberCount == 0) {
                sf::Text emptyNotification(font);
                emptyNotification.setString("[Database Contains Zero Entries]");
                emptyNotification.setCharacterSize(14);
                emptyNotification.setFillColor(sf::Color(130, 130, 130));
                emptyNotification.setPosition(sf::Vector2f(350.f, currentVerticalPosition));
                window.draw(emptyNotification);
            } else {
                for (int i = 0; i < memberCount && i < 15; i++) { 
                    sf::Text listRow(font);
                    listRow.setString(to_string(i + 1) + ". " + clubList[i]->getDetailsString());
                    listRow.setCharacterSize(13);
                    listRow.setFillColor(sf::Color(180, 220, 240));
                    listRow.setPosition(sf::Vector2f(350.f, currentVerticalPosition));
                    window.draw(listRow);
                    currentVerticalPosition += 25.f;
                }
            }
        }

        window.display();
    }

    for (int i = 0; i < memberCount; i++) {
        delete clubList[i];
        clubList[i] = nullptr;
    }

    return 0;
}