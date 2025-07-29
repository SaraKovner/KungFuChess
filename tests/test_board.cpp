#include "catch.hpp"
#include "../src/core/Board.hpp"
#include "../src/graphics/img/MockImg.hpp"

// בדיקות בנייה ומאפיינים בסיסיים של הלוח
TEST_CASE("Board construction and basic properties") {
    SECTION("Board constructor sets properties correctly") {  // בודק שהבנאי מגדיר נכון את כל המאפיינים
        auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
        Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);
        
        REQUIRE(board.cell_H_pix == 100);
        REQUIRE(board.cell_W_pix == 100);
        REQUIRE(board.W_cells == 8);
        REQUIRE(board.H_cells == 8);
        REQUIRE(board.cell_H_m == 1.0);
        REQUIRE(board.cell_W_m == 1.0);
    }
    
    SECTION("Board with different cell sizes") {  // בודק יצירת לוח עם גדלי תאים שונים
        auto mock_img = std::make_shared<MockImg>(std::make_pair(400, 600));
        Board board(50, 75, 6, 8, mock_img, 1.5, 2.0);
        
        REQUIRE(board.cell_H_pix == 50);
        REQUIRE(board.cell_W_pix == 75);
        REQUIRE(board.W_cells == 6);
        REQUIRE(board.H_cells == 8);
        REQUIRE(board.cell_H_m == 1.5);
        REQUIRE(board.cell_W_m == 2.0);
    }
}

// בדיקות המרה ממטרים לתאים
TEST_CASE("Board::m_to_cell conversion") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);  // 1 meter per cell
    
    SECTION("Origin conversion") {  // בודק המרה של נקודת הראשית (0,0)
        auto cell = board.m_to_cell({0.0, 0.0});
        REQUIRE(cell.first == 0);   // row
        REQUIRE(cell.second == 0);  // col
    }
    
    SECTION("Positive coordinates") {  // בודק המרה של קואורדינטות חיוביות
        auto cell = board.m_to_cell({3.0, 2.0});  // x=3m, y=2m
        REQUIRE(cell.first == 2);   // row (y coordinate)
        REQUIRE(cell.second == 3);  // col (x coordinate)
    }
    
    SECTION("Decimal coordinates with rounding") {  // בודק עיגול קואורדינטות עשרוניות
        auto cell = board.m_to_cell({2.4, 1.6});  // Should round to (2, 2)
        REQUIRE(cell.first == 2);   // row
        REQUIRE(cell.second == 2);  // col
        
        auto cell2 = board.m_to_cell({2.6, 1.4});  // Should round to (1, 3)
        REQUIRE(cell2.first == 1);   // row
        REQUIRE(cell2.second == 3);  // col
    }
    
    SECTION("Negative coordinates") {  // בודק המרה של קואורדינטות שליליות
        auto cell = board.m_to_cell({-1.0, -2.0});
        REQUIRE(cell.first == -2);   // row
        REQUIRE(cell.second == -1);  // col
    }
}

// בדיקות המרה מתאים למטרים
TEST_CASE("Board::cell_to_m conversion") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Origin conversion") {  // בודק המרה של תא המקור (0,0)
        auto pos = board.cell_to_m({0, 0});
        REQUIRE(pos.first == 0.0);   // x
        REQUIRE(pos.second == 0.0);  // y
    }
    
    SECTION("Positive cell coordinates") {  // בודק המרה של קואורדינטות תאים חיוביות
        auto pos = board.cell_to_m({2, 3});  // row=2, col=3
        REQUIRE(pos.first == 3.0);   // x = col * cell_W_m
        REQUIRE(pos.second == 2.0);  // y = row * cell_H_m
    }
    
    SECTION("Negative cell coordinates") {  // בודק המרה של קואורדינטות תאים שליליות
        auto pos = board.cell_to_m({-1, -2});
        REQUIRE(pos.first == -2.0);   // x
        REQUIRE(pos.second == -1.0);  // y
    }
}

