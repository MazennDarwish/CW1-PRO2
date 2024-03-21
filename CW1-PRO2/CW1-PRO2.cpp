#include <iostream>
#include <fstream>
#include <random>
#include <string>


using namespace std;

class PasswordManager {
private:
    const string username = "mazen1234"; // Set username
    const string correctPassword = "mazen1234"; // Set password for the user
    const string passwordFile = "passwords.txt";

    string encryptDecrypt(const string& toEncrypt) {
        char key = 'z'; // Simple key for encryption/decryption.
        string output = toEncrypt; // Initialize output with the input string
        for (size_t i = 0; i < toEncrypt.size(); ++i) {
            output[i] = toEncrypt[i] ^ key; // XOR each byte
        }
        return output;
    }

public:
    PasswordManager() {}

    bool authenticate(const string& username, const string& password) {
        return this->username == username && this->correctPassword == password;
    }

    void addPassword(const string& password) {
        ofstream file(passwordFile, ios::app | ios::binary); // Open file in append mode.
        if (!file.is_open()) {
            cout << "Failed to open password file for writing.\n";
            return;
        }
        string encrypted = encryptDecrypt(password);
        file << encrypted << "\n";
        file.close();
    }

    void loadPasswords() {
        ifstream file(passwordFile, ios::binary);
        if (!file.is_open()) {
            cout << "No stored passwords or failed to open password file for reading.\n";
            return;
        }
        cout << "Stored passwords (encrypted):\n";
        string encryptedPassword;
        while (getline(file, encryptedPassword)) {
            cout << encryptedPassword << endl; // Display encrypted passwords as they're loaded
        }
        file.close();
    }

    void removePassword(int index) {
        ifstream file(passwordFile, ios::binary);
        ofstream tempFile("temp.txt", ios::binary);
        if (!file.is_open() || !tempFile.is_open()) {
            cout << "Failed to open files.\n";
            return;
        }
        string line;
        int currentIndex = 0;
        while (getline(file, line)) {
            currentIndex++;
            if (currentIndex != index) { // Copy all but the selected index
                tempFile << line << "\n";
            }
        }
        file.close();
        tempFile.close();
        remove(passwordFile.c_str());
        rename("temp.txt", passwordFile.c_str());
        cout << "Password removed successfully.\n";
    }

    void retrievePassword(int index) {
        ifstream file(passwordFile, ios::binary);
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

bool isValidPassword(const string& password) {
    if (password.length() <= 8) return false;
    bool hasNumber = false;
    bool hasLetter = false;
    for (char c : password) {
        if (isdigit(c)) hasNumber = true;
        if (isalpha(c)) hasLetter = true;
        if (hasNumber && hasLetter) return true;
    }
    return false;
}

string generatePassword(size_t length) {
    const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    string password;
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, characters.size() - 1);

    do {
        password.clear();
        for (size_t i = 0; i < length; ++i) {
            password += characters[distribution(generator)];
        }
    } while (!isValidPassword(password));

    return password;
}

int main() {
    PasswordManager manager;
    string username, password;

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

    bool running = true;
    while (running) {
        cout << "\n Choose an option:\n";
        cout << "1. Add a new password\n";
        cout << "2. Generate and add a new secure password\n";
        cout << "3.Remove a stored password\n";
        cout << "4. Retrieve a specific password\n";
        cout << "5. Log out\n";
        cout << "Option: ";
        int option;
        cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (option) {
        case 1: {
            cout << "Enter your password (must be more than 8 characters and contain at least one number and one letter): ";
            getline(cin, password);
            while (!isValidPassword(password)) {
                cout << "Invalid password. It must contain at least one number and letter and must be longer than 8 characters.\n";
                getline(cin, password);
            }
            manager.addPassword(password);
            cout << "Password added successfully.\n";
            break;
        }
        case 2: {
            string newPassword = generatePassword(12); // Generate a new secure password.
            manager.addPassword(newPassword);
            cout << "Generated and added new password: " << newPassword << endl;
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
            running = false; // Terminate the program.
            break;
        default:
            cout << "Invalid option. Please choose between 1 and 5.\n";
            break;
        }
    }

    return 0; // End of the program.
}
