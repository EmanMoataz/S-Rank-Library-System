#include "Library.h"
#include <iostream>
#include <cmath>
#include <limits>

Library::Library()
    : currentDay_(1), open_(true), nextBookId_(1), nextUserId_(1) {}

// ---------------------------------------------------------------------
// Basic management
// ---------------------------------------------------------------------

void Library::addBook(const std::string& title,
                       const std::string& author,
                       Genre genre,
                       double score) {
    books_.emplace_back(nextBookId_, title, author, genre, score);
    std::cout << "Added book \"" << title << "\" with ID " << nextBookId_ << ".\n";
    nextBookId_++;
}

void Library::addUser(const std::string& name, bool hasCard) {
    users_.emplace_back(nextUserId_, name, hasCard);
    std::cout << "Added user \"" << name << "\" with ID " << nextUserId_ << ".\n";
    nextUserId_++;
}

void Library::displayBooks() const {
    if (books_.empty()) {
        std::cout << "No books in the system yet.\n";
        return;
    }
    std::cout << "--- Books (" << books_.size() << ") ---\n";
    for (const auto& book : books_) {
        book.printSummary();
    }
}

void Library::displayUsers() const {
    if (users_.empty()) {
        std::cout << "No users in the system yet.\n";
        return;
    }
    std::cout << "--- Users (" << users_.size() << ") ---\n";
    for (const auto& user : users_) {
        user.printSummary();
    }
}

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

Book* Library::findBook(int bookId) {
    for (auto& book : books_) {
        if (book.getId() == bookId) return &book;
    }
    return nullptr;
}

User* Library::findUser(int userId) {
    for (auto& user : users_) {
        if (user.getId() == userId) return &user;
    }
    return nullptr;
}

double Library::totalBorrowedScore(const User& user) const {
    double total = 0.0;
    for (int bookId : user.getBorrowedBookIds()) {
        for (const auto& book : books_) {
            if (book.getId() == bookId) {
                total += book.getScore();
                break;
            }
        }
    }
    return total;
}

// ---------------------------------------------------------------------
// Rule calculations
// ---------------------------------------------------------------------

double Library::noCardFee(double bookScore) {
    // y^2 * n  =>  (RULE_Y)^2 * bookScore
    return std::pow(RULE_Y, 2) * bookScore;
}

double Library::latePenalty(int lateDays) {
    // ((n * 2) ^ y) interpreted as exponentiation, not XOR:
    // ((lateDays * 2) ^ RULE_Y) = pow(lateDays * 2, RULE_Y)
    return std::pow(lateDays * 2, RULE_Y);
}

double Library::dailyGrowth(double currentScore) {
    // y * (n * 2 / 3), with n = the book's current score
    double growth = RULE_Y * (currentScore * 2.0 / 3.0);
    double newScore = currentScore + growth;
    if (newScore > 30.0) {
        newScore = 30.0; // do not exceed the 30-point boundary used elsewhere
    }
    return newScore;
}

// ---------------------------------------------------------------------
// Core operations
// ---------------------------------------------------------------------

void Library::borrowBook(int userId, int bookId) {
    if (!open_) {
        std::cout << "The library is closed. Cannot process a borrow request.\n";
        return;
    }

    User* user = findUser(userId);
    Book* book = findBook(bookId);
    if (!user) { std::cout << "No user found with ID " << userId << ".\n"; return; }
    if (!book) { std::cout << "No book found with ID " << bookId << ".\n"; return; }

    if (user->isCardRevoked()) {
        std::cout << user->getName() << "'s library card has been revoked due to "
                   << "repeated violations. Borrowing is not allowed.\n";
        return;
    }

    if (!book->isAvailable()) {
        std::cout << "\"" << book->getTitle() << "\" is currently unavailable.\n";
        offerWaitingList(bookId, userId);
        return;
    }

    // Rule 3: no active card
    if (!user->hasCard()) {
        if (book->getScore() > NO_CARD_FEE_MAX_SCORE) {
            std::cout << user->getName() << " has no active library card, and this "
                       << "book's score (" << book->getScore() << ") is above "
                       << NO_CARD_FEE_MAX_SCORE << ", so it cannot be borrowed without a card.\n";
            return;
        }
        double fee = noCardFee(book->getScore());
        std::cout << user->getName() << " has no active library card. A fee of "
                   << fee << " is charged to borrow this book without one.\n";
        // Fee is charged/announced but does not block the borrow, per the rule
        // as stated (it only restricts books with score > 22 outright).
    }

    // Rule 1: borrowing limit
    double projected = totalBorrowedScore(*user) + book->getScore();
    if (projected >= BORROW_SCORE_LIMIT) {
        std::cout << "Cannot borrow \"" << book->getTitle() << "\": total borrowed score "
                   << "would become " << projected << ", which is not below "
                   << BORROW_SCORE_LIMIT << ".\n";
        return;
    }

    // All checks passed.
    book->setAvailable(false);
    user->addBorrowedBook(bookId);
    std::cout << user->getName() << " successfully borrowed \"" << book->getTitle() << "\".\n";
}

