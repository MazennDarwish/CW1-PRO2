#include <iostream>
#include <fstream>
#include <random>
#include <string>


using namespace std;

// Hash Map implementation with string keys and vector <string> values.
struct HashNode {
    string key;             // Key of the hash node.
    vector<string> values;  // Associated values stored in a vector.
    HashNode* nextNode;     // Pointer to the next node for collision resolution.

    // Constructor initializes a node with a key and values.
    HashNode(string key, vector<string> values) : key(key), values(values), nextNode(nullptr) {}
};

class StringHashMap {
private:
    static const int HASH_TABLE_SIZE = 10; // Small number for simplicity.
    HashNode* hashTable[HASH_TABLE_SIZE] = {}; // Array of pointer to HashNodes.

    // Hash function to convert a string key to a bucket index.
    unsigned long calculateHash(const string& key) {
        unsigned long hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return hash % HASH_TABLE_SIZE;
    }

public:
    StringHashMap() {}

    // Adds or updates a value in the map.
    void insertOrUpdate(const string& key, const vector<string>& values);

    // Retrieves a vector of values by key. Returns nullptr if not found.
    vector<string>* findValuesByKey(const string& key);

    // Removes a key-value pair by key.
    void removeByKey(const string& key);

    // Destructor to clean up dynamically allocated memory.
    ~StringHashMap();
};

// Encrypts or decrypts a string using XOR operation with a simple key.
string encryptDecrypt(const string& toEncrypt) {
    char key = 'z'; // Arbitrary choice of key for encryption/decryption.
    string output = toEncrypt;

    for (size_t i = 0; i < toEncrypt.size(); i++) {
        output[i] = toEncrypt[i] ^ key; // XOR operation for encryption/decryption.
    }

    return output;
}

// Checks if a password is valid based on length and requirements.
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

// Generates a secure random password of a specified length.
string generatePassword(size_t length) {
    // Characters allowed in the password.
    const string characters =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()";
    string password;
    random_device randomDevice;
    mt19937 rng(randomDevice());
    uniform_int_distribution<> dist(0, characters.size() - 1);

    // Keep generating until a valid password is created.
    do {
        password.clear();
        for (size_t i = 0; i < length; ++i) {
            password += characters[dist(rng)];
        }
    } while (!isValidPassword(password));

    return password;
}

class PasswordManager {
private:
    StringHashMap userPasswordsMap; // Map to store user passwords.

    // Constructs the file name for storing a user's passwords.
    string constructFileName(const string& username) {
        return username + "_passwords.txt";
    }

public:
    // Loads user passwords from a file into the hash map.
    void loadUserPasswords(const string& currentUser);

    // Simple authentication mechanism.
    bool authenticate(const string& username, const string& password);

    // Adds a new password for a user.
    void addNewPassword(const string& username, const string& password);

    // Deletes a user's password by index.
    void deleteUserPassword(const string& username, int index);

    // Retrieves all passwords for a user.
    vector<string> getUserPasswords(const string& username);

    // Retrieves a user's password by index.
    string getUserPasswordByIndex(const string& username, int index);

private:
    // Saves an encrypted password to a file.
    void savePasswordToFile(const string& username, const string& encryptedPassword);
};
void StringHashMap::insertOrUpdate(const string& key, const vector<string>& values) {
    unsigned long index = calculateHash(key); // Determine the hash table index.
    HashNode** node = &hashTable[index]; // Pointer to the head node at the hashed index.

    // Search for an existing node with the same key to update it.
    while (*node != nullptr) {
        if ((*node)->key == key) {
            (*node)->values = values; // Key found, update values.
            return;
        }
        node = &((*node)->nextNode); // Move to the next node in the chain.
    }

    *node = new HashNode(key, values); // Key not found, insert new node.
}

vector<string>* StringHashMap::findValuesByKey(const string& key) {
    unsigned long index = calculateHash(key); // Determine the hash table index.
    HashNode* node = hashTable[index]; // Pointer to the head node at the hashed index.

    // Search through the chain for the key.
    while (node != nullptr) {
        if (node->key == key) {
            return &node->values; // Key found, return pointer to values.
        }
        node = node->nextNode; // Move to the next node in the chain.
    }

    return nullptr; // Key not found.
}

void StringHashMap::removeByKey(const string& key) {
    unsigned long index = calculateHash(key); // Determine the hash table index.
    HashNode** node = &hashTable[index]; // Pointer to the head node at the hashed index.

    // Search for the node with the key to delete it.
    while (*node != nullptr) {
        if ((*node)->key == key) {
            HashNode* nodeToDelete = *node; // Node to delete found.
            *node = (*node)->nextNode; // Remove node from chain.
            delete nodeToDelete; // Free memory.
            return;
        }
        node = &((*node)->nextNode); // Move to the next node in the chain.
    }
}

StringHashMap::~StringHashMap() {

    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        HashNode* node = hashTable[i];
        while (node != nullptr) {
            HashNode* next = node->nextNode;
            delete node; // Free memory for each node.
            node = next;
        }
    }
}
void PasswordManager::loadUserPasswords(const string& currentUser) {
    ifstream file(constructFileName(currentUser), ios::binary); // Open file for reading.
    if (!file) return; // Exit if file does not exist.

    vector<string> userPasswords;
    string encryptedPassword;
    while (getline(file, encryptedPassword)) {
        // Decrypt each password and add to vector.
        userPasswords.push_back(encryptDecrypt(encryptedPassword));
    }
    file.close();
    userPasswordsMap.insertOrUpdate(currentUser, userPasswords); // Update hash map with loaded passwords.
}

bool PasswordManager::authenticate(const string& username, const string& password) {
    // Checks if username and password match predefined values.
    return (username == "mazen1234" && password == "mazen1234") || (username == "mezo9304" && password == "mezo9304");
}

