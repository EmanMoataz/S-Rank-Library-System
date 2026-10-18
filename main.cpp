#include <iostream>
#include <limits>
#include <string>
#include <cstdlib>
#include "Book.h"
#include "User.h"
#include "Library.h"

// Clears a bad std::cin state and discards the rest of the current line.
// Used after every numeric read so leftover input/newlines don't corrupt
// the next prompt.
void clearInputLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputLine();
            return value;
        }
        if (std::cin.eof()) {
            std::cout << "\nInput stream ended. Exiting.\n";
            std::exit(0);
        }
        std::cout << "Please enter a whole number.\n";
        clearInputLine();
    }
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputLine();
            return value;
        }
        if (std::cin.eof()) {
            std::cout << "\nInput stream ended. Exiting.\n";
            std::exit(0);
        }
        std::cout << "Please enter a number.\n";
        clearInputLine();
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

bool readYesNo(const std::string& prompt) {
    std::cout << prompt;
    char c;
    std::cin >> c;
    clearInputLine();
    return c == 'y' || c == 'Y';
}

void printMenu(const Library& library) {
    std::cout << "\n=================================\n";
    std::cout << "      S-RANK LIBRARY SYSTEM\n";
    std::cout << "=================================\n";
    std::cout << "Day: " << library.getCurrentDay()
               << " | Status: " << (library.isOpen() ? "OPEN" : "CLOSED") << "\n\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Add User\n";
    std::cout << "3. View Books\n";
    std::cout << "4. View Users\n";
    std::cout << "5. Borrow Book\n";
    std::cout << "6. Return Book\n";
    std::cout << "7. View Waiting List\n";
    std::cout << "8. Advance Day\n";
    std::cout << "9. Close Library\n";
    std::cout << "10. Exit\n\n";
    std::cout << "Choose an option: ";
}

void handleAddBook(Library& library) {
    std::string title = readLine("Title: ");
    std::string author = readLine("Author: ");

    std::cout << "Genre (1=Fantasy, 2=Mystery, 3=Science Fiction, 4=Historical Fiction): ";
    int genreChoice;
    std::cin >> genreChoice;
    clearInputLine();

    Genre genre;
    if (!genreFromMenuChoice(genreChoice, genre)) {
        std::cout << "Invalid genre choice. Defaulting to Fantasy.\n";
        genre = Genre::Fantasy;
    }

    double score = readDouble("Score: ");
    library.addBook(title, author, genre, score);
}

void handleAddUser(Library& library) {
    std::string name = readLine("Name: ");
    bool hasCard = readYesNo("Does this user have an active library card? (y/n): ");
    library.addUser(name, hasCard);
}

void handleBorrowBook(Library& library) {
    int userId = readInt("User ID: ");
    int bookId = readInt("Book ID: ");
    library.borrowBook(userId, bookId);
}

void handleReturnBook(Library& library) {
    int userId = readInt("User ID: ");
    int bookId = readInt("Book ID: ");
    int lateDays = readInt("Number of late days (0 if on time): ");
    library.returnBook(userId, bookId, lateDays);
}

void handleWaitingList(const Library& library) {
    int bookId = readInt("Book ID: ");
    library.displayWaitingList(bookId);
}

int main() {
    Library library;
    bool running = true;

    while (running) {
        printMenu(library);
        int choice = readInt("");

        switch (choice) {
            case 1: handleAddBook(library); break;
            case 2: handleAddUser(library); break;
            case 3: library.displayBooks(); break;
            case 4: library.displayUsers(); break;
            case 5: handleBorrowBook(library); break;
            case 6: handleReturnBook(library); break;
            case 7: handleWaitingList(library); break;
            case 8: library.advanceDay(); break;
            case 9: library.closeLibrary(); break;
            case 10:
                std::cout << "Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "Invalid option. Please choose 1-10.\n";
        }
    }

    return 0;
}
