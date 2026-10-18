#pragma once

#include <string>
#include <vector>

// Represents a library member. Stores borrowed book IDs rather than full
// Book objects, since the Library already owns the authoritative Book data.
class User {
public:
    // Max allowed violations before a card is permanently revoked (rule: "more than 4").
    static const int MAX_VIOLATIONS = 4;

    // hasCardInitially lets the librarian register a "guest" user who never
    // had a card at all, distinct from a user whose card gets revoked later.
    User(int id, const std::string& name, bool hasCardInitially = true);

    // --- Getters ---
    int getId() const;
    const std::string& getName() const;
    bool hasCard() const;
    int getViolations() const;
    const std::vector<int>& getBorrowedBookIds() const;

    // Sum of the scores of all currently-borrowed books. The caller supplies
    // a lookup function-like source (Library) since User does not own Books.
    bool hasBorrowed(int bookId) const;

    // --- Mutators ---
    void addBorrowedBook(int bookId);
    void removeBorrowedBook(int bookId);

    void addViolation();   // increments violation count; revokes card if it now exceeds MAX_VIOLATIONS
    void revokeCard();
    bool isCardRevoked() const;

    void printSummary() const;

private:
    int id_;
    std::string name_;
    bool hasCard_;          // whether the user currently holds an active card
    bool cardPermanentlyRevoked_; // true once revoked due to violations (cannot be re-issued in v1)
    int violations_;
    std::vector<int> borrowedBookIds_;
};
