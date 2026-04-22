#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

static char view_map[35][35];
static bool initialized = false;

static inline bool c_in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }
static const int c_dr[8] = {-1,-1,-1,0,0,1,1,1};
static const int c_dc[8] = {-1,0,1,-1,1,-1,0,1};

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details See advanced.cpp
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 */
void InitGame() {
  for (int r = 0; r < 35; ++r) for (int c = 0; c < 35; ++c) view_map[r][c] = '?';
  initialized = true;
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 */
void ReadMap() {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch; std::cin >> ch;
      view_map[r][c] = ch;
    }
  }
}

static int count_marked_around(int r, int c) {
  int m = 0;
  for (int k = 0; k < 8; ++k) {
    int nr = r + c_dr[k], nc = c + c_dc[k];
    if (c_in_bounds(nr, nc) && view_map[nr][nc] == '@') ++m;
  }
  return m;
}

static void unknown_neighbors(int r, int c, std::vector<std::pair<int,int>>& out) {
  out.clear();
  for (int k = 0; k < 8; ++k) {
    int nr = r + c_dr[k], nc = c + c_dc[k];
    if (c_in_bounds(nr, nc) && view_map[nr][nc] == '?') out.emplace_back(nr, nc);
  }
}

/**
 * @brief The definition of function Decide()
 */
void Decide() {
  // Rule 1: auto-explore when marked equals number and there exists unknown neighbor
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (view_map[r][c] >= '0' && view_map[r][c] <= '8') {
        int num = view_map[r][c] - '0';
        int marked = count_marked_around(r, c);
        std::vector<std::pair<int,int>> unk;
        unknown_neighbors(r, c, unk);
        if (marked == num && !unk.empty()) {
          Execute(r, c, 2);
          return;
        }
      }
    }
  }
  // Rule 2: if unknown count + marked equals number, mark one unknown
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (view_map[r][c] >= '0' && view_map[r][c] <= '8') {
        int num = view_map[r][c] - '0';
        int marked = count_marked_around(r, c);
        std::vector<std::pair<int,int>> unk;
        unknown_neighbors(r, c, unk);
        if (!unk.empty() && marked + (int)unk.size() == num) {
          auto [nr, nc] = unk[0];
          Execute(nr, nc, 1);
          return;
        }
      }
    }
  }
  // Rule 3: choose safest unknown by simple probability heuristic
  double bestRisk = 1e9; int bestR = -1, bestC = -1;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (view_map[r][c] != '?') continue;
      double riskSum = 0.0; int contrib = 0;
      for (int k = 0; k < 8; ++k) {
        int nr = r + c_dr[k], nc = c + c_dc[k];
        if (!c_in_bounds(nr, nc)) continue;
        if (view_map[nr][nc] >= '0' && view_map[nr][nc] <= '8') {
          int num = view_map[nr][nc] - '0';
          int marked = count_marked_around(nr, nc);
          std::vector<std::pair<int,int>> unk;
          unknown_neighbors(nr, nc, unk);
          int U = (int)unk.size();
          int remaining = num - marked;
          if (U > 0 && remaining >= 0) {
            riskSum += (double)remaining / (double)U;
            ++contrib;
          }
        }
      }
      double risk = contrib ? (riskSum / contrib) : 0.5;
      if (risk < bestRisk) { bestRisk = risk; bestR = r; bestC = c; }
    }
  }
  if (bestR != -1) {
    Execute(bestR, bestC, 0);
    return;
  }
  // Fallback: pick first unknown
  for (int r = 0; r < rows; ++r) for (int c = 0; c < columns; ++c) if (view_map[r][c] == '?') { Execute(r, c, 0); return; }
}

#endif
