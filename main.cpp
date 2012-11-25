//Solution
#include <iostream>
#include <set>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>

#define MAXDIM 1000

using namespace std;

enum DIRECTION {
    RIGHT,
    DOWN,
    LEFT,
    UP
};

struct EndPoint {
    int x;
    int y;
    bool ver;
    EndPoint(int x, int y, bool ver) : x(x), y(y), ver(ver){};
};

struct Candidate {
    EndPoint * point;
    DIRECTION direction;
    bool clockwise;
    bool isAhead;
    Candidate(EndPoint * point, DIRECTION dir, bool clock, bool isAhead) : point(point), direction(dir), clockwise(clock), isAhead(isAhead) {}
};

struct PointComp {
    bool operator()(EndPoint * lhs, EndPoint * rhs) {
        return lhs->x == rhs->x ? lhs->y < rhs->y : lhs->x < rhs->x;
    }
};

struct Rectangle {
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int k;

    Rectangle(int xmin, int xmax, int ymin, int ymax, int k) : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax), k(k) {}

    bool isInside(int x, int y) {
        return x >= xmin && x <= xmax && y >= ymin && y <= ymax;
    }
};

int matrixW, matrixH;
bool ** usedMatrix;
map<pair<int, int>, bool> usedMap;

set<EndPoint*, PointComp> newPoints;
deque<EndPoint*> endPoints;
int X, Y;
Rectangle * BASERECT;

bool elligible(EndPoint * p) {
    if(p->x <= 0 || p->y <= 0) return false;
    if(BASERECT->isInside(p->x, p->y)) return false;
    if(p->x < matrixW && p->y < matrixH)
        return !usedMatrix[p->x][p->y];
    else
        return !usedMap[make_pair(p->x, p->y)];
}

void setUsed(EndPoint * p) {
    if(p->x < matrixW && p->y < matrixH)
        usedMatrix[p->x][p->y] = true;
    else
        usedMap[make_pair(p->x, p->y)] = true;
}

int nextGen(int genNum, bool & collided, int iterations) {
    int totalAdded = 0;
    //set<EndPoint*, PointComp> newPoints;
    while(iterations--) {
        EndPoint * v = endPoints.front();
        endPoints.pop_front();
        EndPoint * newPoint[2];
        if(v->ver) {
            newPoint[0] = new EndPoint(v->x - 1, v->y, false);
            newPoint[1] = new EndPoint(v->x + 1, v->y, false);
        } else {
            newPoint[0] = new EndPoint(v->x, v->y - 1, true);
            newPoint[1] = new EndPoint(v->x, v->y + 1, true);
        }
        delete v;
        for(int i = 0; i < 2; i++) {
            if(newPoint[i]->x == X && newPoint[i]->y == Y) {
                collided = true;
            }
            if(elligible(newPoint[i])) {
                setUsed(newPoint[i]);
                newPoints.insert(newPoint[i]);
                totalAdded++;
            } else {
                set<EndPoint*>::iterator it = newPoints.find(newPoint[i]);
                if(it != newPoints.end()) {
                    newPoints.erase(it);
                    totalAdded--;
                }
                delete newPoint[i];
            }
        }
    }
    for(set<EndPoint*,PointComp>::iterator it = newPoints.begin(); it != newPoints.end(); ++it) {
        endPoints.push_back(*it);
    }
    newPoints.clear();
    return totalAdded;
}

int simulate(int maxGens, int genNum) {
    bool collided = false;
    int added = endPoints.size();
    while(!collided && genNum < maxGens) {
        genNum++;
        added = nextGen(genNum, collided, added);
    }
    return collided ? genNum : -1;
}

void reset(int x, int y) {
    endPoints.clear();
    usedMap.clear();
    int w = (x < matrixW-1 ? x : matrixW-1);
    int h = (y < matrixH-1 ? y : matrixH-1);
    for(int i = 0; i <= w; i++) {
        for(int j = 0; j <= h; j++) {
            usedMatrix[i][j] = false;
        }
    }
}

Rectangle * findBaseRectangle(int x, int y) {
    Rectangle * ret = new Rectangle(0,1,0,2,2);

    while(!ret->isInside(x, y)) {
        ret->ymax *= 2;
        ret->xmax = ret->ymax - 1;
        ret->k++;
    }

    ret->ymax /= 2;
    ret->xmax = ret->ymax - 1;
    ret->k--;
    return ret;
}

