#include "catch.hpp"
#include "../src/core/Common.hpp"
#include "../src/graphics/Graphics.hpp"
#include "../src/ui/Command.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "MockFactories.hpp"

TEST_CASE("Graphics construction") {
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    
    SECTION("Basic constructor") {
        REQUIRE_NOTHROW([&]() {
            Graphics graphics("pieces/PW", std::make_pair(50, 50), img_factory, true, 0.2);
        }());
    }
    
    SECTION("Constructor with different parameters") {
        REQUIRE_NOTHROW([&]() {
            Graphics graphics("pieces/KB", std::make_pair(100, 100), img_factory, false, 0.5);
        }());
    }
}

TEST_CASE("Graphics image loading") {
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    Graphics graphics("pieces/RW", std::make_pair(75, 75), img_factory, true, 0.3);
    
    SECTION("Update method") {
        REQUIRE_NOTHROW([&]() {
            graphics.update(1000);
            graphics.update(2000);
        }());
    }
    
    SECTION("Reset with command") {
        REQUIRE_NOTHROW([&]() {
            Command cmd(1000, "RW1", "move", {{0,0}, {1,1}});
            graphics.reset(cmd);
        }());
    }
}

TEST_CASE("Graphics drawing operations") {
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    Graphics graphics("pieces/QW", std::make_pair(80, 80), img_factory, true, 0.4);
    
    ImgPtr mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    
    SECTION("Update at different times") {
        REQUIRE_NOTHROW([&]() {
            graphics.update(0);
            graphics.update(500);
            graphics.update(1000);
        }());
    }
    
    SECTION("Frame counting") {
        REQUIRE_NOTHROW([&]() {
            auto frame = graphics.current_frame();
            REQUIRE(frame >= 0);
        }());
    }
}

TEST_CASE("Graphics state management") {
    ImgFactoryPtr img_factory = std::make_shared<MockImgFactory>();
    Graphics graphics("pieces/NB", std::make_pair(60, 60), img_factory, false, 0.1);
    
    SECTION("Command reset") {
        REQUIRE_NOTHROW([&]() {
            Command cmd(1000, "NB1", "move", {{0,0}, {1,1}});
            graphics.reset(cmd);
        }());
    }
    
    SECTION("Frame management") {
        REQUIRE_NOTHROW([&]() {
            auto frame = graphics.current_frame();
            REQUIRE(frame >= 0);
        }());
    }
}
