#include <iostream>   
#include <fstream>  
using namespace std;

// --- Login Credentials ---
const string ADMIN_USER = "admin";
const string ADMIN_PASS = "1234";

// ============================================================
//  ABSTRACT BASE CLASS — Member
// ============================================================

class Member 
{
private:
    string name;        
    int rollNumber;
public:
    Member(string n, int roll) : name(n), rollNumber(roll) 
    {}

    virtual ~Member() 
    {}

    string getName() const 
    { 
        return name; 
    }
    int getRollNumber() const 
    { 
        return rollNumber; 
    }

    virtual void displayDetails() const = 0;
    virtual int getType() const = 0;
};


// ============================================================
//  DERIVED CLASS 1 — RegularMember
// ============================================================
class RegularMember : public Member 
{
private:
    int attendanceCount;

public:

    RegularMember(string n, int roll, int attendance)
        : Member(n, roll), attendanceCount(attendance) 
        {}

    void displayDetails() const override 
    {
        cout << "  [Regular Member]"                              << endl;
        cout << "  Name           : " << getName()                << endl;
        cout << "  Roll Number    : " << getRollNumber()          << endl;
        cout << "  Attendance     : " << attendanceCount << " session(s)" << endl;
    }

    int getType() const override 
    { 
        return 1; 
    }

    int getAttendanceCount() const 
    { 
        return attendanceCount; 
    }
};


// ============================================================
//  DERIVED CLASS 2 — ExecutiveMember
// ============================================================
class ExecutiveMember : public Member 
{
private:
    string leadershipRole;

public:
    ExecutiveMember(string n, int roll, string role)
        : Member(n, roll), leadershipRole(role) 
        {}

    void displayDetails() const override 
    {
        cout << "  [Executive Member]"                           << endl;
        cout << "  Name           : " << getName()               << endl;
        cout << "  Roll Number    : " << getRollNumber()         << endl;
        cout << "  Leadership Role: " << leadershipRole          << endl;
    }

    int getType() const override 
    { 
        return 2; 
    }

    string getLeadershipRole() const 
    { 
        return leadershipRole; 
    }
};


const int MAX_MEMBERS = 100;
Member* clubList[MAX_MEMBERS];  
int memberCount = 0;    


// ============================================================
//  FUNCTION PROTOTYPES
// ============================================================
void registerMember();
void displayAll();
void searchByRoll();
void saveToFile();
void loadFromFile();
void printDivider();
void printHeader();


void printDivider() 
{
    cout << "  -----------------------------------------------" << endl;
}

void printHeader() 
{
    cout << endl;
    cout << "  ===============================================" << endl;
    cout << "   CAMPUS CLUB MEMBERSHIP MANAGEMENT PORTAL     " << endl;
    cout << "  ===============================================" << endl;
}

void login() 
{
    string user, pass;
    int attempts = 0;

    while (attempts < 3) 
    {
        cout << "\n  === CLUB PORTAL LOGIN ===" << endl;
        cout << "  Username : ";
        cin  >> user;
        cout << "  Password : ";
        cin  >> pass;

        if (user == ADMIN_USER && pass == ADMIN_PASS) 
        {
            cout << "\n  [+] Login successful! Welcome, " << user << ".\n";
            return;
        } 
        else 
        {
            attempts++;
            cout << "\n  [!] Wrong credentials. Attempts left: "
                 << (3 - attempts) << "\n";
        }
    }

    cout << "\n  [!] Too many failed attempts. Exiting.\n\n";
    exit(0);
}

void registerMember() 
{

    if (memberCount >= MAX_MEMBERS) 
    {
        cout << "\n  [!] Club list is full. Cannot register more members.\n";
        return;
    }

    printDivider();
    cout << "  --- Register New Member ---" << endl;

    int type;
    cout << "  Select type:  1 = Regular   2 = Executive  : ";
    cin  >> type;
    cin.ignore(); 

    string name;
    int    roll;

    cout << "  Enter Full Name    : ";
    getline(cin, name);

    cout << "  Enter Roll Number  : ";
    cin  >> roll;
    cin.ignore();

    for (int i = 0; i < memberCount; i++) 
    {
        if (clubList[i]->getRollNumber() == roll) 
        {
            cout << "\n  [!] Roll number " << roll << " is already registered.\n";
            return;
        }
    }

    if (type == 1) 
    {
       
        int attendance;
        cout << "  Enter Attendance Count : ";
        cin  >> attendance;
        cin.ignore();

        clubList[memberCount] = new RegularMember(name, roll, attendance);

    }
     else if (type == 2) 
     {
        string role;
        cout << "  Enter Leadership Role  : ";
        getline(cin, role);

        clubList[memberCount] = new ExecutiveMember(name, roll, role);

    } 
    else 
    {
        cout << "\n  [!] Invalid type. Registration cancelled.\n";
        return;
    }

    memberCount++;
    cout << "\n  [+] Member registered successfully! "
         << "(Total: " << memberCount << ")\n";
}

