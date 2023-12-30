#include <bits/stdc++.h>
#include <string>
using namespace std;
#define printv(s) if(verbose) cout << s<<"\n"
#define debug(s) if(DEBUG) cout << s << "\n"
#define debugv(v) if(DEBUG){ for(auto x : v) cout << x << " "; cout << endl;}
typedef long long ll;
const bool DEBUG = true;
bool verbose = 0;

//poker hands
//10 royal flush
//9 straight flush
//8 four of a kind
//7 full house
//6 flush
//5 straight
//4 3 of a kind
//3 two pair
//2 pair
//1 high

const int SUITS = 4;
const int NUMS = 13;
const int HANDSIZE = 5;


const ll MOD =1e9+7;

ll fastpow(ll b, ll e,ll m){
    ll res = 1;
    while(e > 0){
        if(e&1) res = (res*b)%m;
        b=(b*b)%m;
        e/=2;
    }
    return res;
}
// cards will be num*10+suit
// hand strength + relative strength of hand within category
pair<int,int> handStrength(vector<int> hand){
    //check flush
    int flush = 1;
    int straight = 1;
    sort(hand.begin(),hand.end());
    vector<int> seen(NUMS);
    int relStrength = 0;
    for(int i = 0; i < HANDSIZE; i++){
        if(hand[i]%SUITS != hand[0]%SUITS) flush = 0;
        if(i > 0 && hand[i]/SUITS != hand[i-1]/SUITS+1) straight = 0;
        seen[hand[i]/SUITS]++;
        relStrength = relStrength*NUMS+hand[i]/SUITS;
    }
    if(flush && straight){
        return {9,relStrength};
    }
    if(straight){
        return {5,hand.back()/SUITS};
    }
    //check ace high straight sepperately
    //min must be 10, all numbers start from 0 tho so 9
    straight = 1;
    if(hand[0]/SUITS != 0) straight =0;
    for(int i = 1; i < HANDSIZE; i++){
        if(8+i != hand[i]/SUITS) straight =0;
    }
    if(flush && straight){
        return {10,0};
    }
    if(flush){
        if(hand[0]/4==0) relStrength = NUMS;
        else relStrength = hand.back()/4;
        return {6,relStrength};
    }
    if(straight){
        return {5,13};
    }
    vector<int> strengths(SUITS+1,0);
    vector<int> cnt(SUITS+1,0);
    for(int i = 1; i <= NUMS; i++){
        for(int j = 1; j<=SUITS; j++){
            if(seen[i%NUMS] == j){
                if(i==NUMS) strengths[j] = NUMS*fastpow(NUMS+1,cnt[j]++,MOD);
                else strengths[j] = i*fastpow(NUMS+1,cnt[j]++,MOD);
            }
        }
    }
    relStrength = 0;
    for(int i = SUITS; i > 0; i--){
        relStrength=relStrength*((i==1) ? fastpow(NUMS+1,4,MOD) : (NUMS+1))+strengths[i];
    }
    if(cnt[4] != 0){
        return {8,relStrength};
    }
    if(cnt[3]&&cnt[2]){
        return {7,relStrength};
    }
    if(cnt[3]) return {4,relStrength};
    if(cnt[2] == 2) return {3,relStrength};
    if(cnt[2]) return {2,relStrength};
    return {1,relStrength};
}

pair<int,int> bestHand(pair<int,int> &hand, vector<int> &middle){
    vector<int> cards(middle);
    cards.push_back(hand.first);
    cards.push_back(hand.second);
    sort(cards.begin(),cards.end());
    assert(cards.size()==7);
    pair<int,int> bestH = {0,0};
    vector<int> chand;
    vector<int> it(5,0);
    for(int i = 0; i < 5; i++){
        it[i] = i;
        chand.push_back(cards[i]);
    }
    while(it[0] < 3){
        bestH = max(bestH,handStrength(chand));
        int bck = 4;
        int fromback = 1;
        while(bck >= 0 && it[bck] == 7-fromback){
            chand.pop_back();
            bck--;
            fromback++;
        }
        if(bck < 0)break;
        chand.pop_back();
        it[bck]++;
        chand.push_back(cards[it[bck]]);
        bck++;
        while(bck < 5){
            it[bck] = it[bck-1]+1;
            chand.push_back(cards[it[bck]]);
            bck++;
        }
        assert(chand.size()==5);
    }
    return bestH;
}

