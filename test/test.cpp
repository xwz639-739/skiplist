// test/test.cpp - 跳表测试套件
#include <iostream>
#include <gtest/gtest.h>
#include "skipList.h"
#include <chrono>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <thread>
#include <random>

using namespace std;
using namespace SL;

// =============================================
// 基础功能测试
// =============================================

// 基础插入和查找测试
TEST(SkipListTest, BasicInsertionTest)
{
    SkipList<int> skiplist;
    skiplist.insert(10);
    EXPECT_TRUE(skiplist.search(10));
    EXPECT_FALSE(skiplist.search(20));
    EXPECT_TRUE(skiplist.update(10, 20));
    EXPECT_TRUE(skiplist.search(20));
    EXPECT_TRUE(skiplist.remove(20));
    EXPECT_FALSE(skiplist.search(20));
}

// =============================================
// 整数类型测试
// =============================================

// 整数压力测试
TEST(SkipListTest, IntStressTest)
{
    SkipList<int> skiplist;
    const int NUM_ELEMENTS = 10000;

    // 批量插入
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplist.insert(i);
    }

    // 验证所有元素都存在
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        EXPECT_TRUE(skiplist.search(i)) << "元素 " << i << " 应该存在但未找到";
    }

    // 随机查找
    for (int i = 0; i < 1000; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplist.search(random_value)) << "随机元素 " << random_value << " 应该存在";
    }

    // 批量更新
    for (int i = 0; i < NUM_ELEMENTS; i += 2)
    {
        EXPECT_TRUE(skiplist.update(i, i + NUM_ELEMENTS));
    }

    // 批量删除
    for (int i = 1; i < NUM_ELEMENTS; i += 2)
    {
        EXPECT_TRUE(skiplist.remove(i));
    }
}

// 整数性能测试
TEST(SkipListTest, IntPerformanceTest)
{
    SkipList<int> skiplist;
    const int NUM_OPERATIONS = 5000;

    // 测量插入性能
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        skiplist.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 测量查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        EXPECT_TRUE(skiplist.search(i));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "整数测试 - 插入 " << NUM_OPERATIONS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "整数测试 - 查找 " << NUM_OPERATIONS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
}

// =============================================
// 字符串类型测试
// =============================================

// 字符串基础测试
TEST(SkipListTest, StringBasicTest)
{
    SkipList<std::string> skiplist;
    skiplist.insert("hello");
    skiplist.insert("world");

    EXPECT_TRUE(skiplist.search("hello"));
    EXPECT_TRUE(skiplist.search("world"));
    EXPECT_FALSE(skiplist.search("test"));

    EXPECT_TRUE(skiplist.update("hello", "updated"));
    EXPECT_TRUE(skiplist.search("updated"));
    EXPECT_FALSE(skiplist.search("hello"));
}

// 字符串性能测试（基于数据比较）
TEST(SkipListTest, StringPerformanceTest)
{
    SkipList<std::string> skiplist;
    // 默认使用数据比较（sortBaseOnHash = false）

    const int NUM_ELEMENTS = 50000;
    const int NUM_RANDOM_OPS = 10000;

    std::cout << "\n=== 字符串性能测试开始（基于数据比较） ===" << std::endl;

    // 预先生成字符串
    std::vector<std::string> strings;
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        strings.push_back("key_" + std::to_string(i));
    }

    // 插入性能
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplist.insert(strings[i]);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplist.search(strings[random_value]));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "字符串测试（数据比较） - 插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "字符串测试（数据比较） - 查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration.count() + search_duration.count()) * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS)) << " μs" << std::endl;
}

// 字符串极限性能测试（基于哈希比较）
TEST(SkipListTest, StringExtremePerformanceTest)
{
    SkipList<std::string> skiplist;
    skiplist.setSortBaseOnHash(true); // 使用哈希比较

    const int NUM_ELEMENTS = 50000;
    const int NUM_RANDOM_OPS = 10000;

    std::cout << "\n=== 字符串极限性能测试开始（基于哈希比较） ===" << std::endl;

    // 预先生成字符串
    std::vector<std::string> strings;
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        strings.push_back("key_" + std::to_string(i));
    }

    // 插入性能
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplist.insert(strings[i]);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplist.search(strings[random_value]));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "字符串测试（哈希比较） - 插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "字符串测试（哈希比较） - 查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration.count() + search_duration.count()) * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS)) << " μs" << std::endl;
}

