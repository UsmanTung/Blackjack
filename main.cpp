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

    string getRank(){
        return this->rank;
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& myCard) {
        os << myCard.rank << " of " << SUITVALUES[myCard.suit] << " ";
        return os;

    }
};

class Deck {
private:
    std::array<Card, 52> cards;
    int index;

public:
    Deck() {
        this->index = 0;
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

    Card& dealCard() {
        if (index < 52) {
            return cards[index++];
        } else {
            throw std::runtime_error("No more cards in deck!");
        }
    }

    int getIndex(){
        return this->index;
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

public:
    Hand() {}

    int addCard(Card& newCard){
        cards.push_back(newCard);
        return getTotal();
    }

    int getTotal(){
        int total = 0;
        int aces = 0;
        for (Card c: this->cards){
            int rank = c.getRankValue();
            total += rank;
            if (rank == 11){
                aces++;
            }
        }
        if (total > 21 && aces > 0) {
            total -= 10;
            aces -=1;
        }
        return total;
    }

    std::vector<Card>* getCards(){
        return &this->cards;
    }

    void clear(){
        this->cards.clear();
    }

    void print(std::ostream& os, bool hideSecond = false) const {
    for (size_t i = 0; i < cards.size(); i++) {
        if (hideSecond && i == 1) {
            os << "[Hidden Card]";
        } else {
            os << cards[i];
        }

        if (i != cards.size() - 1) {
            os << ", ";
        }
    }
}

};

class Player{
private:
    Hand hand;

public:
    Player(){
        hand = Hand();
    };
    
    void emptyHand(){
        this->hand.clear();
    }

    void dealCard(Card& card) {
        this->hand.addCard(card);
    }

    Hand& getHand() {
        return this->hand;
    }

};

class Dealer{
private:
    Hand hand;
    bool hidden;

public:
    Dealer(){
        this->hand = Hand();
        this->hidden = true;
    }

    void dealToPlayer(Player& player, Deck& deck) {
        player.dealCard(deck.dealCard());
    }

    void dealToSelf(Deck& deck) {
        hand.addCard(deck.dealCard());
    }

    bool isHidden(){
        return this->hidden;
    }

    void setHidden(){
        this->hidden = !this->hidden;
    }

    Hand& getHand() {
        return this->hand;
    }

    friend std::ostream& operator<<(std::ostream& os, const Dealer& dealer) {
        os << "Dealer's Hand: ";
        dealer.hand.print(os, dealer.hidden);
        return os;
    }
};

class Game {
private:
    Deck deck;
    Dealer dealer;
    Player player;
    bool gameOver;

public:
    Game() : gameOver(false) {
        deck.shuffleCards();
    }

    void startRound() {
        gameOver = false;
        player.emptyHand();
        dealer.getHand().clear();

        if (!dealer.isHidden()) dealer.setHidden();

        dealer.dealToPlayer(player, deck);
        dealer.dealToSelf(deck);
        dealer.dealToPlayer(player, deck);
        dealer.dealToSelf(deck);

        printState();
    }

    void playerTurn() {
        string action;
        while (true) {
            int playerTotal = getPlayerTotal();
            if (playerTotal == 21) {
                cout << "Blackjack! You win instantly!\n";
                gameOver = true;
                return;
            }

            cout << "\nYour move (hit / stand): ";
            cin >> action;

            if (action == "hit") {
                dealer.dealToPlayer(player, deck);
                printState();

                playerTotal = getPlayerTotal();
                if (playerTotal > 21) {
                    cout << "You busted!\n";
                    gameOver = true;
                    return;
                }
                if (playerTotal == 21) {
                    cout << "Blackjack! You win\n";
                    gameOver = true;
                    return;
                }
            } 
            else if (action == "stand") {
                cout << "You stand.\n";
                break;
            } 
            else {
                cout << "Invalid move.\n";
            }
        }
    }

    void dealerTurn() {
        // Reveal hidden card
        if (dealer.isHidden()) dealer.setHidden();
        printState();

        // Dealer hits until reaching 17 or higher
        while (getDealerTotal() <= 17) {
            cout << "Dealer hits.\n";
            dealer.dealToSelf(deck);
            printState();
        }

        if (getDealerTotal() > 21)
            cout << "Dealer busted!\n";
        else
            cout << "Dealer stands on " << getDealerTotal() << ".\n";
    }

    void evaluateWinner() {
        int playerTotal = getPlayerTotal();
        int dealerTotal = getDealerTotal();

        if (playerTotal > 21) {
            cout << "Dealer wins.\n";
        } 
        else if (dealerTotal > 21 || playerTotal > dealerTotal) {
            cout << "You win!\n";
        } 
        else if (dealerTotal > playerTotal) {
            cout << "Dealer wins.\n";
        } 
        else {
            cout << "Push (tie).\n";
        }
    }

    void play() {
        while (true) {
            if (deck.getIndex() > 40) {
                cout << "\nShuffling new deck...\n";
                deck.shuffleCards();
            }

            startRound();
            playerTurn();
            if (!gameOver) {
                dealerTurn();
                evaluateWinner();
            }

            string again;
            cout << "\nPlay again? (y/n): ";
            cin >> again;
            if (again != "y"){
                return;
            }
        }
    }

private:
    void printState() {
        cout << "\n---------------------------------\n";
        cout << dealer;
        if (!dealer.isHidden()) {
            cout << " (Total: " << getDealerTotal() << ")";
        }
        cout << endl;

        cout << "Player's Hand: ";
        player.getHand().print(cout);
        cout << " (Total: " << getPlayerTotal() << ")\n";
        cout << "---------------------------------\n";
    }

    int getPlayerTotal() {
        return player.getHand().getTotal();
    }

    int getDealerTotal() {
        return dealer.getHand().getTotal();
    }
};

int main(){
    Game game;
    game.play();
    return 0;
}