Campus Club Membership Management Portal
A robust, beginner-friendly C++ application built to demonstrate core Object-Oriented Programming (OOP) fundamentals and low-level memory management. This project features dual-mode deployment, providing both a secure Command Line Interface (CLI) with a login gateway and a fully interactive Graphical User Interface (GUI).

🎯 Project Milestone: This exact implementation achieved a perfect 30/30 score in the final academic OOP viva defense for its clean architecture and rigorous manual memory management.

🚀 Features
Dual-Interface Deployment: Access the system via a buffer-safe Console application or an interactive GUI layout.

Secure Console Login Gate: Protected by a 3-attempt authentication gateway using constant credentials.

Strict Encapsulation: Zero raw data exposure; all member details are modified and accessed safely through secure class interfaces.

Custom File Serialization: High-performance, line-by-line file persistence targeting members.txt without relying on heavy external parsing libraries.

Manual Heap Tracking: Formulated entirely without std::vector to cleanly show explicit low-level object creation and tracking mechanics.

🛠️ Core OOP Architecture
The project systematically covers all four primary pillars of Object-Oriented Programming:

Pillar	Implementation in This Project
Abstraction : Built an abstract base class Member utilizing pure virtual functions (displayDetails() = 0), creating a mandatory runtime blueprint without allowing direct class instantiation.
Inheritance :	Derived specialized domains (RegularMember and ExecutiveMember) from the base Member class, extending class fields seamlessly while reusing foundational attributes.
Encapsulation	: Declared identifiers like name and rollNumber under strict private access modifiers, strictly exposing them using public constant getters (getName() const).
Polymorphism : Implemented dynamic binding via an array of base class pointers (Member* clubList[]). The program queries the class Virtual Table (V-Table) at runtime to execute type-specific logic cleanly with a single call.

💾 Technical Design Specs
1. Low-Level Memory Management
Instead of abstracted vectors, storage utilizes a fixed-size raw pointer array:

const int MAX_MEMBERS = 100;
Member* clubList[MAX_MEMBERS];
This forces explicit memory tracking. Dynamic objects are instantiated via new on the heap and explicitly deallocated using delete combined with a nullptr assignment to guarantee zero memory leaks and avoid dangling pointers.

delete clubList[i];
clubList[i] = nullptr;

2. Smart File I/O Serialization
The system outputs data using a highly predictable line-by-line sequence. It bypasses string-splitting delimiters by using a leading identification type token (1 for Regular, 2 for Executive)

[Total Records Registered]
[Member Type Token: 1 or 2]
[Full Name String]
[Unique Roll Number Integer]
[Type-Specific Attribute Field]

3. Buffer-Safe Streams
Includes standard stream cleanups (cin.ignore()) directly after reading raw integers to completely remove leftover newline characters (\n) from the input stream before executing string reads via getline().

🔑 Access Credentials
The integrated console gateway relies on the following default configurations:

Username: admin
Password: 1234

💻 How to Build and Run
Prerequisites
Ensure you have an updated C++ compiler installed (GCC/g++ or Clang).

Step-by-Step Compilation

1. Clone the repository:
git clone https://github.com/YOUR_USERNAME/campus-club-portal.git
cd campus-club-portal

2. Compile the Console Application:
g++ -std=c++11 main.cpp -o ClubPortal

3. Execute the binary:
./ClubPortal