TEST(SkipListTest, ExtremePerformanceTest_Hash)
{
    SkipList<int> skiplist;
    skiplist.setSortBaseOnHash(true);
    const int NUM_ELEMENTS = 100000;
    const int NUM_RANDOM_OPS = 50000;

    std::cout << "\n=== 极限性能测试开始（基于哈希比较） ===" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplist.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "平均每次插入耗时: " << (insert_duration.count() * 1000.0 / NUM_ELEMENTS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplist.search(random_value));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto verify_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "抽样验证1000个元素耗时: " << verify_duration.count() << " ms" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        bool find = skiplist.search(random_value);
        EXPECT_TRUE(find);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次查找耗时: " << (search_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int old_value = rand() % NUM_ELEMENTS;
        int new_value = NUM_ELEMENTS + old_value;
        skiplist.update(old_value, new_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto update_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机更新 " << NUM_RANDOM_OPS << " 个元素耗时: " << update_duration.count() << " ms" << std::endl;
    std::cout << "平均每次更新耗时: " << (update_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = NUM_ELEMENTS + (rand() % NUM_ELEMENTS);
        skiplist.remove(random_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto remove_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机删除 " << NUM_RANDOM_OPS << " 个元素耗时: " << remove_duration.count() << " ms" << std::endl;
    std::cout << "平均每次删除耗时: " << (remove_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    const int EXTRA_ELEMENTS = 50000;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EXTRA_ELEMENTS; i++)
    {
        skiplist.insert(NUM_ELEMENTS * 2 + i);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto extra_insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "额外插入 " << EXTRA_ELEMENTS << " 个元素耗时: " << extra_insert_duration.count() << " ms" << std::endl;

    int final_element_count = 0;
    for (int i = NUM_ELEMENTS * 2; i < NUM_ELEMENTS * 2 + EXTRA_ELEMENTS; i++)
    {
        if (skiplist.search(i))
        {
            final_element_count++;
        }
    }

    std::cout << "最终验证：" << final_element_count << " 个元素存在（应该是 " << EXTRA_ELEMENTS << " 个）" << std::endl;
    EXPECT_EQ(final_element_count, EXTRA_ELEMENTS);

    std::cout << "\n=== 性能总结 ===" << std::endl;
    std::cout << "总操作数: " << (NUM_ELEMENTS + NUM_RANDOM_OPS * 3 + EXTRA_ELEMENTS) << std::endl;
    std::cout << "总耗时: " << (insert_duration + search_duration + update_duration + remove_duration + extra_insert_duration).count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration + search_duration + update_duration + remove_duration + extra_insert_duration).count() * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS * 3 + EXTRA_ELEMENTS)) << " μs" << std::endl;
    std::cout << "=== 极限性能测试结束 ===\n"
              << std::endl;
}

TEST(SkipListTest, ExtremePerformanceTest_Data)
{
    SkipList<int> skiplist;
    skiplist.setSortBaseOnHash(false);
    const int NUM_ELEMENTS = 100000;
    const int NUM_RANDOM_OPS = 50000;

    std::cout << "\n=== 极限性能测试开始（基于数据比较） ===" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplist.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "平均每次插入耗时: " << (insert_duration.count() * 1000.0 / NUM_ELEMENTS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        bool find = skiplist.search(random_value);
        EXPECT_TRUE(find);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次查找耗时: " << (search_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int old_value = rand() % NUM_ELEMENTS;
        int new_value = NUM_ELEMENTS + old_value;
        skiplist.update(old_value, new_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto update_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机更新 " << NUM_RANDOM_OPS << " 个元素耗时: " << update_duration.count() << " ms" << std::endl;
    std::cout << "平均每次更新耗时: " << (update_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = NUM_ELEMENTS + (rand() % NUM_ELEMENTS);
        skiplist.remove(random_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto remove_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机删除 " << NUM_RANDOM_OPS << " 个元素耗时: " << remove_duration.count() << " ms" << std::endl;
    std::cout << "平均每次删除耗时: " << (remove_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    const int EXTRA_ELEMENTS = 50000;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EXTRA_ELEMENTS; i++)
    {
        skiplist.insert(NUM_ELEMENTS * 2 + i);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto extra_insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "额外插入 " << EXTRA_ELEMENTS << " 个元素耗时: " << extra_insert_duration.count() << " ms" << std::endl;

    int final_element_count = 0;
    for (int i = NUM_ELEMENTS * 2; i < NUM_ELEMENTS * 2 + EXTRA_ELEMENTS; i++)
    {
        if (skiplist.search(i))
        {
            final_element_count++;
        }
    }

    std::cout << "最终验证：" << final_element_count << " 个元素存在（应该是 " << EXTRA_ELEMENTS << " 个）" << std::endl;
    EXPECT_EQ(final_element_count, EXTRA_ELEMENTS);

    std::cout << "\n=== 性能总结 ===" << std::endl;
    std::cout << "总操作数: " << (NUM_ELEMENTS + NUM_RANDOM_OPS * 3 + EXTRA_ELEMENTS) << std::endl;
    std::cout << "总耗时: " << (insert_duration + search_duration + update_duration + remove_duration + extra_insert_duration).count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration + search_duration + update_duration + remove_duration + extra_insert_duration).count() * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS * 3 + EXTRA_ELEMENTS)) << " μs" << std::endl;
    std::cout << "=== 极限性能测试结束 ===\n"
              << std::endl;
}

// 多线程并发测试
TEST(SkipListTest, ConcurrentOperationsTest)
{
    const int NUM_THREADS = 4;
    const int OPERATIONS_PER_THREAD = 2500;
    const int TOTAL_ELEMENTS = NUM_THREADS * OPERATIONS_PER_THREAD;

    SkipList<int> skiplist;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> failure_count{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    // 并发插入测试
    auto insert_worker = [&](int thread_id)
    {
        for (int i = 0; i < OPERATIONS_PER_THREAD; i++)
        {
            int value = thread_id * OPERATIONS_PER_THREAD + i;
            skiplist.insertl(value);

            // 验证插入成功
            if (skiplist.searchl(value))
            {
                success_count++;
            }
            else
            {
                failure_count++;
            }
        }
    };

    // 启动插入线程
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back(insert_worker, i);
    }

    // 等待所有插入线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    threads.clear();

    // 并发查找测试
    auto search_worker = [&](int thread_id)
    {
        for (int i = 0; i < OPERATIONS_PER_THREAD; i++)
        {
            int value = rand() % TOTAL_ELEMENTS;
            bool find = skiplist.searchl(value);
            if (find)
            {
                success_count++;
            }
        }
    };

    // 启动查找线程
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back(search_worker, i);
    }

    // 等待所有查找线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    threads.clear();

    // 并发更新测试
    auto update_worker = [&](int thread_id)
    {
        for (int i = 0; i < OPERATIONS_PER_THREAD / 2; i++)
        {
            int old_value = rand() % TOTAL_ELEMENTS;
            int new_value = TOTAL_ELEMENTS + thread_id * OPERATIONS_PER_THREAD / 2 + i;

            if (skiplist.updatel(old_value, new_value))
            {
                success_count++;
            }
        }
    };

    // 启动更新线程
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back(update_worker, i);
    }

    // 等待所有更新线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    threads.clear();

    // 并发删除测试
    auto remove_worker = [&](int thread_id)
    {
        for (int i = 0; i < OPERATIONS_PER_THREAD / 2; i++)
        {
            int value = rand() % (TOTAL_ELEMENTS * 2); // 包括更新后的数据
            if (skiplist.removel(value))
            {
                success_count++;
            }
        }
    };

    // 启动删除线程
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back(remove_worker, i);
    }

    // 等待所有删除线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 验证测试结果
    std::cout << "\n=== 多线程并发测试 ===" << std::endl;
    std::cout << "线程数: " << NUM_THREADS << std::endl;
    std::cout << "每个线程操作数: " << OPERATIONS_PER_THREAD << std::endl;
    std::cout << "总操作数: " << (NUM_THREADS * OPERATIONS_PER_THREAD * 3) << std::endl;
    std::cout << "成功操作数: " << success_count.load() << std::endl;
    std::cout << "失败操作数: " << failure_count.load() << std::endl;
    std::cout << "总耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << (duration.count() * 1000.0 / (NUM_THREADS * OPERATIONS_PER_THREAD * 3)) << " μs" << std::endl;

    // 基本验证：成功操作数应该大于0
    EXPECT_GT(success_count.load(), 0) << "应该有一些成功的并发操作";
}

