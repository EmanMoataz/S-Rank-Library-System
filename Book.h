#pragma once

#include <string>

// The four genres supported by the S-Rank Library System.
enum class Genre {
    Fantasy,
    Mystery,
    ScienceFiction,
    HistoricalFiction
};

// Converts a Genre value to a human-readable string (for menus/printing).
std::string genreToString(Genre genre);

// Converts a user-typed number (1-4) into a Genre. Returns false if invalid.
bool genreFromMenuChoice(int choice, Genre& outGenre);

// Represents a single book owned by the library.
class Book {
public:
    Book(int id,
         const std::string& title,
         const std::string& author,
         Genre genre,
         double score);

    // --- Getters ---
    int getId() const;
    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    Genre getGenre() const;
    double getScore() const;
    bool isAvailable() const;

    // --- Setters ---
    void setScore(double newScore);
    void setAvailable(bool available);

    // Prints a single line describing this book. Used by Library::displayBooks().
    void printSummary() const;

private:
    int id_;
    std::string title_;
    std::string author_;
    Genre genre_;
    double score_;
    bool available_;
};