void PasswordManager::addNewPassword(const string& username, const string& password) {
    vector<string>* userPasswords = userPasswordsMap.findValuesByKey(username);
    if (!userPasswords) {
        // If the user is not found, create a new vector and add the password.
        vector<string> newPasswords{ password };
        userPasswordsMap.insertOrUpdate(username, newPasswords);
    }
    else {
        // If the user is found, add the new password to their vector.
        userPasswords->push_back(password);
        userPasswordsMap.insertOrUpdate(username, *userPasswords);
    }
    // Save the new password list to the file.
    savePasswordToFile(username, encryptDecrypt(password)); // Encrypt before saving.
}

void PasswordManager::deleteUserPassword(const string& username, int index) {
    vector<string>* userPasswords = userPasswordsMap.findValuesByKey(username);
    if (userPasswords && index >= 0 && index < userPasswords->size()) {
        // If valid index, remove the password from the vector.
        userPasswords->erase(userPasswords->begin() + index);
        userPasswordsMap.insertOrUpdate(username, *userPasswords);

        // Rewrite the updated password list to the file.
        ofstream file(constructFileName(username), ios::trunc | ios::binary);
        if (!file) return; // Exit if file open fails.
        for (const string& pass : *userPasswords) {
            file << encryptDecrypt(pass) << "\n"; // Encrypt each password before saving.
        }
        file.close();
    }
}

vector<string> PasswordManager::getUserPasswords(const string& username) {
    vector<string>* userPasswords = userPasswordsMap.findValuesByKey(username);
    if (userPasswords) {
        return *userPasswords; // Return a copy of the user's passwords.
    }
    return {}; // Return empty vector if user not found.
}

string PasswordManager::getUserPasswordByIndex(const string& username, int index) {
    vector<string>* userPasswords = userPasswordsMap.findValuesByKey(username);
    if (userPasswords && index >= 0 && index < userPasswords->size()) {
        return userPasswords->at(index); // Return password at specified index.
    }
    return ""; // Return empty string if index is invalid.
}

void PasswordManager::savePasswordToFile(const string& username, const string& encryptedPassword) {
    ofstream file(constructFileName(username), ios::app | ios::binary); // Open file for appending.
    if (file) {
        file << encryptedPassword << "\n"; // Write encrypted password to file.
        file.close();
    }
}
int main() {
    PasswordManager manager;
    string username, password;

    // Prompt the user for login credentials.
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    // Authenticate the user with the provided credentials.
    if (!manager.authenticate(username, password)) {
        cout << "Login failed. Exiting program.\n";
        return 1; // Exit the program if authentication fails.
    }

    cout << "Login successful!\n";
    // Load the user's passwords into the manager for this session.
    manager.loadUserPasswords(username);

    // Main loop for password manager functionality.
    bool running = true;
    while (running) {
        cout << "Choose an option:\n";
        cout << "1. Create your own password\n";
        cout << "2. Retrieve a specific password\n";
        cout << "3. Have a password automatically generated for you\n";
        cout << "4. Log out\n";
        cout << "5. Remove a stored password\n";
        cout << "Option: ";

        int option;
        cin >> option;
        // Clear the input buffer to handle any lingering input before the next read.
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (option) {
        case 1: {
            // Option to manually enter and store a new password.
            cout << "Enter your password (must be more than 8 characters and contain at least one number and one letter): ";
            getline(cin, password);
            // Validate password strength.
            while (!isValidPassword(password)) {
                cout << "Invalid password. Please ensure your password meets the criteria.\n";
                cout << "Enter your password: ";
                getline(cin, password);
            }
            // Add the valid password to the manager.
            manager.addNewPassword(username, password);
            cout << "Password added successfully.\n";
            break;
        }
        case 2: {
            // Option to retrieve and display a specific password.
            auto passwords = manager.getUserPasswords(username);
            if (passwords.empty()) {
                cout << "No passwords stored.\n";
            }
            else {
                cout << "Select the password number to retrieve:\n";
                for (size_t i = 0; i < passwords.size(); i++) {
                    cout << i + 1 << ": Password " << i + 1 << endl;
                }
                int choice;
                cin >> choice;
                choice--; // Adjust choice to match zero-based indexing.
                if (choice >= 0 && choice < passwords.size()) {
                    cout << "Password " << choice + 1 << ": " << passwords[choice] << endl;
                }
                else {
                    cout << "Invalid selection.\n";
                }
            }
            break;
        }
        case 3: {
            // Option to have a password automatically generated and stored.
            string newPassword = generatePassword(10); // Generate a strong password.
            manager.addNewPassword(username, newPassword); // Store the new password.
            cout << "Generated Password: " << newPassword << endl;
            break;
        }
        case 4:
            // Option to log out of the password manager.
            running = false;
            cout << "Exiting.\n";
            break;
        case 5: {
            // Option to remove a stored password.
            auto passwords = manager.getUserPasswords(username);
            if (passwords.empty()) {
                cout << "No passwords stored.\n";
            }
            else {
                cout << "Select the password number to remove:\n";
                for (size_t i = 0; i < passwords.size(); i++) {
                    cout << i + 1 << ": Password " << i + 1 << endl;
                }
                int choice;
                cin >> choice;
                choice--; // Adjust choice to match zero-based indexing.
                if (choice >= 0 && choice < passwords.size()) {
                    manager.deleteUserPassword(username, choice);
                    cout << "Password " << choice + 1 << " removed successfully.\n";
                }
                else {
                    cout << "Invalid selection.\n";
                }
            }
            break;
        }
        default:
            // Handle invalid option selections.
            cout << "Invalid option. Please choose between 1 and 5.\n";
            break;
        }
    }

    return 0; // End the program after logging out.
}

