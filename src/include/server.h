#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <queue>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

static bool is_mine[35][35];
static bool visited[35][35];
static bool marked[35][35];
static int mine_cnt[35][35];
static int fail_r = -1, fail_c = -1;

static const int dr[8] = {-1,-1,-1,0,0,1,1,1};
static const int dc[8] = {-1,0,1,-1,1,-1,0,1};

static inline bool in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }

static void compute_counts() {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      int cnt = 0;
      for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (in_bounds(nr, nc) && is_mine[nr][nc]) ++cnt;
      }
      mine_cnt[r][c] = cnt;
    }
  }
}

static void visit_zero_fill(int sr, int sc) {
  std::queue<std::pair<int,int>> q;
  q.emplace(sr, sc);
  visited[sr][sc] = true;
  while (!q.empty()) {
    auto [r, c] = q.front(); q.pop();
    if (mine_cnt[r][c] != 0) continue;
    for (int k = 0; k < 8; ++k) {
      int nr = r + dr[k], nc = c + dc[k];
      if (!in_bounds(nr, nc)) continue;
      if (visited[nr][nc]) continue;
      if (marked[nr][nc]) continue;
      if (is_mine[nr][nc]) continue;
      visited[nr][nc] = true;
      if (mine_cnt[nr][nc] == 0) q.emplace(nr, nc);
    }
  }
}

static void check_win() {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (!is_mine[r][c] && !visited[r][c]) { game_state = 0; return; }
    }
  }
  game_state = 1;
}

/**
 * @brief The definition of function InitMap()
 */
void InitMap() {
  std::cin >> rows >> columns;
  total_mines = 0;
  game_state = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      visited[r][c] = false;
      marked[r][c] = false;
      is_mine[r][c] = false;
      mine_cnt[r][c] = 0;
    }
  }
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch; std::cin >> ch;
      is_mine[r][c] = (ch == 'X');
      if (is_mine[r][c]) ++total_mines;
    }
  }
  fail_r = fail_c = -1;
  compute_counts();
}

/**
 * @brief The definition of function VisitBlock(int, int)
 */
void VisitBlock(int r, int c) {
  if (game_state != 0) return;
  if (!in_bounds(r, c)) return;
  if (visited[r][c] || marked[r][c]) { game_state = 0; return; }
  if (is_mine[r][c]) {
    visited[r][c] = true;
    fail_r = r; fail_c = c;
    game_state = -1;
    return;
  }
  if (mine_cnt[r][c] == 0) visit_zero_fill(r, c); else visited[r][c] = true;
  check_win();
}

/**
 * @brief The definition of function MarkMine(int, int)
 */
void MarkMine(int r, int c) {
  if (game_state != 0) return;
  if (!in_bounds(r, c)) return;
  if (visited[r][c]) { game_state = 0; return; }
  if (marked[r][c]) { game_state = 0; return; }
  marked[r][c] = true;
  if (!is_mine[r][c]) {
    fail_r = r; fail_c = c;
    game_state = -1;
    return;
  }
  check_win();
}

/**
 * @brief The definition of function AutoExplore(int, int)
 */
void AutoExplore(int r, int c) {
  if (game_state != 0) return;
  if (!in_bounds(r, c)) return;
  if (!visited[r][c]) return;
  if (is_mine[r][c]) return;
  int need = mine_cnt[r][c];
  int marked_around = 0;
  for (int k = 0; k < 8; ++k) {
    int nr = r + dr[k], nc = c + dc[k];
    if (in_bounds(nr, nc) && marked[nr][nc]) ++marked_around;
  }
  if (marked_around != need) return;
  for (int k = 0; k < 8; ++k) {
    int nr = r + dr[k], nc = c + dc[k];
    if (!in_bounds(nr, nc)) continue;
    if (visited[nr][nc]) continue;
    if (marked[nr][nc]) continue;
    if (is_mine[nr][nc]) continue;
    VisitBlock(nr, nc);
    if (game_state != 0) return;
  }
  check_win();
}

/**
 * @brief The definition of function ExitGame()
 */
void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
  }
  int visit_count = 0;
  int marked_mine_count = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (!is_mine[r][c] && visited[r][c]) ++visit_count;
      if (marked[r][c] && is_mine[r][c]) ++marked_mine_count;
    }
  }
  if (game_state == 1) marked_mine_count = total_mines;
  std::cout << visit_count << " " << marked_mine_count << std::endl;
  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 */
void PrintMap() {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char out = '?';
      if (game_state == 1) {
        if (is_mine[r][c]) out = '@';
        else out = char('0' + mine_cnt[r][c]);
      } else {
        if (marked[r][c]) {
          out = is_mine[r][c] ? '@' : 'X';
        } else if (visited[r][c]) {
          out = is_mine[r][c] ? 'X' : char('0' + mine_cnt[r][c]);
        } else {
          out = '?';
        }
      }
      std::cout << out;
    }
    std::cout << std::endl;
  }
}

#endif
