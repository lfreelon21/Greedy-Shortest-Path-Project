/*
Name: Leroy Freelon III
Date: 11/11/22
Description: Option 1: Wave pathfinding algorithm to navigate a maze array with obstacles
Usage: run program, then input values when prompted by console.
<exe>
*/
#include <iostream>
#include <queue>
#include <stack>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <fstream>
#include <vector>

typedef std::pair<int, int> PAIR;
typedef std::queue<std::pair<int,int>> PAIR_QUEUE;

using namespace std;
//fxn prototypes
void PrintEnvironment(vector<vector<int>>&, int, int);
void floodFill(vector<vector<int>>&, int, int, PAIR);
bool validCoords(PAIR, int, int);
PAIR nextValidinNeighborhood(PAIR, PAIR, int, int);
PAIR_QUEUE validNeighborhood(PAIR, int, int);
bool withinOne(int, int);
void Splash(vector<vector<int>>&, PAIR, int, int);
bool emptyInValidNeighborhood(vector<vector<int>>&, PAIR, int, int);
PAIR_QUEUE greedyPath(vector<vector<int>>&, PAIR, PAIR, int, int);
void insertImpassibles(vector<vector<int>>&, int, int, int);
vector<PAIR> uniqueCoords(int, int, int);
vector<vector<char>> charArr(vector<vector<int>>&, int, int);


//operators for pairs to shorten addition, subtraction, and output formatting
PAIR operator+ (const PAIR& lhs, const PAIR& rhs){

    PAIR p3;
    p3.first = lhs.first + rhs.first;
    p3.second = lhs.second + rhs.second;

    return p3;
}
PAIR operator- (const PAIR& lhs, const PAIR& rhs){

    PAIR p3;
    p3.first = lhs.first - rhs.first;
    p3.second = lhs.second - rhs.second;

    return p3;
}
ostream& operator<< (ostream& os, const PAIR& p){
    
    os << "(" << p.first << "," << p.second << ")"; 

    return os;
}

int main(){
    
    int width = 0, height = 0, pctImpassible = -1, goalX, goalY, startX, startY;
    char choice;
    bool proceed = true;
    ofstream of;

    cout << "Welcome to Leroy's CSE240 WaveFront Pather\n\n"
    << "Please tell me about the grid you want to generate.\n";

    while(!(width >= 10 && height >= 10)){
        cout << "Enter the width (at least 10): ";
        cin >> width;
        cout << "Enter the height (at least 10): ";
        cin >> height;
    }

    vector<vector<int>> vec(height, vector<int>(width, 0));

    while(!(pctImpassible >= 0 && pctImpassible <= 100)){
        cout << "\nEnter the percent of impassible terrain (0-100): ";
        cin >> pctImpassible;
    }
    if(pctImpassible > 35){
        cout << "\nHaving a value greater than 35% might create poor results, do you wish to continue? (y/n): ";
        cin >> choice;
        while(tolower(choice) != 'n' && tolower(choice != 'y')){
            cout << "\nChoice must be y or n.\n";
            cin >> choice;
        } 
        if(tolower(choice) == 'n') proceed = false;
    }
    if(proceed){

        insertImpassibles(vec, pctImpassible, width, height);
        PrintEnvironment(vec, width, height);

        cout << "\nPlease enter the Goal Position X: ";
        cin >> goalX;
        cout << "Please enter the Goal Position Y: ";
        cin >> goalY;

        while(!validCoords(PAIR(goalY, goalX), width, height) || vec[goalY][goalX] == -1){
            cout << "\n\nSorry, that position is inside an obstacle\n";
            cout << "Please enter the Goal Position X: ";
            cin >> goalX;
            cout << "Please enter the Goal Position Y: ";
            cin >> goalY;
        }

        vec[goalY][goalX] = 1;
        PAIR goal(goalY, goalX);
        floodFill(vec, width, height, goal);//flood array with values
        PrintEnvironment(vec, width, height);

        vector<vector<char>> charArray = charArr(vec, width, height);//dynamically create char array

        cout << "\nPlease enter the Start Position X: ";
        cin >> startX;
        cout << "Please enter the Start Position Y: ";
        cin >> startY;

        while(!validCoords(PAIR(startY, startX), width, height) || vec[startY][startX] == -1){
            cout << "\n\nSorry, that position is inside an obstacle\n";
            cout << "\nPlease enter the Start Position X: ";
            cin >> startX;
            cout << "Please enter the Start Position Y: ";
            cin >> startY;
        }

        PAIR start(startY, startX);
        charArray[startY][startX] = '@';
        charArray[goalY][goalX] = '$';
        PAIR_QUEUE greedy = greedyPath(vec, start, goal, width, height);

        if(greedy.back() != goal || greedy.empty()) cout << "\nNo Path Found.\n";
        
        while(!greedy.empty()){
            if(greedy.front() != start && greedy.front() != goal)
                charArray[greedy.front().first][greedy.front().second] = '*';
            greedy.pop();
        }
        PrintEnvironment(vec, width, height);
        cout <<"\n";
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                cout << charArray[i][j];
            }
            cout << endl;
        }

        of.open("wave.txt");
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                of << charArray[i][j];
            }
            of << endl;
        }
        of.close();

    }
}

