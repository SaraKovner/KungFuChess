#pragma once

#include <utility>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const noexcept {
        return static_cast<size_t>(p.first) * 31u + static_cast<size_t>(p.second);
    }
};

// Forward declarations
struct Command;

class Piece;
using PiecePtr = std::shared_ptr<Piece>;

class ImgFactory;
using ImgFactoryPtr = std::shared_ptr<ImgFactory>;

class Img;
using ImgPtr = std::shared_ptr<Img>;

class Graphics;
using GraphicsPtr = std::shared_ptr<Graphics>;

class State;
using StatePtr = std::shared_ptr<State>;