void displayAll() 
{
    printDivider();
    cout << "  --- All Registered Members (" << memberCount << ") ---" << endl;

    if (memberCount == 0) 
    {
        cout << "  No members registered yet." << endl;
        return;
    }

    for (int i = 0; i < memberCount; i++) 
    {
        cout << endl;
        cout << "  Record #" << (i + 1) << endl;
        clubList[i]->displayDetails(); 
        printDivider();
    }
}

void searchByRoll() 
{
    printDivider();
    cout << "  --- Search Member by Roll Number ---" << endl;

    int roll;
    cout << "  Enter Roll Number to search : ";
    cin  >> roll;
    cin.ignore();

    bool found = false;

    for (int i = 0; i < memberCount; i++) 
    {
        if (clubList[i]->getRollNumber() == roll) 
        {
            cout << "\n  [Found]" << endl;
            clubList[i]->displayDetails(); 
            found = true;
            break;
        }
    }

    if (!found) 
    {
        cout << "\n  [!] No member with roll number " << roll << " found.\n";
    }
}

void saveToFile() 
{
    ofstream outFile("members.txt");

    if (!outFile) 
    {
        cout << "\n  [!] Error: Could not open members.txt for writing.\n";
        return;
    }

    outFile << memberCount << "\n";

    for (int i = 0; i < memberCount; i++) 
    {

        int type = clubList[i]->getType(); 

        outFile << type                          << "\n";  
        outFile << clubList[i]->getName()        << "\n"; 
        outFile << clubList[i]->getRollNumber()  << "\n";

        if (type == 1) 
        {
            RegularMember* rm = (RegularMember*) clubList[i];
            outFile << rm->getAttendanceCount()  << "\n";  
        } 
        else 
        {
               ExecutiveMember* em = (ExecutiveMember*) clubList[i];
            outFile << em->getLeadershipRole()   << "\n";  
        }
    }

    outFile.close();
    cout << "\n  [+] " << memberCount << " member(s) saved to members.txt\n";
}

void loadFromFile() 
{
    ifstream inFile("members.txt");

    if (!inFile) 
    {
        cout << "\n  [!] members.txt not found. Starting with empty list.\n";
        return;
    }

    for (int i = 0; i < memberCount; i++) 
    {
        delete clubList[i];   
        clubList[i] = nullptr;
    }
    memberCount = 0;

    int total;
    inFile >> total;     
    inFile.ignore();     

    for (int i = 0; i < total && memberCount < MAX_MEMBERS; i++) 
    {

        int type;
        inFile >> type;
        inFile.ignore();     

        string name;
        getline(inFile, name);

        int roll;
        inFile >> roll;
        inFile.ignore();     

        if (type == 1) 
        {
            int attendance;
            inFile >> attendance;
            inFile.ignore(); 

            clubList[memberCount] = new RegularMember(name, roll, attendance);

        } 
        else 
        {
            string role;
            getline(inFile, role);

            clubList[memberCount] = new ExecutiveMember(name, roll, role);
        }

        memberCount++;
    }

    inFile.close();
    cout << "\n  [+] " << memberCount << " member(s) loaded from members.txt\n";
}

int main() 
{

    login() ;

    loadFromFile();

    int choice;

    while (true) 
    { 

        printHeader();
        cout << "  1. Register Member"        << endl;
        cout << "  2. Display All Members"    << endl;
        cout << "  3. Search by Roll Number"  << endl;
        cout << "  4. Save Members to File"   << endl;
        cout << "  5. Load Members from File" << endl;
        cout << "  6. Exit"                   << endl;
        printDivider();
        cout << "  Enter your choice (1-6) : ";
        cin  >> choice;
        cin.ignore();

        switch (choice) 
        {
            case 1: registerMember(); 
            break;
            case 2: displayAll();     
            break;
            case 3: searchByRoll();   
            break;
            case 4: saveToFile();     
            break;
            case 5: loadFromFile();   
            break;

            case 6:
                
                cout << "\n  Auto-saving before exit...\n";
                saveToFile();

                
                for (int i = 0; i < memberCount; i++) 
                {
                    delete clubList[i];
                    clubList[i] = nullptr;
                }

                cout << "  Goodbye! Portal closed.\n\n";
                return 0;

            default:
                cout << "\n  [!] Invalid input. Please enter a number from 1 to 6.\n";
        }
    }

    return 0;
}