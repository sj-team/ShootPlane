#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

const unsigned char ERR = 0x99;

const int g_board_size_x = 10;
const int g_board_size_y = 10;
const int g_plane_num = 3;


typedef unsigned char uchar;

// board state 
namespace bs {
	const uchar empty = 0x00;
	const uchar head = 0x01;
	const uchar body = 0x02;
	const uchar unkown = 0x03;
	const uchar err = ERR;
}

bool isLegal(const vector<vector<uchar>> & board , int x ,int y ) ;

struct Pt {
	int x, y;
	Pt(int x, int y) {
		this->x = x;
		this->y = y;
	}
	bool legal() const {
		if (x >= 0 && x < g_board_size_x && y >= 0 && y < g_board_size_y )
			return true;

		return false;
	}
};

class ChessBoard {
private:
	vector<vector<uchar>> board;
	vector<vector<bool>> mask_board;
	vector<pair<Pt, Pt>> plane_pos;
	int plane_num = 0;
	int id;

public:
	ChessBoard(int id);
	void clear();
	uchar unmask(const Pt & p);
	bool unmaskPlane(const Pt & p1 ,const Pt& p2);
	uchar getboard(const Pt & p, int cid = -1);
	bool getmask(const Pt & p);
	bool setPlane(const Pt & p1, const Pt & p2);
	int getnum();

	bool initLocate(const Pt &p1_1,const Pt &p1_2 ,const Pt &p2_1,const Pt &p2_2,const Pt &p3_1,const Pt &p3_2);
	//bool erasePlane(const Pt & p1, const Pt & p2);
};

