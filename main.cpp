#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <array>
#include <iterator>
#include <algorithm>

using namespace std;

enum Suit {Hearts, Clubs, Diamonds, Spades};
const string SUITVALUES[] = {"Hearts", "Clubs", "Diamonds", "Spades"};
const string RANKVALUES[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

class Card {
private:
    string rank;
    Suit suit;

public:
    Card() : suit(Hearts), rank("2") {}

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

class Deck {
private:
    std::array<Card, 52> cards;
    int index;

public:
    Deck() {
        int count = 0;
        for (int i=0; i<4; i++){
            for(int j=0; j<13; j++){
                this->cards[count++] = Card((Suit)i, RANKVALUES[j]);
            }
        }
    }
    
    void shuffleCards() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(this->cards.begin(), this->cards.end(), std::default_random_engine(seed));
        this->index = 0;
    }

    Card& operator[](int i) {
        return this->cards[i];
    }

    friend std::ostream& operator<<(std::ostream& os, Deck& myDeck){
        for (int i=0; i < 52; i++){
            os << myDeck[i] << endl;
        }
        return os;
    }

};

class Hand{
private:
    std::vector<Card> cards;
    int aces;

public:
    Hand(){
        this->aces = 0;
    }

    int addCard(Card& newCard){
        cards.push_back(newCard);
        if (newCard.getRankValue() == 11){
            this->aces++;
        }
        return getTotal();
    }

    int getTotal(){
        int total = 0;
        for (Card c: this->cards){
            total += c.getRankValue();
        }
        if (total > 21 && this->aces > 0) {
            total -= 10;
            this->aces -=1;
        }
        return total;
    }

};

class Player{
private:
    Hand hand;

public:
    Player(){
        hand = Hand();
    };
};


int main(){
    Deck cards = Deck();
    std::cout << cards << endl;
    cards.shuffleCards();
    std::cout << cards;
    return 0;
}