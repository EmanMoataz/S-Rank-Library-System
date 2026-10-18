#pragma once

#include <vector>
#include <map>
#include <string>
#include "Book.h"
#include "User.h"

// The Library owns all Books and Users and enforces every business rule.
// main.cpp should only handle menu display and basic input collection;
// all rule logic (borrow limits, fees, penalties, growth, violations)
// lives here.
class Library {
public:
    Library();

    // --- Basic management ---
    void addBook(const std::string& title,
                 const std::string& author,
                 Genre genre,
                 double score);
    void addUser(const std::string& name, bool hasCard);

    void displayBooks() const;
    void displayUsers() const;

    // --- Core operations (each performs its own rule checks and prints results) ---
    void borrowBook(int userId, int bookId);
    void returnBook(int userId, int bookId, int lateDays);

    void addToWaitingList(int bookId, int userId);
    void displayWaitingList(int bookId) const;

    void advanceDay();
    void closeLibrary();
    bool isOpen() const;

    int getCurrentDay() const;

private:
    std::vector<Book> books_;
    std::vector<User> users_;
    std::map<int, std::vector<int>> waitingLists_;  // bookId -> ordered queue of userIds
    int currentDay_;
    bool open_;

    int nextBookId_;
    int nextUserId_;

    // --- Helpers ---
    Book* findBook(int bookId);
    User* findUser(int userId);
    double totalBorrowedScore(const User& user) const;

    // Offers the user the chance to join the waiting list for an unavailable book.
    void offerWaitingList(int bookId, int userId);

    // --- Business rule constants ---
    static constexpr double BORROW_SCORE_LIMIT = 30.0;      // Rule 1
    static constexpr double NO_CARD_FEE_MAX_SCORE = 22.0;   // Rule 3
    static constexpr double RULE_Y = 2.0;                   // shared "y" constant from the challenge

    // --- Business rule calculations ---
    // Rule 3: fee for borrowing without a card, for books with score <= 22.
    //   Formula given: y^2 * n, with y = 2.
    //   ASSUMPTION: "n" is the score of the book being borrowed (the only
    //   book-specific numeric value available in this context). So:
    //   fee = (y^2) * bookScore = 4 * bookScore.
    static double noCardFee(double bookScore);

    // Rule 4: late penalty. Formula given: ((n * 2) ^ y), with y = 2, n = late days.
    //   NOTE: '^' in the original challenge means "to the power of", NOT the
    //   C++ bitwise XOR operator. So this is implemented as std::pow(n * 2, 2),
    //   i.e. (n * 2) * (n * 2).
    static double latePenalty(int lateDays);

    // Rule 6: daily book score growth. Formula given: y * (n * 2 / 3), y = 2.
    //   ASSUMPTION: "n" is the book's own current score (the only numeric
    //   value a Book carries that this formula could sensibly grow from).
    //   So: growth = 2 * (currentScore * 2 / 3), capped so the new score
    //   never exceeds 30 (the same boundary used for the borrowing limit).
    static double dailyGrowth(double currentScore);
};
