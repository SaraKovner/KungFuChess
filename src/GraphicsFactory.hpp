#pragma once

#include "Graphics.hpp"
#include <memory>
#include <string>
#include "img/ImgFactory.hpp"
#include "nlohmann/json.hpp"


// Simple GraphicsFactory that forwards an image loader placeholder to
// Graphics.  In this head-less C++ port, the img_loader is unused but the
// factory mirrors the Python API expected by the unit tests.
class GraphicsFactory {
public:
    explicit GraphicsFactory(ImgFactoryPtr factory_ptr = nullptr)
        : img_factory(factory_ptr) {}

    std::shared_ptr<Graphics> load(const std::string& sprites_dir,
                                   const nlohmann::json& cfg,
                                   std::pair<int,int> cell_size) const {
        // Extract graphics settings from config
        bool loop = cfg.value("is_loop", true);
        double fps = cfg.value("frames_per_sec", 3.0); // Slower default FPS
        
        auto gfx = std::make_shared<Graphics>(sprites_dir, cell_size, img_factory, loop, fps);
        (void)cell_size; // unused for now
        return gfx;
    }
private:
    ImgFactoryPtr img_factory;
};