/*
 * returns 1 if h1 is better, 0 if tied, -1 if h2 is better
 */
int compareHands(pair<int,int> h1, pair<int,int> h2, vector<int> middle){
    pair<int,int> h1Strength = bestHand(h1,middle);
    pair<int,int> h2Strength = bestHand(h2,middle);
    if(h1Strength > h2Strength) return 1;
    else if(h2Strength == h2Strength) return 0;
    return -1;
}


pair<vector<int>,int> calcRemHands(vector<pair<int,int>> hands, vector<int> excluded, vector<int> middle){
    vector<int> cards(SUITS*NUMS);
    for(int i = 0; i < hands.size(); i++){
        cards[hands[i].first] = 1;
        cards[hands[i].second] = 1;
    }
    for(int i = 0; i < excluded.size(); i++){
        cards[excluded[i]] = 1;
    }
    for(int i = 0; i < middle.size(); i++){
        cards[middle[i]] = 1;
    }
    vector<int> remCards;
    for(int i = 0; i < SUITS*NUMS; i++){
        if(cards[i]) continue;
        remCards.push_back(i);
    }
    vector<int> wins(hands.size(),0);
    int remsize = remCards.size();
    int totalEval = 0;
    int chooseCards = 5-middle.size();
    vector<int> it(chooseCards,0);
    for(int i = 0;i < chooseCards; i++){
        it[i] = i;
        middle.push_back(remCards[i]);
    }
    vector<vector<int>> stats(hands.size(),vector<int>(11,0));
    vector<vector<int>> tieStats(hands.size(),vector<int>(11,0));
    while(it[0] < remCards.size()-chooseCards){
        vector<int> best(1,0);
        pair<int,int> bVal = bestHand(hands[0],middle);
        for(int i = 1; i < hands.size(); i++){
            pair<int,int> curVal = bestHand(hands[i],middle);
            if(curVal > bVal){
                best.clear();
                best.push_back(i);
                bVal = curVal;
            }
            else if(curVal==bVal){
                best.push_back(i);
            }
            if(bVal.first==10)debugv(middle)
        }
        if(best.size()==1){
            wins[best[0]]++;
            stats[best[0]][bVal.first]++;
        }
        else{
            for(int i = 0; i <best.size(); i++){
                tieStats[best[i]][bVal.first]++;
            }
        }
        if(totalEval%1000==0) printv(totalEval);
        totalEval++;
        int bck = chooseCards-1;
        int fromback=1;
        while(bck >= 0 && it[bck] == remsize-fromback){
            middle.pop_back();
            bck--;
            fromback++;
        }
        if(bck < 0)break;
        middle.pop_back();
        it[bck]++;
        middle.push_back(remCards[it[bck]]);
        bck++;
        while(bck < chooseCards){
            it[bck] = it[bck-1]+1;
            middle.push_back(remCards[it[bck]]);
            bck++;
        }
    }
    for(int i = 0; i < hands.size(); i++){
        printv("Player " + to_string(i) +": ");
        for(int j = 1; j <= 10; j++){
            printv("j: " + to_string(j) + " stats: " + to_string(stats[i][j]));
        }
    }
    for(int i = 0; i < hands.size(); i++){
        printv("Tie Stats for Player " + to_string(i) +": ");
        for(int j = 1; j <= 10; j++){
            printv("j: " + to_string(j) + " stats: " + to_string(tieStats[i][j]));
        }
    }
    return {wins,totalEval};
}

