#include <iostream>
#include <fstream>
#include <random>
#include <string>

using namespace std;

class PasswordManager {
private:
    string userFile = "user.txt"; // File to store user credentials
    string passwordFile; // File to store passwords for each specific user
    char key = 'z'; // Simple key for encryption/decryption using the XOR cipher

    // Encrypts or decrypts the input using the XOR cipher
    string encryptDecrypt(const string& toEncrypt) {
        string output = toEncrypt;
        for (size_t i = 0; i < toEncrypt.size(); ++i) {
            output[i] = toEncrypt[i] ^ key;
        }
        return output;
    }

public:
    PasswordManager() {}

    // Signs up a new user, returns true if successful
    bool signUp(const string& username, const string& password) {
        ifstream userFileIn(userFile, ios::binary); //Open user file for reading
        if (!userFileIn.is_open()) {
            cout << "Failed to open user file for reading.\n";
            return false;
        }

        // Checks if the username already exists
        string storedUsername, storedPassword;
        while (getline(userFileIn, storedUsername) && getline(userFileIn, storedPassword)) {
            storedUsername = encryptDecrypt(storedUsername);
            if (username == storedUsername) {
                cout << "User already exists.\n";
                userFileIn.close();
                return false;
            }
        }
        userFileIn.close();

        // Adds a new user to the file
        ofstream userFileOut(userFile, ios::app | ios::binary);
        if (!userFileOut.is_open()) {
            cout << "Failed to open user file for writing.\n";
            return false;
        }
        userFileOut << encryptDecrypt(username) << "\n" << encryptDecrypt(password) << "\n";
        userFileOut.close();
        return true;
    }

    // Authenticates a user, return true if successful
    bool authenticate(const string& username, const string& password) {
        ifstream userFileIn(userFile, ios::binary);
        if (!userFileIn.is_open()) {
            cout << "User file not found. Please sign up first.\n";
            return false;
        }

        // Verifies the username and password
        string storedUsername, storedPassword;
        bool isAuthenticated = false;
        while (getline(userFileIn, storedUsername) && getline(userFileIn, storedPassword)) {
            storedUsername = encryptDecrypt(storedUsername);
            storedPassword = encryptDecrypt(storedPassword);
            if (username == storedUsername && password == storedPassword) {
                isAuthenticated = true;
                break;
            }
        }
        userFileIn.close();

        // Sets the file name for the authenticated user
        if (isAuthenticated) {
            passwordFile = username + "_passwords.txt";
        }
        return isAuthenticated;
    }

    // Adds a new password for the user
    void addPassword(const string& password) {
        ofstream file(passwordFile, ios::app | ios::binary);
        if (!file.is_open()) {
            cout << "Failed to open password file for writing.\n";
            return;
        }
        string encrypted = encryptDecrypt(password);
        file << encrypted << "\n";
        file.close();
    }

    // Loads and displays all stored passwords for the user
    void loadPasswords() {
        ifstream file(passwordFile, ios::binary);
        if (!file.is_open()) {
            cout << "No stored passwords or failed to open password file for reading.\n";
            return;
        }
        cout << "Stored passwords (encrypted):\n";
        string encryptedPassword;
        while (getline(file, encryptedPassword)) {
            cout << encryptedPassword << endl; // Displays the encrypted passwords
        }
        file.close();
    }

    // Removes a password chosen by the user at a given index
    void removePassword(int index) {
        ifstream file(passwordFile, ios::binary);
        ofstream tempFile("temp.txt", ios::binary);
        if (!file.is_open() || !tempFile.is_open()) {
            cout << "Failed to open files.\n";
            return;
        }

        // Copy all passwords except the one to remove to a temporary file
        string line;
        int currentIndex = 0;
        while (getline(file, line)) {
            currentIndex++;
            if (currentIndex != index) {
                tempFile << line << "\n";
            }
        }

        // Replace the original file with the updated one to contain all non-deleted passwords
        file.close();
        tempFile.close();
        remove(passwordFile.c_str
        ());
        rename("temp.txt", passwordFile.c_str());
        cout << "Password removed successfully.\n";
    }