// 多线程压力测试
TEST(SkipListTest, ConcurrentStressTest)
{
    const int NUM_THREADS = 8;
    const int OPERATIONS_PER_THREAD = 5000;
    const int VALUE_RANGE = 10000;

    SkipList<int> skiplist;
    std::vector<std::thread> threads;
    std::atomic<int> total_operations{0};
    std::atomic<int> conflicts{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    // 混合操作工作线程
    auto mixed_worker = [&](int thread_id)
    {
        std::mt19937 rng(thread_id);
        std::uniform_int_distribution<int> value_dist(0, VALUE_RANGE - 1);
        std::uniform_int_distribution<int> op_dist(0, 3); // 0:插入, 1:查找, 2:更新, 3:删除

        for (int i = 0; i < OPERATIONS_PER_THREAD; i++)
        {
            int operation = op_dist(rng);
            int value = value_dist(rng);

            switch (operation)
            {
            case 0: // 插入
                skiplist.insertl(value);
                break;
            case 1: // 查找
                skiplist.searchl(value);
                break;
            case 2: // 更新
            {
                int new_value = VALUE_RANGE + thread_id * OPERATIONS_PER_THREAD + i;
                skiplist.updatel(value, new_value);
            }
            break;
            case 3: // 删除
                skiplist.removel(value);
                break;
            }

            total_operations++;
        }
    };

    // 启动所有工作线程
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back(mixed_worker, i);
    }

    // 等待所有线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== 多线程压力测试 ===" << std::endl;
    std::cout << "线程数: " << NUM_THREADS << std::endl;
    std::cout << "每个线程操作数: " << OPERATIONS_PER_THREAD << std::endl;
    std::cout << "总操作数: " << total_operations.load() << std::endl;
    std::cout << "冲突数: " << conflicts.load() << std::endl;
    std::cout << "总耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << (duration.count() * 1000.0 / total_operations.load()) << " μs" << std::endl;
    std::cout << "操作吞吐量: " << (total_operations.load() * 1000.0 / duration.count()) << " ops/sec" << std::endl;

    // 验证最终状态
    int final_count = 0;
    for (int i = 0; i < VALUE_RANGE * 2; i++)
    {
        if (skiplist.searchl(i))
        {
            final_count++;
        }
    }

    std::cout << "最终数据量: " << final_count << " 个元素" << std::endl;
    EXPECT_GT(final_count, 0) << "跳表中应该还有一些元素";
}

