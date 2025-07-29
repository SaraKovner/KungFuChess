#include "catch.hpp"
#include "../src/graphics/img/MockImg.hpp"
#include "../src/graphics/img/ImgFactory.hpp"
#include "../src/graphics/img/OpenCvImg.hpp"

TEST_CASE("MockImg - Full Coverage") {
    SECTION("Construction with different sizes") {
        REQUIRE_NOTHROW([&]() {
            MockImg img1({100, 100});
            MockImg img2({800, 600});
            MockImg img3({1920, 1080});
        }());
    }
    
    SECTION("Drawing operations") {
        MockImg img({400, 300});
        
        REQUIRE_NOTHROW([&]() {
            img.draw_rect(10, 10, 50, 50, {255, 0, 0});
            img.draw_rect(0, 0, 100, 100, {0, 255, 0});
            img.draw_rect(-10, -10, 20, 20, {0, 0, 255}); // Negative coords
        }());
    }
    
    SECTION("Text operations") {
        MockImg img({400, 300});
        
        REQUIRE_NOTHROW([&]() {
            img.put_text("Hello", 10, 10, 1.0);
            img.put_text("World", 50, 50, 0.5);
            img.put_text("", 0, 0, 2.0); // Empty text
            img.put_text("Test", -10, -10, 1.0); // Negative coords
        }());
    }
    
    SECTION("Drawing on other images") {
        MockImg img1({200, 200});
        MockImg img2({400, 400});
        
        REQUIRE_NOTHROW([&]() {
            img1.draw_on(img2, 50, 50);
            img1.draw_on(img2, 0, 0);
            img1.draw_on(img2, -10, -10); // Negative offset
        }());
    }
    
    SECTION("Show and clone operations") {
        MockImg img({300, 300});
        
        REQUIRE_NOTHROW([&]() {
            img.show();
            auto cloned = img.clone();
            REQUIRE(cloned != nullptr);
            cloned->show();
        }());
    }
    
    SECTION("Edge cases") {
        REQUIRE_NOTHROW([&]() {
            MockImg tiny({1, 1});
            MockImg huge({10000, 10000});
            
            tiny.draw_rect(0, 0, 1, 1, {255, 255, 255});
            huge.put_text("Test", 5000, 5000, 1.0);
        }());
    }
}

TEST_CASE("MockImgFactory - Full Coverage") {
    MockImgFactory factory;
    
    SECTION("Load with different paths") {
        REQUIRE_NOTHROW([&]() {
            auto img1 = factory.load("test.png", {100, 100});
            auto img2 = factory.load("nonexistent.jpg", {200, 200});
            auto img3 = factory.load("", {50, 50}); // Empty path
            
            REQUIRE(img1 != nullptr);
            REQUIRE(img2 != nullptr);
            REQUIRE(img3 != nullptr);
        }());
    }
    
    SECTION("Load with different sizes") {
        REQUIRE_NOTHROW([&]() {
            auto small = factory.load("test.png", {10, 10});
            auto medium = factory.load("test.png", {500, 500});
            auto large = factory.load("test.png", {2000, 2000});
            
            REQUIRE(small != nullptr);
            REQUIRE(medium != nullptr);
            REQUIRE(large != nullptr);
        }());
    }
    
    SECTION("Load with zero/negative sizes") {
        REQUIRE_NOTHROW([&]() {
            auto zero = factory.load("test.png", {0, 0});
            auto negative = factory.load("test.png", {-100, -100});
            
            REQUIRE(zero != nullptr);
            REQUIRE(negative != nullptr);
        }());
    }
}

TEST_CASE("ImgFactory Interface") {
    SECTION("Pure virtual interface") {
        // Test that ImgFactory is properly abstract
        // This is tested by using MockImgFactory which implements it
        MockImgFactory factory;
        ImgFactoryPtr ptr = std::make_shared<MockImgFactory>();
        
        REQUIRE_NOTHROW([&]() {
            auto img = ptr->load("test.png", {100, 100});
            REQUIRE(img != nullptr);
        }());
    }
}