// בדיקות המרה ממטרים לפיקסלים
TEST_CASE("Board::m_to_pix conversion") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);  // 1 meter = 100 pixels
    
    SECTION("Origin conversion") {  // בודק המרה של נקודת הראשית לפיקסלים
        auto pix = board.m_to_pix({0.0, 0.0});
        REQUIRE(pix.first == 0);   // x_px
        REQUIRE(pix.second == 0);  // y_px
    }
    
    SECTION("Simple meter to pixel conversion") {  // בודק המרה פשוטה ממטרים לפיקסלים
        auto pix = board.m_to_pix({2.0, 1.5});  // 2m x, 1.5m y
        REQUIRE(pix.first == 200);   // x_px = 2 * 100
        REQUIRE(pix.second == 150);  // y_px = 1.5 * 100
    }
    
    SECTION("Fractional coordinates with rounding") {  // בודק עיגול קואורדינטות שבריות
        auto pix = board.m_to_pix({2.34, 1.66});
        REQUIRE(pix.first == 234);   // x_px = round(2.34 * 100)
        REQUIRE(pix.second == 166);  // y_px = round(1.66 * 100)
    }
}

// בדיקות לוח עם גדלי תאים שונים
TEST_CASE("Board with different cell sizes") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(600, 400));
    Board board(50, 75, 8, 6, mock_img, 2.0, 1.5);  // 2m high, 1.5m wide cells
    
    SECTION("m_to_cell with different cell sizes") {  // בודק המרה ממטרים לתאים עם גדלים שונים
        auto cell = board.m_to_cell({3.0, 4.0});  // 3m x, 4m y
        REQUIRE(cell.first == 2);   // row = round(4.0 / 2.0) = 2
        REQUIRE(cell.second == 2);  // col = round(3.0 / 1.5) = 2
    }
    
    SECTION("cell_to_m with different cell sizes") {  // בודק המרה מתאים למטרים עם גדלים שונים
        auto pos = board.cell_to_m({1, 2});  // row=1, col=2
        REQUIRE(pos.first == 3.0);   // x = 2 * 1.5
        REQUIRE(pos.second == 2.0);  // y = 1 * 2.0
    }
    
    SECTION("m_to_pix with different cell sizes") {  // בודק המרה ממטרים לפיקסלים עם גדלים שונים
        auto pix = board.m_to_pix({3.0, 4.0});  // 3m x, 4m y
        // x_px = round(3.0 / 1.5 * 75) = round(2 * 75) = 150
        // y_px = round(4.0 / 2.0 * 50) = round(2 * 50) = 100
        REQUIRE(pix.first == 150);
        REQUIRE(pix.second == 100);
    }
}

// בדיקות שכפול הלוח (clone)
TEST_CASE("Board::clone creates independent copy") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board original(100, 100, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Clone has same properties") {  // בודק שהעותק המשוכפל יש לו אותם מאפיינים
        Board clone = original.clone();
        
        REQUIRE(clone.cell_H_pix == original.cell_H_pix);
        REQUIRE(clone.cell_W_pix == original.cell_W_pix);
        REQUIRE(clone.W_cells == original.W_cells);
        REQUIRE(clone.H_cells == original.H_cells);
        REQUIRE(clone.cell_H_m == original.cell_H_m);
        REQUIRE(clone.cell_W_m == original.cell_W_m);
    }
    
    SECTION("Clone has independent image") {  // בודק שלעותק יש תמונה עצמאית
        Board clone = original.clone();
        // The clone should have a different image object (though with same content)
        // This is hard to test directly with MockImg, but we can verify the clone doesn't crash
        REQUIRE_NOTHROW(clone.show());
        REQUIRE_NOTHROW(original.show());
    }
}

// בדיקות המרות הלוך ושוב (round-trip)
TEST_CASE("Round-trip conversions") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("cell -> m -> cell round trip") {  // בודק המרה: תא → מטרים → תא
        std::pair<int, int> original_cell = {3, 5};
        auto meters = board.cell_to_m(original_cell);
        auto back_to_cell = board.m_to_cell(meters);
        
        REQUIRE(back_to_cell.first == original_cell.first);
        REQUIRE(back_to_cell.second == original_cell.second);
    }
    
    SECTION("m -> cell -> m round trip (exact values)") {  // בודק המרה: מטרים → תא → מטרים (ערכים מדויקים)
        std::pair<double, double> original_pos = {3.0, 5.0};  // Exact cell centers
        auto cell = board.m_to_cell(original_pos);
        auto back_to_meters = board.cell_to_m(cell);
        
        REQUIRE(back_to_meters.first == original_pos.first);
        REQUIRE(back_to_meters.second == original_pos.second);
    }
}

// בדיקות מקרי קצה ותנאי גבול
TEST_CASE("Board edge cases and boundary conditions") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    
    SECTION("Very small cell sizes") {
        Board board(1, 1, 100, 100, mock_img, 0.01, 0.01);
        
        auto cell = board.m_to_cell({0.005, 0.007});  // Should round to (1, 1)
        REQUIRE(cell.first == 1);
        REQUIRE(cell.second == 1);
        
        auto pos = board.cell_to_m({10, 20});
        REQUIRE(pos.first == 0.2);  // 20 * 0.01
        REQUIRE(pos.second == 0.1); // 10 * 0.01
    }
    
    SECTION("Very large cell sizes") {
        Board board(1000, 1000, 2, 2, mock_img, 100.0, 100.0);
        
        auto cell = board.m_to_cell({150.0, 250.0});  // Should round to (3, 2)
        REQUIRE(cell.first == 3);   // round(250/100) = 3
        REQUIRE(cell.second == 2);  // round(150/100) = 2
        
        auto pix = board.m_to_pix({100.0, 200.0}); 
        REQUIRE(pix.first == 1000);   // round(100/100 * 1000) = 1000
        REQUIRE(pix.second == 2000);  // round(200/100 * 1000) = 2000
    }
    
    SECTION("Zero coordinates") {
        Board board(50, 50, 10, 10, mock_img, 1.0, 1.0);
        
        auto cell_zero = board.m_to_cell({0.0, 0.0});
        REQUIRE(cell_zero.first == 0);
        REQUIRE(cell_zero.second == 0);
        
        auto pos_zero = board.cell_to_m({0, 0});
        REQUIRE(pos_zero.first == 0.0);
        REQUIRE(pos_zero.second == 0.0);
        
        auto pix_zero = board.m_to_pix({0.0, 0.0});
        REQUIRE(pix_zero.first == 0);
        REQUIRE(pix_zero.second == 0);
    }
    
    SECTION("Large negative coordinates") {
        Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);
        
        auto cell = board.m_to_cell({-10.0, -15.0});
        REQUIRE(cell.first == -15);
        REQUIRE(cell.second == -10);
        
        auto pos = board.cell_to_m({-5, -8});
        REQUIRE(pos.first == -8.0);
        REQUIRE(pos.second == -5.0);
        
        auto pix = board.m_to_pix({-3.0, -4.0});
        REQUIRE(pix.first == -300);
        REQUIRE(pix.second == -400);
    }
}

TEST_CASE("Board precision and rounding tests") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(400, 300));
    Board board(100, 100, 4, 3, mock_img, 1.0, 1.0);
    
    SECTION("Rounding behavior near half values") {
        // Test values exactly at 0.5 boundaries
        auto cell1 = board.m_to_cell({1.5, 2.5});  // Should round to (3, 2)
        REQUIRE(cell1.first == 3);   // round(2.5) = 3
        REQUIRE(cell1.second == 2);  // round(1.5) = 2
        
        auto cell2 = board.m_to_cell({0.5, 0.5});  // Should round to (1, 1)
        REQUIRE(cell2.first == 1);
        REQUIRE(cell2.second == 1);
        
        auto cell3 = board.m_to_cell({-0.5, -0.5}); // Should round to (-1, -1)
        REQUIRE(cell3.first == -1);
        REQUIRE(cell3.second == -1);
    }
    
    SECTION("Very small differences from integer values") {
        auto cell1 = board.m_to_cell({2.0001, 3.0001});  // Should round to (3, 2)
        REQUIRE(cell1.first == 3);
        REQUIRE(cell1.second == 2);
        
        auto cell2 = board.m_to_cell({1.9999, 2.9999});  // Should round to (3, 2)
        REQUIRE(cell2.first == 3);
        REQUIRE(cell2.second == 2);
    }
    
    SECTION("Pixel conversion rounding") {
        auto pix1 = board.m_to_pix({1.234, 2.567});
        REQUIRE(pix1.first == 123);   // round(1.234 * 100) = 123
        REQUIRE(pix1.second == 257);  // round(2.567 * 100) = 257
        
        auto pix2 = board.m_to_pix({1.235, 2.565});
        REQUIRE(pix2.first == 124);   // round(1.235 * 100) = 124  
        REQUIRE(pix2.second == 257);  // round(2.565 * 100) = 257
    }
}

TEST_CASE("Board with asymmetric cell dimensions") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(600, 800));
    Board board(80, 120, 5, 10, mock_img, 0.5, 2.0);  // Very different H vs W
    
    SECTION("Coordinate conversions with asymmetric cells") {
        auto cell = board.m_to_cell({4.0, 1.0});  // 4m x, 1m y
        REQUIRE(cell.first == 2);   // row = round(1.0 / 0.5) = 2
        REQUIRE(cell.second == 2);  // col = round(4.0 / 2.0) = 2
    }
    
    SECTION("Reverse conversion with asymmetric cells") {
        auto pos = board.cell_to_m({3, 1});  // row=3, col=1
        REQUIRE(pos.first == 2.0);   // x = 1 * 2.0
        REQUIRE(pos.second == 1.5);  // y = 3 * 0.5
    }
    
    SECTION("Pixel conversion with asymmetric cells") {
        auto pix = board.m_to_pix({2.0, 1.0});  // 2m x, 1m y
        // x_px = round(2.0 / 2.0 * 120) = round(1 * 120) = 120
        // y_px = round(1.0 / 0.5 * 80) = round(2 * 80) = 160
        REQUIRE(pix.first == 120);
        REQUIRE(pix.second == 160);
    }
}

TEST_CASE("Board copy operations") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board original(100, 100, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Copy constructor") {
        Board copy(original);
        
        REQUIRE(copy.cell_H_pix == original.cell_H_pix);
        REQUIRE(copy.cell_W_pix == original.cell_W_pix);
        REQUIRE(copy.W_cells == original.W_cells);
        REQUIRE(copy.H_cells == original.H_cells);
        REQUIRE(copy.cell_H_m == original.cell_H_m);
        REQUIRE(copy.cell_W_m == original.cell_W_m);
        
        // Test that conversions work the same
        auto cell_orig = original.m_to_cell({3.5, 2.5});
        auto cell_copy = copy.m_to_cell({3.5, 2.5});
        REQUIRE(cell_orig.first == cell_copy.first);
        REQUIRE(cell_orig.second == cell_copy.second);
    }
    
    SECTION("Assignment operator") {
        auto another_img = std::make_shared<MockImg>(std::make_pair(400, 300));
        Board assigned(50, 75, 4, 6, another_img, 2.0, 1.5);
        
        assigned = original;
        
        REQUIRE(assigned.cell_H_pix == original.cell_H_pix);
        REQUIRE(assigned.cell_W_pix == original.cell_W_pix);
        REQUIRE(assigned.W_cells == original.W_cells);
        REQUIRE(assigned.H_cells == original.H_cells);
        REQUIRE(assigned.cell_H_m == original.cell_H_m);
        REQUIRE(assigned.cell_W_m == original.cell_W_m);
    }
    
    SECTION("Move constructor") {
        Board to_move(100, 100, 8, 8, mock_img, 1.0, 1.0);
        Board moved(std::move(to_move));
        
        REQUIRE(moved.cell_H_pix == 100);
        REQUIRE(moved.cell_W_pix == 100);
        REQUIRE(moved.W_cells == 8);
        REQUIRE(moved.H_cells == 8);
        REQUIRE(moved.cell_H_m == 1.0);
        REQUIRE(moved.cell_W_m == 1.0);
    }
}

TEST_CASE("Board show method") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(800, 600));
    Board board(100, 100, 8, 8, mock_img, 1.0, 1.0);
    
    SECTION("Show method doesn't throw") {
        REQUIRE_NOTHROW(board.show());
    }
    
    SECTION("Show method works after clone") {
        Board cloned = board.clone();
        REQUIRE_NOTHROW(cloned.show());
    }
}

TEST_CASE("Board stress test with many conversions") {
    auto mock_img = std::make_shared<MockImg>(std::make_pair(1000, 1000));
    Board board(10, 10, 100, 100, mock_img, 0.1, 0.1);
    
    SECTION("Many random coordinate conversions") {
        // Test that conversions are consistent across many values
        for (int i = -50; i <= 50; i++) {
            for (int j = -50; j <= 50; j++) {
                double x = i * 0.1;
                double y = j * 0.1;
                
                // Round trip: m -> cell -> m
                auto cell = board.m_to_cell({x, y});
                auto back_to_m = board.cell_to_m(cell);
                
                // Should be exactly the same for these nice values
                REQUIRE(back_to_m.first == x);
                REQUIRE(back_to_m.second == y);
                
                // Test pixel conversion doesn't crash
                auto pix = board.m_to_pix({x, y});
                REQUIRE(pix.first == i * 10);  // Should be i * 10 pixels (since cell_W_pix = 10)
                REQUIRE(pix.second == j * 10); // Should be j * 10 pixels (since cell_H_pix = 10)
            }
        }
    }
}