// 大规模随机数据QPS测试
TEST(SkipListTest, LargeScaleQPSTest)
{
    const std::vector<int> SCALES = {100000, 500000, 1000000}; // 10w, 50w, 100w

    for (int scale : SCALES)
    {
        SkipList<int> skiplist;
        std::vector<int> test_data;
        test_data.reserve(scale);

        // 生成随机测试数据
        std::cout << "\n=== 生成 " << scale << " 个随机测试数据 ===" << std::endl;
        auto gen_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < scale; i++)
        {
            test_data.push_back(rand() % (scale * 2)); // 扩大范围减少重复
        }

        auto gen_end = std::chrono::high_resolution_clock::now();
        auto gen_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gen_end - gen_start);
        std::cout << "数据生成耗时: " << gen_duration.count() << " ms" << std::endl;

        // 测试1: 大规模随机插入（写QPS测试）
        std::cout << "\n=== 写QPS测试 - " << scale << " 个随机插入 ===" << std::endl;
        auto write_start = std::chrono::high_resolution_clock::now();

        int write_success = 0;
        for (int i = 0; i < scale; i++)
        {
            skiplist.insert(test_data[i]);
            write_success++;
        }

        auto write_end = std::chrono::high_resolution_clock::now();
        auto write_duration = std::chrono::duration_cast<std::chrono::milliseconds>(write_end - write_start);
        double write_qps = (scale * 1000.0) / write_duration.count();

        std::cout << "成功插入: " << write_success << " 个" << std::endl;
        std::cout << "插入耗时: " << write_duration.count() << " ms" << std::endl;
        std::cout << "写QPS: " << std::fixed << std::setprecision(0) << write_qps << " ops/sec" << std::endl;
        std::cout << "平均每次插入: " << std::setprecision(2) << (write_duration.count() * 1000.0 / scale) << " μs" << std::endl;

        // 测试2: 大规模随机查找（读QPS测试）
        std::cout << "\n=== 读QPS测试 - " << scale << " 个随机查找 ===" << std::endl;
        auto read_start = std::chrono::high_resolution_clock::now();

        int read_success = 0;
        for (int i = 0; i < scale; i++)
        {
            if (skiplist.search(test_data[i]))
            {
                read_success++;
            }
        }

        auto read_end = std::chrono::high_resolution_clock::now();
        auto read_duration = std::chrono::duration_cast<std::chrono::milliseconds>(read_end - read_start);
        double read_qps = (scale * 1000.0) / read_duration.count();

        std::cout << "成功查找: " << read_success << " 个" << std::endl;
        std::cout << "查找耗时: " << read_duration.count() << " ms" << std::endl;
        std::cout << "读QPS: " << std::fixed << std::setprecision(0) << read_qps << " ops/sec" << std::endl;
        std::cout << "平均每次查找: " << std::setprecision(2) << (read_duration.count() * 1000.0 / scale) << " μs" << std::endl;

        // 测试3: 混合读写QPS测试（读写比例 7:3）
        std::cout << "\n=== 混合QPS测试 - 读写比例 7:3 ===" << std::endl;
        const int MIX_OPS = scale / 10; // 减少测试规模避免太长
        const int READ_OPS = MIX_OPS * 7 / 10;
        const int WRITE_OPS = MIX_OPS * 3 / 10;

        auto mix_start = std::chrono::high_resolution_clock::now();

        int mix_read_success = 0;
        int mix_write_success = 0;

        for (int i = 0; i < MIX_OPS; i++)
        {
            if (i % 10 < 7) // 70% 读操作
            {
                int idx = rand() % scale;
                if (skiplist.search(test_data[idx]))
                {
                    mix_read_success++;
                }
            }
            else // 30% 写操作
            {
                int new_value = rand() % (scale * 2) + scale; // 新数据
                skiplist.insert(new_value);
                mix_write_success++;
            }
        }

        auto mix_end = std::chrono::high_resolution_clock::now();
        auto mix_duration = std::chrono::duration_cast<std::chrono::milliseconds>(mix_end - mix_start);
        double mix_qps = (MIX_OPS * 1000.0) / mix_duration.count();

        std::cout << "混合操作总数: " << MIX_OPS << " (读: " << READ_OPS << ", 写: " << WRITE_OPS << ")" << std::endl;
        std::cout << "读成功: " << mix_read_success << " 个" << std::endl;
        std::cout << "写成功: " << mix_write_success << " 个" << std::endl;
        std::cout << "混合耗时: " << mix_duration.count() << " ms" << std::endl;
        std::cout << "混合QPS: " << std::fixed << std::setprecision(0) << mix_qps << " ops/sec" << std::endl;
        std::cout << "平均每次操作: " << std::setprecision(2) << (mix_duration.count() * 1000.0 / MIX_OPS) << " μs" << std::endl;

        // 性能总结
        std::cout << "\n=== " << scale << " 规模性能总结 ===" << std::endl;
        std::cout << "写QPS: " << std::fixed << std::setprecision(0) << write_qps << " ops/sec" << std::endl;
        std::cout << "读QPS: " << std::fixed << std::setprecision(0) << read_qps << " ops/sec" << std::endl;
        std::cout << "混合QPS: " << std::fixed << std::setprecision(0) << mix_qps << " ops/sec" << std::endl;
        std::cout << "读/写性能比: " << std::setprecision(2) << (read_qps / write_qps) << " : 1" << std::endl;

        // 验证数据完整性
        int final_count = 0;
        for (int i = 0; i < 100; i++) // 抽样验证
        {
            if (skiplist.search(test_data[rand() % scale]))
            {
                final_count++;
            }
        }
        std::cout << "数据完整性抽样验证: " << final_count << "/100 个数据存在" << std::endl;

        // 清理跳表准备下一轮测试
        // 注意：大规模删除会很耗时，这里跳过删除以节省时间
    }

    std::cout << "\n=== 大规模QPS测试完成 ===" << std::endl;
}

// 如果使用gtest_main库，则不需要定义main函数
// 否则保留以下main函数
#ifndef GTEST_IS_MAIN_
int main(int argc, char **argv)
{
    cout << "Running tests..." << endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif