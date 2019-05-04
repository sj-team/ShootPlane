#include "common.h"

bool isLegal(const vector<vector<uchar>> & board , int x ,int y ) {
	if (x >= 0 && x < g_board_size_x && y >= 0 && y < g_board_size_y && board[y][x]==bs::empty)
		return true;
	return false;
}

bool board_change(vector<vector<uchar>> & board, const Pt & p1, int direction, bool set_plane) {
	switch (direction)
	{
	case 0:  //down
		for (int i = 0; i < 5; i++)
			if (set_plane)
				board[p1.y - 1][p1.x - 2 + i] = bs::body;
			else if (!isLegal(board , p1.x - 2 + i, p1.y - 1))
				return false;

		for (int i = 0; i < 3; i++)
			if (set_plane)
				board[p1.y - 3][p1.x - 1 + i] = bs::body;
			else if (!isLegal(board ,p1.x - 1 + i, p1.y - 3))
				return false;
		if (set_plane)
		{
			board[p1.y - 2][p1.x] = bs::body;
			board[p1.y][p1.x] = bs::head;
		}
		else if (!isLegal(board , p1.x, p1.y) || !isLegal(board,p1.x, p1.y - 2))
			return false;
		break;
	case 1:  // up
		for (int i = 0; i < 5; i++)
			if (set_plane)
				board[p1.y + 1][p1.x - 2 + i] = bs::body;
			else if (!isLegal(board, p1.x - 2 + i, p1.y + 1))
				return false;

		for (int i = 0; i < 3; i++)
			if (set_plane)
				board[p1.y + 3][p1.x - 1 + i] = bs::body;
			else if (!isLegal(board, p1.x - 1 + i, p1.y + 3))
				return false;
		if (set_plane)
		{
			board[p1.y + 2][p1.x] = bs::body;
			board[p1.y][p1.x] = bs::head;
		}
		else if (!isLegal(board, p1.x, p1.y) || !isLegal(board, p1.x, p1.y + 2))
			return false;
		break;
	case 2:  // right
		for (int i = 0; i < 5; i++)
			if (set_plane)
				board[p1.y - 2 + i][p1.x-1] = bs::body;
			else if (!isLegal(board, p1.x-1 , p1.y - 2+i))
				return false;

		for (int i = 0; i < 3; i++)
			if (set_plane)
				board[p1.y - 1 + i][p1.x - 3] = bs::body;
			else if (!isLegal(board, p1.x - 3, p1.y - 1+i))
				return false;
		if (set_plane)
		{
			board[p1.y][p1.x-2] = bs::body;
			board[p1.y][p1.x] = bs::head;
		}
		else if (!isLegal(board, p1.x, p1.y) || !isLegal(board, p1.x-2, p1.y))
			return false;
		break;
	case 3: //left
		for (int i = 0; i < 5; i++)
			if (set_plane)
				board[p1.y - 2 + i][p1.x + 1] = bs::body;
			else if (!isLegal(board, p1.x + 1, p1.y - 2 + i))
				return false;

		for (int i = 0; i < 3; i++)
			if (set_plane)
				board[p1.y - 1 + i][p1.x + 3] = bs::body;
			else if (!isLegal(board, p1.x + 3, p1.y - 1 + i))
				return false;
		if (set_plane)
		{
			board[p1.y][p1.x + 2] = bs::body;
			board[p1.y][p1.x] = bs::head;
		}
		else if (!isLegal(board, p1.x, p1.y) || !isLegal(board, p1.x + 2, p1.y))
			return false;
		break;
	default:
		break;
	}

	return true;
}

int ChessBoard::getnum() {
	return plane_num;
}

ChessBoard::ChessBoard(int id) {
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

	cout << "444" << endl;
	if (!p1.legal() || !p2.legal())
		return false;
	cout << "333" << endl;
	if (plane_pos.size() >= g_plane_num)
		return false;

	cout << "111" << endl;
	if (!(p1.x == p2.x && abs(p1.y - p2.y) == 3 || p1.y == p2.y && abs(p1.x - p2.x) == 3))
		return false;
	cout << "222" << endl;
	bool legal = true;

	int direction;
	// 0->down 1->up  2->right 3->left  
	if (p1.x == p2.x)
		direction = p1.y > p2.y ? 0 : 1;
	else
		direction = p1.x > p2.x ? 2 : 3;
	
	if (!board_change(board, p1, direction, false))
		return false;
	else
		board_change(board, p1, direction, true);

	plane_pos.push_back(pair<Pt, Pt>(p1, p2));
	plane_num++;
	return true;
}

uchar ChessBoard::getboard(const Pt & p, int cid) {
	if (cid != id && mask_board[p.y][p.x])
		return bs::unkown;
	return board[p.y][p.x];
}

bool ChessBoard::getmask(const Pt&p) {
	return mask_board[p.y][p.x];
}

