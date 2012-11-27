//Solution, roughly O(log N) with a huge constant
#include <iostream>
#include <set>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>

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

struct PointComp {
    bool operator()(EndPoint * lhs, EndPoint * rhs) {
        return lhs->x == rhs->x ? lhs->y < rhs->y : lhs->x < rhs->x;
    }
};

struct Candidate {
    EndPoint * point;
    DIRECTION direction;
    bool clockwise;
    bool isAhead;
    int distance;

    Candidate(EndPoint * point, DIRECTION dir, bool clock, bool isAhead) : point(point), direction(dir), clockwise(clock), isAhead(isAhead) {}

    void setDistance(int x, int y) {
        int xd = x - point->x;
        int yd = y - point->y;
        if(xd < 0) xd = -xd;
        if(yd < 0) yd = -yd;
        distance = xd > yd ? xd : yd;
    }
};

bool candComp(Candidate * lhs, Candidate * rhs) {
    return lhs->distance == rhs->distance ? lhs->isAhead : lhs->distance < rhs->distance;
}

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

map<pair<int, int>, bool> usedMap;
set<EndPoint*, PointComp> newPoints;
deque<EndPoint*> endPoints;
int X, Y;
bool borderline = false;
Rectangle * BASERECT;

bool elligible(EndPoint * p) {
    if(p->x <= 0 || p->y <= 0) return false;
    if(BASERECT->isInside(p->x, p->y)) return false;
    return !usedMap[make_pair(p->x, p->y)];
}

void setUsed(EndPoint * p) {
    usedMap[make_pair(p->x, p->y)] = true;
}

int nextGen(int genNum, bool & collided, int iterations) {
    int totalAdded = 0;
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
                    EndPoint * pe = *it;
                    newPoints.erase(it);
                    delete pe;
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

void reset() {
    for(int i = 0; i < endPoints.size(); i++)
        delete endPoints[i];
    endPoints.clear();
    usedMap.clear();
    borderline = false;
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
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 2, start->y + 1, 2);
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
                ret = new Rectangle(start->x - 1, start->x + 1, start->y - 1, start->y + 2, 2);
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
                if(clockwise) {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + (d + 1);
                } else {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - (d + 1);
                    ret->ymax = start->y + d;
                }
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
                if(clockwise) {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - (d + 1);
                    ret->ymax = start->y + d;
                } else {
                    ret->xmin = start->x - d;
                    ret->xmax = start->x + d;
                    ret->ymin = start->y - d;
                    ret->ymax = start->y + (d + 1);
                }
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

void findStartingPoint(EndPoint *& start, int x, int y, Rectangle * r, DIRECTION & dir, bool & clockwise, int & stagesDone) {
    Candidate * candidates[3];
    switch(dir) {
        case RIGHT:
            if(clockwise) {
                candidates[0] = new Candidate(new EndPoint(r->xmin - 1, r->ymax, false),  LEFT, false,false);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymax, false), RIGHT, true, false);
                candidates[2] = new Candidate(new EndPoint(r->xmax, r->ymin - 1, true ),  DOWN, true, true);
            } else {
                candidates[0] = new Candidate(new EndPoint(r->xmax, r->ymax + 1, true),  UP, false, true);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymin, false), RIGHT, false, false);
                candidates[2] = new Candidate(new EndPoint(r->xmin - 1, r->ymin, false), LEFT, true, false);
            }
        break;
        case DOWN:
            if(clockwise) {
                candidates[0] = new Candidate(new EndPoint(r->xmax, r->ymax + 1, true),  UP,  false, false);
                candidates[1] = new Candidate(new EndPoint(r->xmax, r->ymin - 1, true),  DOWN, true, false);
                candidates[2] = new Candidate(new EndPoint(r->xmin - 1, r->ymin, false), LEFT, true, true);
            } else {
                candidates[0] = new Candidate(new EndPoint(r->xmin, r->ymax + 1, true),  UP,  true, false);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymin, false), RIGHT, false, true);
                candidates[2] = new Candidate(new EndPoint(r->xmin, r->ymin - 1, true),  DOWN, false, false);
            }
        break;
        case LEFT:
            if(clockwise) {
                candidates[0] = new Candidate(new EndPoint(r->xmin, r->ymax + 1, true),  UP,  true, true);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymin, false), RIGHT, false, false);
                candidates[2] = new Candidate(new EndPoint(r->xmin - 1, r->ymin, false), LEFT, true, false);
            } else {
                candidates[0] = new Candidate(new EndPoint(r->xmin - 1, r->ymax, false),  LEFT, false,false);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymax, false), RIGHT, true, false);
                candidates[2] = new Candidate(new EndPoint(r->xmin, r->ymin - 1, true ),  DOWN, false, true);
            }
        break;
        case UP:
            if(clockwise) {
                candidates[0] = new Candidate(new EndPoint(r->xmin, r->ymax + 1, true),  UP,  true, false);
                candidates[1] = new Candidate(new EndPoint(r->xmax + 1, r->ymax, false), RIGHT, true, true);
                candidates[2] = new Candidate(new EndPoint(r->xmin, r->ymin - 1, true),  DOWN, false, false);
            } else {
                candidates[0] = new Candidate(new EndPoint(r->xmin - 1, r->ymax, false), LEFT, false, true);
                candidates[1] = new Candidate(new EndPoint(r->xmax, r->ymax + 1, true), UP, false, false);
                candidates[2] = new Candidate(new EndPoint(r->xmax, r->ymin - 1, true), DOWN, true, false);
            }
        break;
    }
    for(int i = 0; i < 3; i++) {
        candidates[i]->setDistance(x, y);
    }
    sort(candidates, candidates + 3, candComp);
    if(candidates[0]->distance == candidates[1]->distance) {
        borderline = true;
    }
    dir = candidates[0]->direction;
    clockwise = candidates[0]->clockwise;
    stagesDone += (1 << r->k) - candidates[0]->isAhead;
    EndPoint * res = candidates[0]->point;
    delete candidates[1]->point;
    delete candidates[2]->point;
    for(int i=0;i<3;i++)delete candidates[i];
    delete start;
    start = res;
}