map<string,int> wordToSuitNum {
    {"Spades",0},{"S",0},{"Clubs",1},{"C",1},
    {"Hearts",2},{"H",2},{"Diamonds",3},{"D",3}
};
map<string,int> wordToNumNum{
    {"Jack",11},{"J",11},{"Queen",12},{"Q",12},{"King",13},{"K",13},{"Ace",1},{"A",1}
};

int readCard(){
    int st = -1;
    while(st==-1){
        printv("What suit? {[S]pades or 0, [C]lubs or 1, [H]earts or 2, [D]iamonds or 3");
        string suit;
        cin >> suit;
        if(wordToSuitNum.count(suit) != 0){
            st = wordToSuitNum[suit];
            break;
        }
        try{
            int tmpst = stoi(suit);
            if(tmpst >= 0 && tmpst <= 3) st = tmpst;
        }
        catch(...){
            
        }
        if(st==-1) printv("That is not a valid suit");
    }
    int num = -1;
    while(num == -1){
        printv("What number? 1-13,[J]ack,[Q]ueen,[K]ing,[A]ce");
        string numb;
        cin >> numb;
        if(wordToNumNum.count(numb) != 0){
            numb = wordToSuitNum[numb];
            break;
        }
        try{
            int tmpnum = stoi(numb);
            if(tmpnum >= 1 && tmpnum <= 13) num = tmpnum;
        }
        catch(...){
            
        }
        if(num==-1) printv("That is not a valid num");
    }
    num--;
    debug(num*SUITS+st);
    return num*SUITS + st;

}
int uniqueCardsSeen = 0;
vector<int> cardsSeen(NUMS*SUITS+10);
int readNewCard(){
    int card = -1;
    while(card == -1){
        card = readCard();
        if(cardsSeen[card]){
            printv("Card has already been used");
            card = -1;
        }
    }
    cardsSeen[card] = 1;
    return card;
}

void evalHands(){
    fill(cardsSeen.begin(),cardsSeen.end(),0);
    int numHands=-1;
    while(numHands==-1){
        printv("Enter number of hands, 1-10");
        cin >> numHands;
        if(numHands < 1 || numHands > 10) numHands = -1;
        printv("Please enter a number between 1 and 10");
    }
    vector<pair<int,int>> hands(numHands);
    for(int i = 0; i < numHands; i++){
        printv("Player " + to_string(i+1) + "'s hand");
        hands[i].first = readNewCard();
        hands[i].second = readNewCard();
    }
    int cardsExcluded = -1;
    while(cardsExcluded ==-1){
        printv("Enter the number of cards eliminated from the pool, between 0 and 20");
        cin >> cardsExcluded;
        if(cardsExcluded < 0 || cardsExcluded > 20){
            cardsExcluded = -1;
        }
    }
    vector<int> excluded(cardsExcluded);
    for(int i = 0; i < cardsExcluded; i++){
        excluded[i] = readNewCard();
    }
    int cardsOnTable = -1;
    while(cardsOnTable == -1){
        printv("Enter the number of cards on the table, between 0 and 5");
        cin >> cardsOnTable;
        if(cardsOnTable < 0 || cardsOnTable > 5) cardsOnTable = -1;
    }
    vector<int> table(cardsOnTable);
    for(int i = 0; i < cardsOnTable; i++){
        table[i] = readNewCard();
    }
    auto res = calcRemHands(hands,  excluded, table);
    int totalPossibleHands = res.second;
    for(int i = 0; i < numHands; i++){
        cout << "HAND " << i+1 << ": " << res.first[i] << "/" << totalPossibleHands << ", " << res.first[i]*100.0/totalPossibleHands <<"%\n";
    }
}

void readAndCheckHand(){
    fill(cardsSeen.begin(),cardsSeen.end(),0);
    vector<int> hand(5);
    for(int i = 0; i < 5; i++){
        hand[i] = readNewCard(); 
    }
    auto hs = handStrength(hand);
    cout <<  hs.first << " " << hs.second<< endl;
}

int main(int argc, char *argv[]){
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i],"-v")==0) verbose = 1;
    }
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i],"-h")==0){
            readAndCheckHand();
            return 0;
        }
    }
    evalHands();
}