void Library::returnBook(int userId, int bookId, int lateDays) {
    if (!open_) {
        std::cout << "The library is closed. Cannot process a return.\n";
        return;
    }

    User* user = findUser(userId);
    Book* book = findBook(bookId);
    if (!user) { std::cout << "No user found with ID " << userId << ".\n"; return; }
    if (!book) { std::cout << "No book found with ID " << bookId << ".\n"; return; }

    if (!user->hasBorrowed(bookId)) {
        std::cout << user->getName() << " does not currently have \"" << book->getTitle()
                   << "\" borrowed.\n";
        return;
    }

    user->removeBorrowedBook(bookId);
    book->setAvailable(true);
    std::cout << user->getName() << " returned \"" << book->getTitle() << "\".\n";

    if (lateDays > 0) {
        double penalty = latePenalty(lateDays);
        std::cout << "Book was " << lateDays << " day(s) late. Late penalty: "
                   << penalty << ".\n";

        // ASSUMPTION: a late return counts as one rule violation for the user.
        user->addViolation();
        std::cout << user->getName() << " now has " << user->getViolations()
                   << " violation(s).\n";
        if (user->isCardRevoked()) {
            std::cout << user->getName() << "'s library card has been permanently "
                       << "revoked after exceeding " << User::MAX_VIOLATIONS
                       << " violations.\n";
        }
    }

    // If anyone is waiting for this book, let the librarian know.
    auto it = waitingLists_.find(bookId);
    if (it != waitingLists_.end() && !it->second.empty()) {
        int nextUserId = it->second.front();
        User* nextUser = findUser(nextUserId);
        std::cout << "Note: \"" << book->getTitle() << "\" now has a waiting list. "
                   << "Next in line: "
                   << (nextUser ? nextUser->getName() : "Unknown user") << ".\n";
    }
}

void Library::offerWaitingList(int bookId, int userId) {
    std::cout << "Would you like to join the waiting list for this book? (y/n): ";
    char choice;
    std::cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        addToWaitingList(bookId, userId);
    } else {
        std::cout << "Okay, not joining the waiting list.\n";
    }
}

void Library::addToWaitingList(int bookId, int userId) {
    Book* book = findBook(bookId);
    User* user = findUser(userId);
    if (!book) { std::cout << "No book found with ID " << bookId << ".\n"; return; }
    if (!user) { std::cout << "No user found with ID " << userId << ".\n"; return; }

    auto& queue = waitingLists_[bookId];
    for (int existingId : queue) {
        if (existingId == userId) {
            std::cout << user->getName() << " is already on the waiting list for \""
                       << book->getTitle() << "\".\n";
            return;
        }
    }
    queue.push_back(userId);
    std::cout << user->getName() << " joined the waiting list for \""
               << book->getTitle() << "\" (position " << queue.size() << ").\n";
}

void Library::displayWaitingList(int bookId) const {
    auto it = waitingLists_.find(bookId);
    if (it == waitingLists_.end() || it->second.empty()) {
        std::cout << "The waiting list for book ID " << bookId << " is empty.\n";
        return;
    }
    std::cout << "--- Waiting list for book ID " << bookId << " ---\n";
    int position = 1;
    for (int userId : it->second) {
        const User* user = nullptr;
        for (const auto& u : users_) {
            if (u.getId() == userId) { user = &u; break; }
        }
        std::cout << position << ". " << (user ? user->getName() : "Unknown user")
                   << " (ID " << userId << ")\n";
        position++;
    }
}

void Library::advanceDay() {
    if (!open_) {
        std::cout << "The library is closed. Cannot advance the day.\n";
        return;
    }
    currentDay_++;
    for (auto& book : books_) {
        book.setScore(dailyGrowth(book.getScore()));
    }
    std::cout << "Day advanced to " << currentDay_ << ". All book scores have grown.\n";
}

void Library::closeLibrary() {
    open_ = false;
    std::cout << "The library is now closed. Borrowing, returning, and day-advancing "
               << "are disabled. You may still view books/users or exit.\n";
}

bool Library::isOpen() const { return open_; }
int Library::getCurrentDay() const { return currentDay_; }