int solve(int x, int y) {
    X = x; Y = y;
    if(X == 0 && (Y == 1 || Y == 0)) {
        return 1;
    }

    if(BASERECT != 0) delete BASERECT;
    BASERECT = findBaseRectangle(X, Y);
    bool closeEnough = false;
    reset();
    EndPoint * start;
    if(BASERECT->k < 2) {
        start = new EndPoint(0,1,true);
        closeEnough = true;
    } else {
        start = new EndPoint(BASERECT->xmax+1, BASERECT->ymax, false);
        if(X == start->x && Y == start->y) {
            delete start;
            return 1 << BASERECT->k;
        }
    }

    //cout<<"xmin="<<BASERECT->xmin<<" xmax="<<BASERECT->xmax<<" ymin="<<BASERECT->ymin<<" ymax="<<BASERECT->ymax<<" x="<<start->x<<" y="<<start->y<<endl;

    int bound = 1 << (BASERECT->k+1);
    int stagesDone = (BASERECT->k >= 2 ? (1 << BASERECT->k) : 1);
    if(closeEnough) {
        setUsed(start);
        endPoints.push_back(start);
        return simulate(bound, stagesDone);
    }

    DIRECTION direction = RIGHT;
    bool clockwise = true;
    Rectangle * cornerRect;
    while(!closeEnough){
        cornerRect = findCornerRectangle(X, Y, start, direction, clockwise);
        //cout<<"xmin="<<cornerRect->xmin<<" xmax="<<cornerRect->xmax<<" ymin="<<cornerRect->ymin<<" ymax="<<cornerRect->ymax;
        if(cornerRect->k < 2) {
            closeEnough = true;
        } else {
            findStartingPoint(start, X, Y, cornerRect, direction, clockwise, stagesDone);
            //cout<<" x="<<start->x<<" y="<<start->y<<" dir="<<direction<<" k="<<cornerRect->k<<" done="<<stagesDone<<endl;
            if(start->x == X && start->y == Y) {
                delete start;
                delete cornerRect;
                return stagesDone;
            }
            if(stagesDone >= bound) {
                delete start;
                delete cornerRect;
                return -1;
            }
        }
        delete cornerRect;
    }

    if(elligible(start))endPoints.push_back(start);
    setUsed(start);
    if(!borderline) {
        return simulate(bound, stagesDone);
    } else {
        return -1;
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    int t;
    cin>>t;
    while(t--) {
        cin>>X>>Y;
        if(X < 0) X = -X;
        if(Y < 0) Y = -Y;
        cout<<solve(X, Y)<<endl;
    }
    return 0;
}
