#include "User.h"
#include <algorithm>
#include <iostream>

User::User(int id, const std::string& name, bool hasCardInitially)
    : id_(id),
      name_(name),
      hasCard_(hasCardInitially),
      cardPermanentlyRevoked_(false),
      violations_(0) {}

int User::getId() const { return id_; }
const std::string& User::getName() const { return name_; }
bool User::hasCard() const { return hasCard_ && !cardPermanentlyRevoked_; }
int User::getViolations() const { return violations_; }
const std::vector<int>& User::getBorrowedBookIds() const { return borrowedBookIds_; }

bool User::hasBorrowed(int bookId) const {
    return std::find(borrowedBookIds_.begin(), borrowedBookIds_.end(), bookId)
           != borrowedBookIds_.end();
}

void User::addBorrowedBook(int bookId) {
    borrowedBookIds_.push_back(bookId);
}

void User::removeBorrowedBook(int bookId) {
    borrowedBookIds_.erase(
        std::remove(borrowedBookIds_.begin(), borrowedBookIds_.end(), bookId),
        borrowedBookIds_.end());
}

void User::addViolation() {
    violations_++;
    if (violations_ > MAX_VIOLATIONS) {
        revokeCard();
    }
}

void User::revokeCard() {
    hasCard_ = false;
    cardPermanentlyRevoked_ = true;
}

bool User::isCardRevoked() const {
    return cardPermanentlyRevoked_;
}

void User::printSummary() const {
    std::cout << "[ID " << id_ << "] " << name_
               << " | Card: " << (hasCard() ? "Active" : (cardPermanentlyRevoked_ ? "REVOKED" : "None"))
               << " | Violations: " << violations_
               << " | Borrowed books: " << borrowedBookIds_.size()
               << std::endl;
}
