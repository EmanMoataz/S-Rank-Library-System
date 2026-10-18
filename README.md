# S-Rank Library Management System (v1)

## Build

```
g++ -std=c++17 -Wall -Wextra -o library_system main.cpp Book.cpp User.cpp Library.cpp
./library_system
```

No external libraries required. Compiled and smoke-tested with g++ (C++17).

## File layout

- `Book.h` / `Book.cpp` — the `Book` class and `Genre` enum class.
- `User.h` / `User.cpp` — the `User` class (stores borrowed book **IDs**, not full `Book` copies).
- `Library.h` / `Library.cpp` — owns all books/users/waiting lists and enforces every rule.
  `main.cpp` never contains rule logic — it only reads menu input and calls `Library` methods.
- `main.cpp` — the terminal menu.

## Assumptions made for ambiguous rules (stated explicitly, as requested)

1. **Rule 3 (no-card fee, `y^2 * n`)** — `n` is taken to be the **score of the book being borrowed**,
   since that's the only book-specific numeric value available in this context.
   So `fee = 4 * bookScore`. A user with no card can still borrow a book with score ≤ 22 (fee charged),
   but is refused outright for books with score > 22.

2. **Rule 4 (late penalty, `(n*2)^y`)** — `^` in the original challenge means "raised to the power of,"
   **not** C++'s bitwise XOR. It's implemented as `std::pow(lateDays * 2, 2)`.

3. **Rule 6 (daily growth, `y*(n*2/3)`)** — `n` is taken to be the **book's own current score**.
   So each day: `newScore = currentScore + 2*(currentScore*2/3)`, capped at 30 (the same ceiling
   used for the borrowing limit) so growth doesn't run away unbounded. Growth applies to every book
   in the library, not just borrowed ones — the challenge doesn't restrict it to borrowed books.

4. **What counts as a "violation" (Rule 5)** — the prompt tracks violations but never says what
   causes one. This version counts **any late return** (late days > 0) as exactly one violation.
   More than 4 total violations permanently revokes the card (matches "more than 4" from the rule).

5. **Initial card status** — a librarian can register a user with or without a card
   (`Add User` asks). This makes Rule 3 (no card) testable independently from Rule 5
   (card revoked after violations) — they're tracked as distinct states internally,
   though both surface through the same `hasCard()` check.

6. **Waiting list offer** — when a requested book is unavailable, the program asks interactively
   whether the user wants to join that book's waiting list (FIFO order preserved via `std::vector`).

## Rules implemented

1. Borrow limit: rejects if `currentBorrowedScore + newBookScore >= 30`.
2. Availability + waiting list (FIFO).
3. Library card check + no-card fee for score ≤ 22 books.
4. Late return penalty (`std::pow`, not `^`).
5. Violation tracking → permanent card revocation after more than 4 violations.
6. Daily score growth via `advanceDay()`, capped at 30.
7. Open/closed state — closing blocks borrow/return/advanceDay but Display and Exit still work.

## Suggested things to extend later (not needed for v1)

- Persisting data to a file between runs.
- Automatically notifying the next person in a waiting list when a book is returned (currently
  it just prints who's next — it doesn't auto-borrow for them).
- Letting a librarian manually reissue a revoked card.
# S-Rank-Library-System
Library Management System built with C++ and Object-Oriented Programming.