    // Retrieves and displays a password at a given index
    void retrievePassword(int index) {
        ifstream file(passwordFile, ios::binary); // Open the password file for reading
        if (!file.is_open()) {
            cout << "Failed to open password file for reading.\n";
            return;
        }
        int currentIndex = 0;
        string encryptedPassword;
        while (getline(file, encryptedPassword)) {
            currentIndex++;
            if (currentIndex == index) {
                cout << "Password " << index << ": " << encryptDecrypt(encryptedPassword) << endl;
                file.close();
                return;
            }
        }
        file.close();
        cout << "No password found at index " << index << ".\n";
    }
};

// Validates the password strength and features
bool isValidPassword(const string& password) {
    if (password.length() <= 8) return false; // Length check
    bool hasNumber = false;
    bool hasLetter = false;
    for (char c : password) {
        if (isdigit(c)) hasNumber = true; // Checks for at least one number
        if (isalpha(c)) hasLetter = true; // Checks for at least one letter
        if (hasNumber && hasLetter) return true; // Must contain at least one of both
    }
    return false; // If requirements are not met
}

// Generates a random password meeting the criteria of the password validation
string generatePassword(size_t length) {
    const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    string password;
    random_device rd;
    mt19937 generator(rd()); // Random number generator known as Mersenne Twister
    uniform_int_distribution<> distribution(0, characters.size() - 1);
    do {
        password.clear();
        for (size_t i = 0; i < length; ++i) {
            password += characters[distribution(generator)]; // Generates random characters
        }
    } while (!isValidPassword(password)); // Ensures that the generated password is valid

    return password;
}

int main() {
    PasswordManager manager;
    string username, password;
    int choice;

    cout << "Do you want to (1) Sign up or (2) Log in? Enter 1 or 2 (Enter 0 to exit): ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 0) {
        cout << "Exiting program.\n";
        return 0; // Exit the program
    }
    else if (choice == 1) {
        cout << "Choose a username: ";
        getline(cin, username);
        cout << "Choose a password: ";
        getline(cin, password);
        if (manager.signUp(username, password)) {
            cout << "Sign-up successful!\n";
        }
        else {
            cout << "Please try signing up again with a different username.\n";
            return 1; // Exiting for simplicity
        }
    }

    // Asking for login after sign up or if the user chose to log in directly
    cout << "Please log in.\n";
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    if (!manager.authenticate(username, password)) {
        cout << "Login failed. Exiting program.\n";
        return 1;
    }

    cout << "Login successful!\n";
    manager.loadPasswords();

    // Password management logic
    bool running = true;
    while (running) {
        cout << "\nChoose an option:\n";
        cout << "1. Add a new password\n";
        cout << "2. Generate and add a new secure password\n";
        cout << "3. Remove a stored password\n";
        cout << "4. Retrieve a specific password\n";
        cout << "5. Log out\n";
        cout << "Option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            cout << "Enter your password (must be more than 8 characters and contain at least one number and one letter): ";
            getline(cin, password);
            if (!isValidPassword(password)) {
                cout << "Invalid password. It must contain at least one number and letter and must be longer than 8 characters.\n";
            }
            else {
                manager.addPassword(password);
                cout << "Password added successfully.\n";
            }
            break;
        }
        case 2: {
            string newPassword = generatePassword(12); // Generate a new secure password
            manager.addPassword(newPassword);
            cout << "Generated and added new password:" << newPassword << endl;
            break;
        }
        case 3: {
            cout << "Enter the index of the password you want to remove: ";
            int index;
            cin >> index;
            manager.removePassword(index);
            break;
        }
        case 4: {
            cout << "Enter the index of the password you want to retrieve: ";
            int index;
            cin >> index;
            manager.retrievePassword(index);
            break;
        }
        case 5:
            cout << "You are now logged out. Goodbye.\n";
            running = false; // Terminate the program
            break;
        default:
            cout << "Invalid option. Please choose between 1 and 5.\n";
            break;
        }
    }

    return 0; // End of the program
}