Rectangle * findCornerRectangle(int x, int y, EndPoint * start, DIRECTION dir, bool clockwise) {
    Rectangle * ret, * prev;
    prev = new Rectangle(0,0,0,0,0);
    switch(dir) {
        case RIGHT:
            if(clockwise) {
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 1, start->y + 2, 2);
            } else {
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 1, start->y + 2, 2);
            }
        break;
        case DOWN:
            if(clockwise) {
                ret = new Rectangle(start->x - 1, start->x + 2, start->y - 1, start->y + 1, 2);
            } else {
                ret = new Rectangle(start->x - 2, start->x + 1, start->y - 1, start->y + 1, 2);
            }
        break;
        case LEFT:
            if(clockwise) {
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 2, start->y + 1, 2);
            } else {
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 2, start->y + 1, 2);
            }
        break;
        case UP:
            if(clockwise) {
                ret = new Rectangle(start->x - 2, start->x + 1, start->y - 1, start->y + 1, 2);
            } else {
                ret = new Rectangle(start->x - 1, start->x + 2, start->y - 1, start->y + 1, 2);
            }
        break;
    };

    while(!ret->isInside(x, y)) {
        prev->xmin = ret->xmin;
        prev->xmax = ret->xmax;
        prev->ymin = ret->ymin;
        prev->ymax = ret->ymax;
        prev->k = ret->k;
        ret->k++;
        int d = ((1 << (ret->k - 1)) - 1); //d = (2^k - 2) / 2
        switch(dir) {
            case RIGHT:
                ret->xmin = start->x - d;
                ret->xmax = start->x + d;
                ret->ymin = start->y - d;
                ret->ymax = start->y + (d + 1);
            break;
            case DOWN:
                if(clockwise) {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + (d + 1);
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + d;
                } else {
                    ret->xmin = start->x - (d + 1);
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + d;
                }
            break;
            case LEFT:
                ret->xmin = start->x - d;
                ret->xmax = start->x + d;
                ret->ymin = start->y - (d + 1);
                ret->ymax = start->y + d;
            break;
            case UP:
                if(clockwise) {
                    ret->xmin = start->x - (d + 1);
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + d;
                } else {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + (d + 1);
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + d;
                }
            break;
        };
    }

    delete ret;
    return prev;
}

EndPoint * findStartingPoint(int x, int y, Rectangle * curRect, DIRECTION & dir, bool & clockwise, int & stagesDone) {
    //TODO: wyznaczenie 3 punktow kandydatow, wyznaczenie ktory z nich wyprzedza pozostale o 1 pokolenie,
    //wybranie punktu do dalszego przetwarzania, zaktualizowanie orientacji,
    //liczby pokolen na podstawie wybranego punktu, zwrocenie tego punktu
}

void solve(int x, int y) {
    X = x; Y = y;
    if(X == 0 && (Y == 1 || Y == 0)) { //check whether bacteriostat is at stage 1
        cout<<1<<endl;
        return;
    }

    ////PHASE 1: find base rectangle (2^k x 2^k-1) such that bacteriostat does not lie inside and for k+1 it would lie inside
    BASERECT = findBaseRectangle(X, Y);
    bool closeEnough = false;
    reset(1 << BASERECT->k, 1 << BASERECT->k);
    //cout<<BASERECT->xmax<<" "<<BASERECT->ymax<<" "<<BASERECT->k<<endl;
    EndPoint * start;
    if(BASERECT->k < 2) { //the rule works for k >= 2
        start = new EndPoint(0,1,true);
        closeEnough = true;
    } else {
        start = new EndPoint(BASERECT->xmax+1, BASERECT->ymax, false);
        if(X == start->x && Y == start->y) {
            cout<<(1 << BASERECT->k)<<endl;
            delete start;
            delete BASERECT;
            return;
        }
    }

    //compute number of stages already passed
    int stagesDone = (BASERECT->k >= 2 ? (1 << BASERECT->k) : 1);

    ////PHASE 2: Iteratively solve corner case by finding the biggest corner rectangle such that bacteriostat does not lie within
    DIRECTION direction = RIGHT; //says what direction is the starting point heading
    bool clockwise = true; //says if the current corner case is clockwise oriented or not
    Rectangle * cornerRect;
    while(!closeEnough){
        cornerRect = findCornerRectangle(X, Y, start, direction, clockwise);
        if(cornerRect->k < 2) {
            closeEnough = true;
        } else {
            delete start;
            //find new starting point (one of 3 possible)
            start = findStartingPoint(X, Y, cornerRect, direction, clockwise, stagesDone);
            if(start->x == X && start->y == Y) {
                cout<<stagesDone<<endl;
                delete start;
                delete cornerRect;
                delete BASERECT;
                return;
            }
        }
        delete cornerRect;
    }

    //cout<<start->x<<" "<<start->y<<endl;

    ////PHASE 3: Simulate the final few steps
    //start simulation with cut off point set to 2^k + 1 which means that we will do 2^k + 1 iterations at most before declaring
    //that the simulation would run forever
    cout<<simulate((1 << (BASERECT->k+1)) + 1, stagesDone)<<endl;
    delete BASERECT;
}

void prepareSurface(int h, int w) {
    matrixW = (w > MAXDIM ? MAXDIM : w);
    matrixH = (h > MAXDIM ? MAXDIM : h);
    usedMatrix = new bool*[matrixW];
    for(int i = 0; i < matrixW; i++) {
        usedMatrix[i] = new bool[matrixH];
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    vector<pair<int, int> > testCases;
    int t, maxx = 0, maxy = 0;
    cin>>t;
    while(t--) {
        cin>>X>>Y;
        if(X < 0) X = -X;
        if(Y < 0) Y = -Y;
        if(X > maxx) maxx = X;
        if(Y > maxy) maxy = Y;
        testCases.push_back(make_pair(X, Y));
    }
    int d = (maxy > maxx ? maxy : maxx);
    prepareSurface(2*d + 1, 2*d + 1);
    for(int i = 0; i < testCases.size(); i++) {
        solve(testCases[i].first, testCases[i].second);
    }
    return 0;
}
