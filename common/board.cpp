#include "common.h"

ChessBoard::ChessBoard(int id ) {
	this->id = id; 
	for (int i = 0; i < g_board_size_y; i++) {
		board.push_back(vector<uchar>(g_board_size_x));
		mask_board.push_back(vector<bool>(g_board_size_x));
	}
	clear();
}

void ChessBoard::clear() {
	for (int i = 0; i < g_board_size_y; i++)
		for (int j = 0; j < g_board_size_x; j++) {
			board[i][j] = bs::empty;
			mask_board[i][j] = true;
		}

}

uchar ChessBoard::umask(const Pt & p) {
	if (!p.legal())
		return bs::err;

	mask_board[p.y][p.x] = false;
	return board[p.y][p.x];
}

bool ChessBoard::setPlane(const Pt & p1, const Pt & p2) {

	if (!p1.legal() || !p2.legal() )
		return false;
	if (plane_num >= g_plane_num)
		return false;

	if (!(p1.x == p2.x && abs(p1.y - p2.y) == 4 || p1.y == p2.y && abs(p1.x - p2.x) == 4))
		return false;

	bool legal = true;
	

	if (p1.x == p2.x) {
		int add = p1.y < p2.y ? 1 : -1;
		int x = p1.x;
		int y = min(p1.y, p2.y);
		for (int i =0 ;i<4;i++,y++)
			if (!Pt(x, y).legal() || board[x][y] != bs::empty)
			{
				legal = false;
				break;
			}
		if (!legal)
			return false;
		x = p1.x - 2;
		y = p1.y < p2.y ? p1.y + 1 : p1.y - 1;
		int y2 = p1.y < p2.y ? p1.y + 3 : p1.y - 3;
		int bound = p1.x + 2;
		for (; x < bound; x++) {
			if (!Pt(x, y).legal() || board[x][y] != bs::empty)
			{
				legal = false;
				break;
			}
			if (abs(p1.x - x) <= 1 && (!Pt(x, y2).legal() || board[x][y2] != bs::empty)) {
				legal = false;
				break;
			}
		}
		if (!legal)
			return false;

	}
	else {

	}
	plane_num++;
	return true;
}

bool ChessBoard::erasePlane(const Pt & p1, const Pt & p2) {
	if (!p1.legal() || !p2.legal())
		return false;
	return true;
}

uchar ChessBoard::getboard(const Pt & p , int cid ) {
	if (cid!=id && mask_board[p.y][p.x])
		return bs::unkown;
	return board[p.y][p.x];
}

bool ChessBoard::getmask(const Pt&p) {
	return mask_board[p.y][p.x];
}
