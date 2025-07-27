#include "Board.hpp"

#include <cmath>    // std::round
#include <iostream> // std::cout

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Board::Board(int cell_H_pix_,
             int cell_W_pix_,
             int W_cells_,
             int H_cells_,
             const ImgPtr& image,
             double cell_H_m_,
             double cell_W_m_)
    : cell_H_pix(cell_H_pix_),
      cell_W_pix(cell_W_pix_),
      W_cells(W_cells_),
      H_cells(H_cells_),
      img(image),
      cell_H_m(cell_H_m_),
      cell_W_m(cell_W_m_) {}

// ---------------------------------------------------------------------------
Board Board::clone() const {
    ImgPtr new_img = img->clone();
    return Board(cell_H_pix, cell_W_pix, W_cells, H_cells, new_img, cell_H_m, cell_W_m);
}

// ---------------------------------------------------------------------------
void Board::show() const {
    img->show();
}

// ---------------------------------------------------------------------------
std::pair<int, int> Board::m_to_cell(const std::pair<double, double>& pos_m) const {
    double x_m = pos_m.first;
    double y_m = pos_m.second;
    int col = static_cast<int>(std::round(x_m / cell_W_m));
    int row = static_cast<int>(std::round(y_m / cell_H_m));
    return {row, col};
}

std::pair<double, double> Board::cell_to_m(const std::pair<int, int>& cell) const {
    int r = cell.first;
    int c = cell.second;
    return {static_cast<double>(c) * cell_W_m, static_cast<double>(r) * cell_H_m};
}

std::pair<int, int> Board::m_to_pix(const std::pair<double, double>& pos_m) const {
    double x_m = pos_m.first;
    double y_m = pos_m.second;
    int x_px = static_cast<int>(std::round(x_m / cell_W_m * cell_W_pix));
    int y_px = static_cast<int>(std::round(y_m / cell_H_m * cell_H_pix));
    return {x_px, y_px};
}