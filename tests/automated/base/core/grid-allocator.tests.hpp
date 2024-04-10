#pragma once

#include "tundra/core/log.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/grid-allocator.hpp>

namespace td::grid_allocator_tests {

    TD_TEST("core/grid-allocator/basic") {
        GridAllocator allocator(1024, 1024);

        GridAllocator::Result result;
        
        result = allocator.allocate(64, 64);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(32, 32);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(32, 32);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(8, 8);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(8, 8);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(256, 256);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        result = allocator.allocate(128, 128);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_NOT_EQUAL(result.position, Vec2<int16>{0});

        uint32 expected_allocated_size = 64 * 64 + 32 * 32 * 2 + 8 * 8 * 2 + 256 * 256 + 128 * 128;
        TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), expected_allocated_size);
        TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), 7U);        
    }

    TD_TEST("core/grid-allocator/allocate-full-width") {
        GridAllocator allocator(1024, 1024);
        GridAllocator::Result result = allocator.allocate(1024, 512);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(allocator.get_free_rects().get_size(), 1U);
    }

    TD_TEST("core/grid-allocator/allocate-full-height") {
        GridAllocator allocator(1024, 1024);
        GridAllocator::Result result = allocator.allocate(512, 1024);        
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(allocator.get_free_rects().get_size(), 1U);
    }

    TD_TEST("core/grid-allocator/allocate-full") {

        GridAllocator allocator(1024, 1024);

        GridAllocator::Result result = allocator.allocate(1024, 1024);        

        Vec2<int16> expected_position{ 0, 0 };

        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(result.position, expected_position);
        TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), 1024U*1024U);
        TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), 1U);
        TD_TEST_ASSERT_EQUAL(allocator.get_free_rects().get_size(), 0U);
    }

    TD_TEST("core/grid-allocator/full-when-allocating") {

        GridAllocator allocator(1024, 1024);
        static_cast<void>(allocator.allocate(512, 512));
        GridAllocator::Result result = allocator.allocate(768, 768);        

        TD_TEST_ASSERT_EQUAL(result.success, false);
        TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), 512U*512U);
        TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), 1U);
    }

    TD_TEST("core/grid-allocator/full-with-many-small") {

        GridAllocator allocator(1024, 1024);

        for( uint32 i = 1; i <= 32U * 32U; i++) {
            GridAllocator::Result result = allocator.allocate(32, 32);        
            TD_TEST_ASSERT_EQUAL(result.success, true);
            TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), 32 * 32 * i);
            TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), i );
        }

        GridAllocator::Result failing_result = allocator.allocate(32, 32);        
        TD_TEST_ASSERT_EQUAL(failing_result.success, false);
    }

    TD_TEST("core/grid-allocator/x-aligned") {

        GridAllocator allocator(1024, 1024);

        static_cast<void>(allocator.allocate(1, 1));

        GridAllocator::Result result = allocator.allocate(32, 32, 3);
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(result.position.x % (1 << 3), 0);
    }

    TD_TEST("core/grid-allocator/y-aligned") {

        GridAllocator allocator(1024, 1024);

        static_cast<void>(allocator.allocate(1, 1));

        GridAllocator::Result result = allocator.allocate(32, 32, 0, 3);
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(result.position.y % (1 << 3), 0);
    }

    TD_TEST("core/grid-allocator/xy-aligned") {

        GridAllocator allocator(1024, 1024);

        static_cast<void>(allocator.allocate(1, 1));

        GridAllocator::Result result = allocator.allocate(32, 32, 3, 3);
        TD_TEST_ASSERT_EQUAL(result.success, true);
        TD_TEST_ASSERT_EQUAL(result.position.x % (1 << 3), 0);
        TD_TEST_ASSERT_EQUAL(result.position.y % (1 << 3), 0);
    }

    TD_TEST("core/grid-allocator/x-aligned-full-one-dimension") {

        GridAllocator allocator(1024, 24);

        for( uint32 i = 1; i <= 32U; i++) {
            
            GridAllocator::Result result = allocator.allocate(24, 24, 5, 5);        
            TD_TEST_ASSERT_EQUAL(result.success, true);
            TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), 24 * 24 + 32 * 24 * (i - 1));
            TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), i );
            TD_TEST_ASSERT_EQUAL(result.position.x % (1 << 5), 0);
            TD_TEST_ASSERT_EQUAL(result.position.y % (1 << 5), 0);
        }

        GridAllocator::Result failing_result = allocator.allocate(24, 24, 4, 4);        
        TD_TEST_ASSERT_EQUAL(failing_result.success, false);
        
    }

    TD_TEST("core/grid-allocator/xy-aligned-full") {

        GridAllocator allocator(1024, 1024);
        
        uint32 expected_allocated_bytes = 0;

        for( uint32 row = 1; row <= 32U; row++) {

            for( uint32 column = 1; column <= 32U; column++ ) {
                GridAllocator::Result result = allocator.allocate(24, 24, 5, 5);        

                TD_TEST_ASSERT_EQUAL(result.position.x % (1 << 5), 0);
                TD_TEST_ASSERT_EQUAL(result.position.y % (1 << 5), 0);
                TD_TEST_ASSERT_EQUAL(allocator.get_num_allocations(), 32 * (row - 1) + column );
                TD_TEST_ASSERT_EQUAL(result.success, true);

                if( row == 1 ) {
                    if( column == 1 ) expected_allocated_bytes += 24 * 24;
                    else expected_allocated_bytes += 32 * 24;
                }
                else if( column == 1 ) {
                    expected_allocated_bytes += 24 * 32;
                }
                else {
                    expected_allocated_bytes += 32 * 32;
                }

                TD_TEST_ASSERT_EQUAL(allocator.get_num_bytes_allocated(), expected_allocated_bytes);
            }
        }

        GridAllocator::Result failing_result = allocator.allocate(24, 24, 4, 4);        
        TD_TEST_ASSERT_EQUAL(failing_result.success, false);
        
    }


}