#include "Book.h"
#include <iostream>
#include <iomanip>

std::string genreToString(Genre genre) {
    switch (genre) {
        case Genre::Fantasy:           return "Fantasy";
        case Genre::Mystery:           return "Mystery";
        case Genre::ScienceFiction:    return "Science Fiction";
        case Genre::HistoricalFiction: return "Historical Fiction";
    }
    return "Unknown";
}

bool genreFromMenuChoice(int choice, Genre& outGenre) {
    switch (choice) {
        case 1: outGenre = Genre::Fantasy;           return true;
        case 2: outGenre = Genre::Mystery;           return true;
        case 3: outGenre = Genre::ScienceFiction;    return true;
        case 4: outGenre = Genre::HistoricalFiction; return true;
        default: return false;
    }
}

Book::Book(int id,
           const std::string& title,
           const std::string& author,
           Genre genre,
           double score)
    : id_(id),
      title_(title),
      author_(author),
      genre_(genre),
      score_(score),
      available_(true) {}

int Book::getId() const { return id_; }
const std::string& Book::getTitle() const { return title_; }
const std::string& Book::getAuthor() const { return author_; }
Genre Book::getGenre() const { return genre_; }
double Book::getScore() const { return score_; }
bool Book::isAvailable() const { return available_; }

void Book::setScore(double newScore) { score_ = newScore; }
void Book::setAvailable(bool available) { available_ = available; }

void Book::printSummary() const {
    std::cout << "[ID " << id_ << "] \"" << title_ << "\" by " << author_
               << " | Genre: " << genreToString(genre_)
               << " | Score: " << std::fixed << std::setprecision(2) << score_
               << " | " << (available_ ? "Available" : "Borrowed")
               << std::endl;
}