void floodFill(vector<vector<int>> &vec, int w, int h, PAIR origin){

    int centerVal = vec[origin.first][origin.second];

    vector<PAIR> validList = {
        PAIR(origin.first - 1, origin.second),
        PAIR(origin.first +1, origin.second),
        PAIR(origin.first, origin.second +1),
        PAIR(origin.first, origin.second -1),
        PAIR(origin.first - 1, origin.second + 1),
        PAIR(origin.first + 1, origin.second + 1),
        PAIR(origin.first - 1, origin.second - 1),
        PAIR(origin.first + 1, origin.second - 1)
    };
    
    Splash(vec, origin, w, h);

    for(PAIR dir : validList){

        if(validCoords(dir, w, h) && emptyInValidNeighborhood(vec, dir, w, h) && vec[dir.first][dir.second] != -1){
            floodFill(vec, w, h, dir);
        }
    }
}

bool validCoords(PAIR origin, int w, int h){//check that coordinates are within bounds of array
    
    int origY = origin.first, origX = origin.second;

    if(origY >= 0 && origY < h && 
        origX >= 0 && origX < w) return true;
    return false;
}
PAIR nextValidinNeighborhood(PAIR center, PAIR start, int w, int h){

    //start-origin gives relative pos
    PAIR relPos = start-center, next(-1,-1);
    int y = start.first, x = start.second;
    //then increment clockwise
    while(!validCoords(next, w, h)){//check each set of clockwise coords
        //increment next before loop iteration
        if(next == start) break;

        if(relPos.first < 0){
            if(relPos.second > 0) next = PAIR(y +1, x);
            else next = PAIR(y, x+1);
        }
        else if(relPos.first > 0){
            if(relPos.second < 0) next = PAIR(y-1, x);
            else next = PAIR(y, x-1);
        }
        else{
            if(relPos.second>0) next = PAIR(y+1, x);
            else if(relPos.second<0) next = PAIR(y-1, x);
        }
        relPos = next-center;
        y = next.first;
        x = next.second;
    }
    
    return next;  
}
PAIR_QUEUE validNeighborhood(PAIR center, int w, int h){

    PAIR_QUEUE validList;
    PAIR firstValid = nextValidinNeighborhood(center, PAIR(center.first-1, center.second), w, h), curr;//finds first valid coords starting from north direction with center as pivot

    validList.push(firstValid);
    curr = nextValidinNeighborhood(center, firstValid, w, h);

    while(curr != firstValid){//O(8) worst case ~~ O(1)

        validList.push(curr);
        curr = nextValidinNeighborhood(center, curr, w, h);
    }
    return validList;
}
bool withinOne(int n, int m){

    if(n-m == 1 || n-m == -1 || n-m == 0) return true;
    return false;
}
void Splash(vector<vector<int>> &vec, PAIR center, int w, int h){

    PAIR_QUEUE Neighborhood = validNeighborhood(center, w, h), cleanedNeighborhood;
    int centerVal = vec[center.first][center.second], currVal;

    if(centerVal == 0 || centerVal == -1) return;

    while(!Neighborhood.empty()){//O(8) ~~ O(1)
        
        currVal = vec[Neighborhood.front().first][Neighborhood.front().second];

        if(currVal != -1) cleanedNeighborhood.push(Neighborhood.front());
        
        Neighborhood.pop();
    }
    while(!cleanedNeighborhood.empty()){// O(8) ~~ O(1)

        currVal = vec[cleanedNeighborhood.front().first][cleanedNeighborhood.front().second];

        if(currVal == 0 || (!withinOne(centerVal, currVal) && centerVal < currVal)) 
            vec[cleanedNeighborhood.front().first][cleanedNeighborhood.front().second] = centerVal + 1;

        cleanedNeighborhood.pop();
    }
}
bool emptyInValidNeighborhood(vector<vector<int>> &vec, PAIR center, int w, int h){//O(1)

    PAIR_QUEUE n = validNeighborhood(center, w, h);
    int centerVal = vec[center.first][center.second], curr;

    if (centerVal == 0) return false;

    while(!n.empty()){//O(8) ~~ O(1)

        curr = vec[n.front().first][n.front().second];

        if(curr == 0 || (!withinOne(curr, centerVal) && centerVal < curr)) return true;
        n.pop();
    }
    return false;
}
PAIR_QUEUE greedyPath(vector<vector<int>> &vec, PAIR start, PAIR goal, int w, int h){

    PAIR curr = start;
    PAIR_QUEUE possibleMoves, Path;
    if(start == PAIR(0,0)) return Path;
    PAIR move;
    int diffGreatest = 0, diffCurr;

    while(vec[curr.first][curr.second] != vec[goal.first][goal.second] && diffGreatest != -3){

        possibleMoves = validNeighborhood(curr, w, h);
        diffGreatest = -3;

        while(!possibleMoves.empty()){

            diffCurr = vec[curr.first][curr.second] - vec[possibleMoves.front().first][possibleMoves.front().second];

            if(diffCurr > diffGreatest && vec[possibleMoves.front().first][possibleMoves.front().second] != -1){
                diffGreatest = diffCurr;
                move = possibleMoves.front();
            }
            possibleMoves.pop();
        }
        Path.push(move);
        curr = move;
    }
    return Path;
}
void insertImpassibles(vector<vector<int>> &vec, int percentage, int w, int h){//input impassibles into arr at random locs

    int numElems = w * h;
    double percentDecimal = percentage / 100.0;
    int numImpassible = percentDecimal * numElems;
    while(numImpassible >= numElems-1) numImpassible--;//ensure numImpassible is not greater than or equal to number of elements in array - 1
    
    vector<PAIR> impassibles = uniqueCoords(numImpassible, w, h);
    for(int i = 0; i < numImpassible; i++){
        vec[impassibles[i].first][impassibles[i].second] = -1;
    }
}
vector<PAIR> uniqueCoords(int num, int w, int h){//Pair array of unique coordinates

    srand((unsigned)time(NULL));
    vector<PAIR> randPairs(num, make_pair(0,0));
    bool overlap;

    for(int i = 0; i < num; i++){
        overlap = false;
        PAIR p = make_pair(rand() % h, rand() % w);
        for(int j = 0; j < num; j++){
            if(randPairs[j] == p) overlap = true;//ensure unique and not goal position
        }
        if(overlap) i--;
        else randPairs[i] = p;
    }
    return randPairs;
}
void PrintEnvironment(vector<vector<int>> &vec, int w, int h){
    
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            cout << setw(4) << vec[i][j];
        }
        cout << endl;
    }
}
vector<vector<char>> charArr(vector<vector<int>> &vec, int w, int h){

    vector<vector<char>> charArray(h, vector<char>(w, ' '));

    for(int i = 0; i < h; i++){//input obstacles and empty chars
        for(int j = 0; j < w; j++){
            if(vec[i][j] == -1) charArray[i][j] = '#';
        }
    }
    return charArray;
}