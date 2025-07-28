#include "Graphics.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <filesystem>
#include <iostream>

Graphics::Graphics(const std::string& sprites_folder,
	std::pair<int, int> cell_size,
	ImgFactoryPtr img_factory,
	bool loop_, double fps_)

	: loop(loop_), fps(fps_), frame_duration_ms(1000.0 / fps_) {

    namespace fs = std::filesystem;
    if(!sprites_folder.empty() && img_factory) {
        std::vector<fs::path> pngs;
        fs::path root(sprites_folder);
        if(fs::exists(root) && fs::is_directory(root)) {
            for(const auto& entry : fs::directory_iterator(root)) {
                if(entry.is_regular_file() && entry.path().extension() == ".png") {
                    pngs.push_back(entry.path());
                }
            }
            // Sort files numerically (1.png, 2.png, 3.png, etc.)
            std::sort(pngs.begin(), pngs.end(), [](const fs::path& a, const fs::path& b) {
                std::string stem_a = a.stem().string();
                std::string stem_b = b.stem().string();
                
                // Convert to numbers for proper numerical sorting
                int num_a = std::stoi(stem_a);
                int num_b = std::stoi(stem_b);
                
                return num_a < num_b;
            });
            
            for(size_t i = 0; i < pngs.size(); ++i) {
                const auto& p = pngs[i];
                auto img_ptr = img_factory->load(p.string(), {80, 80});
                if(img_ptr) {
                    frames.push_back(img_ptr);
                }
            }
        }
    }
}

void Graphics::reset(const Command& cmd) {
	start_ms = cmd.timestamp;
	cur_frame = 0;
}

void Graphics::update(int now_ms) {
	if (frames.empty()) return;
	
	int elapsed = now_ms - start_ms;
	double frames_passed_exact = elapsed / frame_duration_ms;
	size_t frames_passed = static_cast<size_t>(frames_passed_exact);
	

	
	if (loop) {
		cur_frame = frames_passed % frames.size();
	} else {
		cur_frame = std::min(frames_passed, frames.size() - 1);
	}
}

const ImgPtr Graphics::get_img() const {
	if (frames.empty()) throw std::runtime_error("Graphics has no frames loaded");
	// Frame display - removed spam
	return frames[cur_frame];
}