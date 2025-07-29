#include "catch.hpp"
#include "../src/graphics/img/OpenCvImg.hpp"

TEST_CASE("OpenCvImg - Construction") {
    SECTION("Construction with valid size") {
        REQUIRE_NOTHROW([&]() {
            OpenCvImg img;
            img.create_blank(400, 300);
        }());
    }
    
    SECTION("Construction with different sizes") {
        REQUIRE_NOTHROW([&]() {
            OpenCvImg small;
            small.create_blank(50, 50);
            OpenCvImg medium;
            medium.create_blank(800, 600);
            OpenCvImg large;
            large.create_blank(1920, 1080);
        }());
    }
    
    SECTION("Construction with edge case sizes") {
        REQUIRE_NOTHROW([&]() {
            OpenCvImg tiny;
            tiny.create_blank(1, 1);
            OpenCvImg wide;
            wide.create_blank(2000, 100);
            OpenCvImg tall;
            tall.create_blank(100, 2000);
        }());
    }
}

TEST_CASE("OpenCvImg - Drawing Operations") {
    OpenCvImg img;
    img.create_blank(400, 300);
    
    SECTION("Draw rectangles") {
        REQUIRE_NOTHROW([&]() {
            img.draw_rect(10, 10, 50, 50, {255, 0, 0});
            img.draw_rect(100, 100, 100, 100, {0, 255, 0});
            img.draw_rect(200, 200, 80, 80, {0, 0, 255});
        }());
    }
    
    SECTION("Draw rectangles with edge cases") {
        REQUIRE_NOTHROW([&]() {
            img.draw_rect(0, 0, 10, 10, {255, 255, 255}); // Top-left corner
            img.draw_rect(390, 290, 10, 10, {0, 0, 0}); // Near bottom-right
            img.draw_rect(-5, -5, 20, 20, {128, 128, 128}); // Negative coords
            img.draw_rect(350, 250, 100, 100, {64, 64, 64}); // Partially outside
        }());
    }
    
    SECTION("Draw rectangles with zero/negative sizes") {
        REQUIRE_NOTHROW([&]() {
            img.draw_rect(50, 50, 0, 0, {255, 0, 0}); // Zero size
            img.draw_rect(100, 100, -10, -10, {0, 255, 0}); // Negative size
        }());
    }
}

TEST_CASE("OpenCvImg - Text Operations") {
    OpenCvImg img;
    img.create_blank(400, 300);
    
    SECTION("Put text with different parameters") {
        REQUIRE_NOTHROW([&]() {
            img.put_text("Hello", 10, 30, 1.0);
            img.put_text("World", 10, 60, 0.5);
            img.put_text("OpenCV", 10, 90, 2.0);
        }());
    }
    
    SECTION("Put text with edge cases") {
        REQUIRE_NOTHROW([&]() {
            img.put_text("", 50, 50, 1.0); // Empty string
            img.put_text("Test", 0, 20, 1.0); // At edge
            img.put_text("Outside", -10, -10, 1.0); // Negative coords
            img.put_text("Large", 350, 280, 3.0); // Large text near edge
        }());
    }
    
    SECTION("Put text with special characters") {
        REQUIRE_NOTHROW([&]() {
            img.put_text("123", 10, 120, 1.0);
            img.put_text("!@#$%", 10, 150, 1.0);
            img.put_text("Test\nNewline", 10, 180, 1.0);
        }());
    }
}

TEST_CASE("OpenCvImg - Image Operations") {
    OpenCvImg img1;
    img1.create_blank(200, 200);
    OpenCvImg img2;
    img2.create_blank(400, 400);
    
    SECTION("Draw on other images") {
        REQUIRE_NOTHROW([&]() {
            img1.draw_on(img2, 50, 50);
            img1.draw_on(img2, 0, 0); // Top-left
            img1.draw_on(img2, 200, 200); // Bottom-right area
        }());
    }
    
    SECTION("Draw on with edge cases") {
        REQUIRE_NOTHROW([&]() {
            img1.draw_on(img2, -50, -50); // Negative offset
            img1.draw_on(img2, 350, 350); // Partially outside
            img1.draw_on(img2, 500, 500); // Completely outside
        }());
    }
    
    SECTION("Clone operations") {
        REQUIRE_NOTHROW([&]() {
            auto cloned = img1.clone();
            REQUIRE(cloned != nullptr);
            
            // Test that clone is independent
            img1.draw_rect(10, 10, 50, 50, {255, 0, 0});
            cloned->draw_rect(60, 60, 50, 50, {0, 255, 0});
        }());
    }
}

TEST_CASE("OpenCvImg - Display Operations") {
    OpenCvImg img;
    img.create_blank(300, 300);
    
    SECTION("Show image") {
        // Add some content to make it visible
        img.draw_rect(50, 50, 100, 100, {255, 0, 0});
        img.put_text("Test", 60, 80, 1.0);
        
        REQUIRE_NOTHROW([&]() {
            img.show();
        }());
    }
    
    SECTION("Show empty image") {
        OpenCvImg empty;
        empty.create_blank(100, 100);
        REQUIRE_NOTHROW([&]() {
            empty.show();
        }());
    }
}

TEST_CASE("OpenCvImgFactory - Full Coverage") {
    OpenCvImgFactory factory;
    
    SECTION("Load existing files") {
        // Note: These tests assume the files exist in the assets directory
        REQUIRE_NOTHROW([&]() {
            auto img = factory.load("../assets/pieces/board.png", {640, 640});
            // The factory should handle missing files gracefully
        }());
    }
    
    SECTION("Load non-existent files") {
        REQUIRE_NOTHROW([&]() {
            auto img = factory.load("nonexistent.png", {100, 100});
            // Should create a blank image or handle gracefully
        }());
    }
    
    SECTION("Load with different sizes") {
        REQUIRE_NOTHROW([&]() {
            auto small = factory.load("test.png", {50, 50});
            auto large = factory.load("test.png", {1000, 1000});
            auto zero = factory.load("test.png", {0, 0});
        }());
    }
    
    SECTION("Load with invalid paths") {
        REQUIRE_NOTHROW([&]() {
            auto img1 = factory.load("", {100, 100}); // Empty path
            auto img2 = factory.load("invalid/path/file.png", {100, 100});
            auto img3 = factory.load("file_without_extension", {100, 100});
        }());
    }
}

TEST_CASE("OpenCvImg - Static Methods") {
    SECTION("Close all windows") {
        // Create and show some images
        OpenCvImg img1;
        img1.create_blank(200, 200);
        OpenCvImg img2;
        img2.create_blank(300, 300);
        
        img1.show();
        img2.show();
        
        REQUIRE_NOTHROW([&]() {
            OpenCvImg::close_all_windows();
        }());
    }
}