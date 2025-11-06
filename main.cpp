#include <iostream>

using namespace std;

enum Suit {Hearts, Clubs, Diamonds, Spades};
const string SUITVALUES[] = {"Hearts", "Clubs", "Diamonds", "Spades"};
const string RANKVALUES[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

class Card {
public:
    string rank;
    Suit suit;

    Card(Suit suit, string rank): suit(suit), rank(rank) { }

    int getRankValue() const {
        if (this->rank == "A") {
            return 11;
        } else if (this->rank == "K" || this->rank == "Q" || this->rank == "J") {
            return 10;
        } else {
            return stoi(this->rank);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& myCard) {
        os << myCard.rank << " of " << SUITVALUES[myCard.suit];
        return os;

    }
};


int main(){
    Card myCard = Card(Suit(0),"J");
    std::cout << myCard;
    return 0;
}