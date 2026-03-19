// test/test.cpp - 跳表测试套件
#include <iostream>
#include <gtest/gtest.h>
#include "skipListV.hpp"
#include "skipListKV.hpp"
#include <chrono>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <thread>
#include <random>
#include <fstream>
#include <cstdio> // for std::remove

using namespace std;
using namespace SL;

// =============================================
// 整数类型测试
// =============================================

// 整数压力测试
TEST(SkipListVTest, IntStressTest)
{
    SkipListV<int> skiplistv;
    const int NUM_ELEMENTS = 10000;

    // 批量插入
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplistv.insert(i);
    }

    // 验证所有元素都存在
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        EXPECT_TRUE(skiplistv.search(i)) << "元素 " << i << " 应该存在但未找到";
    }

    // 随机查找
    for (int i = 0; i < 1000; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplistv.search(random_value)) << "随机元素 " << random_value << " 应该存在";
    }

    // 批量更新
    for (int i = 0; i < NUM_ELEMENTS; i += 2)
    {
        EXPECT_TRUE(skiplistv.update(i, i + NUM_ELEMENTS));
    }

    // 批量删除
    for (int i = 1; i < NUM_ELEMENTS; i += 2)
    {
        EXPECT_TRUE(skiplistv.remove(i));
    }
}

// 整数性能测试
TEST(SkipListVTest, IntPerformanceTest)
{
    SkipListV<int> skiplistv;
    const int NUM_OPERATIONS = 5000;

    // 测量插入性能
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        skiplistv.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 测量查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        EXPECT_TRUE(skiplistv.search(i));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "整数测试 - 插入 " << NUM_OPERATIONS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "整数测试 - 查找 " << NUM_OPERATIONS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
}

// 字符串性能测试（基于数据比较）
TEST(SkipListVTest, StringPerformanceTest)
{
    SkipListV<std::string> skiplistv;
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
        skiplistv.insert(strings[i]);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplistv.search(strings[random_value]));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "字符串测试（数据比较） - 插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "字符串测试（数据比较） - 查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration.count() + search_duration.count()) * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS)) << " μs" << std::endl;
}

// 字符串极限性能测试（基于哈希比较）
TEST(SkipListVTest, StringExtremePerformanceTest)
{
    SkipListV<std::string> skiplistv;
    skiplistv.setSortBaseOnHash(true); // 使用哈希比较

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
        skiplistv.insert(strings[i]);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 查找性能
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplistv.search(strings[random_value]));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto search_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "字符串测试（哈希比较） - 插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "字符串测试（哈希比较） - 查找 " << NUM_RANDOM_OPS << " 个元素耗时: " << search_duration.count() << " ms" << std::endl;
    std::cout << "平均每次操作耗时: " << ((insert_duration.count() + search_duration.count()) * 1000.0 / (NUM_ELEMENTS + NUM_RANDOM_OPS)) << " μs" << std::endl;
}

TEST(SkipListVTest, ExtremePerformanceTest_Hash)
{
    SkipListV<int> skiplistv;
    skiplistv.setSortBaseOnHash(true);
    const int NUM_ELEMENTS = 100000;
    const int NUM_RANDOM_OPS = 50000;

    std::cout << "\n=== 极限性能测试开始（基于哈希比较） ===" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplistv.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "平均每次插入耗时: " << (insert_duration.count() * 1000.0 / NUM_ELEMENTS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplistv.search(random_value));
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto verify_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "抽样验证1000个元素耗时: " << verify_duration.count() << " ms" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        bool find = skiplistv.search(random_value);
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
        skiplistv.update(old_value, new_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto update_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机更新 " << NUM_RANDOM_OPS << " 个元素耗时: " << update_duration.count() << " ms" << std::endl;
    std::cout << "平均每次更新耗时: " << (update_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = NUM_ELEMENTS + (rand() % NUM_ELEMENTS);
        skiplistv.remove(random_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto remove_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机删除 " << NUM_RANDOM_OPS << " 个元素耗时: " << remove_duration.count() << " ms" << std::endl;
    std::cout << "平均每次删除耗时: " << (remove_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    const int EXTRA_ELEMENTS = 50000;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EXTRA_ELEMENTS; i++)
    {
        skiplistv.insert(NUM_ELEMENTS * 2 + i);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto extra_insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "额外插入 " << EXTRA_ELEMENTS << " 个元素耗时: " << extra_insert_duration.count() << " ms" << std::endl;

    int final_element_count = 0;
    for (int i = NUM_ELEMENTS * 2; i < NUM_ELEMENTS * 2 + EXTRA_ELEMENTS; i++)
    {
        if (skiplistv.search(i))
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

TEST(SkipListVTest, ExtremePerformanceTest_Data)
{
    SkipListV<int> skiplistv;
    skiplistv.setSortBaseOnHash(false);
    const int NUM_ELEMENTS = 100000;
    const int NUM_RANDOM_OPS = 50000;

    std::cout << "\n=== 极限性能测试开始（基于数据比较） ===" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplistv.insert(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "插入 " << NUM_ELEMENTS << " 个元素耗时: " << insert_duration.count() << " ms" << std::endl;
    std::cout << "平均每次插入耗时: " << (insert_duration.count() * 1000.0 / NUM_ELEMENTS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = rand() % NUM_ELEMENTS;
        bool find = skiplistv.search(random_value);
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
        skiplistv.update(old_value, new_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto update_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机更新 " << NUM_RANDOM_OPS << " 个元素耗时: " << update_duration.count() << " ms" << std::endl;
    std::cout << "平均每次更新耗时: " << (update_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_RANDOM_OPS; i++)
    {
        int random_value = NUM_ELEMENTS + (rand() % NUM_ELEMENTS);
        skiplistv.remove(random_value);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto remove_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "随机删除 " << NUM_RANDOM_OPS << " 个元素耗时: " << remove_duration.count() << " ms" << std::endl;
    std::cout << "平均每次删除耗时: " << (remove_duration.count() * 1000.0 / NUM_RANDOM_OPS) << " μs" << std::endl;

    const int EXTRA_ELEMENTS = 50000;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < EXTRA_ELEMENTS; i++)
    {
        skiplistv.insert(NUM_ELEMENTS * 2 + i);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto extra_insert_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "额外插入 " << EXTRA_ELEMENTS << " 个元素耗时: " << extra_insert_duration.count() << " ms" << std::endl;

    int final_element_count = 0;
    for (int i = NUM_ELEMENTS * 2; i < NUM_ELEMENTS * 2 + EXTRA_ELEMENTS; i++)
    {
        if (skiplistv.search(i))
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

// 多线程压力测试
TEST(SkipListVTest, ConcurrentStressTest)
{
    const int NUM_THREADS = 8;
    const int OPERATIONS_PER_THREAD = 5000;
    const int VALUE_RANGE = 10000;

    SkipListV<int> skiplistv;
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
                skiplistv.insertl(value);
                break;
            case 1: // 查找
                skiplistv.searchl(value);
                break;
            case 2: // 更新
            {
                int new_value = VALUE_RANGE + thread_id * OPERATIONS_PER_THREAD + i;
                skiplistv.updatel(value, new_value);
            }
            break;
            case 3: // 删除
                skiplistv.removel(value);
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
        if (skiplistv.searchl(i))
        {
            final_count++;
        }
    }

    std::cout << "最终数据量: " << final_count << " 个元素" << std::endl;
    EXPECT_GT(final_count, 0) << "跳表中应该还有一些元素";
}

// 大规模随机数据QPS测试
TEST(SkipListVTest, LargeScaleQPSTest)
{
    const std::vector<int> SCALES = {100000, 500000, 1000000}; // 10w, 50w, 100w

    for (int scale : SCALES)
    {
        SkipListV<int> skiplistv(18);
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
            skiplistv.insert(test_data[i]);
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
            if (skiplistv.search(test_data[i]))
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

        // 性能总结
        std::cout << "\n=== " << scale << " 规模性能总结 ===" << std::endl;
        std::cout << "写QPS: " << std::fixed << std::setprecision(0) << write_qps << " ops/sec" << std::endl;
        std::cout << "读QPS: " << std::fixed << std::setprecision(0) << read_qps << " ops/sec" << std::endl;
        std::cout << "读/写性能比: " << std::setprecision(2) << (read_qps / write_qps) << " : 1" << std::endl;

        // 验证数据完整性
        int final_count = 0;
        for (int i = 0; i < 100; i++) // 抽样验证
        {
            if (skiplistv.search(test_data[rand() % scale]))
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

// =============================================
// 文件持久化测试
// =============================================

// 基础文件保存和加载测试
TEST(SkipListVTest, FileSaveLoadTest)
{
    SkipListV<int> skiplistv;
    const std::string test_file = "/tmp/SkipListV_test.txt";

    // 设置存储文件路径
    skiplistv.setStorageFile(test_file);

    // 插入一些测试数据
    skiplistv.insert(10);
    skiplistv.insert(20);
    skiplistv.insert(30);
    skiplistv.insert(40);
    skiplistv.insert(50);

    // 验证原始数据
    EXPECT_TRUE(skiplistv.search(10));
    EXPECT_TRUE(skiplistv.search(20));
    EXPECT_TRUE(skiplistv.search(30));
    EXPECT_TRUE(skiplistv.search(40));
    EXPECT_TRUE(skiplistv.search(50));

    // 保存到文件
    skiplistv.saveToFile();

    // 验证文件是否正确生成
    std::ifstream file_check(test_file);
    EXPECT_TRUE(file_check.is_open()) << "文件应该被正确创建";
    file_check.close();

    // 验证文件内容
    std::ifstream content_check(test_file);
    std::string line;
    std::vector<int> file_contents;
    while (std::getline(content_check, line))
    {
        if (!line.empty())
        {
            file_contents.push_back(std::stoi(line));
        }
    }
    content_check.close();

    // 验证文件中的数据
    EXPECT_EQ(file_contents.size(), 5) << "文件应该包含5个数据元素";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), 10), file_contents.end()) << "文件应该包含10";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), 20), file_contents.end()) << "文件应该包含20";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), 30), file_contents.end()) << "文件应该包含30";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), 40), file_contents.end()) << "文件应该包含40";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), 50), file_contents.end()) << "文件应该包含50";

    // 创建新的跳表实例并加载数据
    SkipListV<int> loaded_SkipListV;
    loaded_SkipListV.setStorageFile(test_file);
    loaded_SkipListV.loadFromFile();

    // 验证加载的数据
    EXPECT_TRUE(loaded_SkipListV.search(10)) << "加载的跳表应该包含10";
    EXPECT_TRUE(loaded_SkipListV.search(20)) << "加载的跳表应该包含20";
    EXPECT_TRUE(loaded_SkipListV.search(30)) << "加载的跳表应该包含30";
    EXPECT_TRUE(loaded_SkipListV.search(40)) << "加载的跳表应该包含40";
    EXPECT_TRUE(loaded_SkipListV.search(50)) << "加载的跳表应该包含50";
    EXPECT_FALSE(loaded_SkipListV.search(60)) << "加载的跳表不应该包含60";

    // 验证数据完整性：所有原始数据都应该在加载的跳表中
    std::vector<int> original_data = {10, 20, 30, 40, 50};
    for (int data : original_data)
    {
        EXPECT_TRUE(loaded_SkipListV.search(data)) << "数据 " << data << " 应该在加载的跳表中";
    }

    // 清理测试文件
    std::remove(test_file.c_str());

    // 验证文件已被清理
    std::ifstream cleanup_check(test_file);
    EXPECT_FALSE(cleanup_check.is_open()) << "测试文件应该已被清理";
}

// 空跳表文件操作测试
TEST(SkipListVTest, EmptyFileSaveLoadTest)
{
    SkipListV<int> skiplistv;
    const std::string test_file = "/tmp/empty_SkipListV_test.txt";

    skiplistv.setStorageFile(test_file);

    // 验证原始跳表为空
    EXPECT_FALSE(skiplistv.search(10)) << "原始跳表应该为空";
    EXPECT_FALSE(skiplistv.search(20)) << "原始跳表应该为空";

    // 保存空跳表
    skiplistv.saveToFile();

    // 验证文件是否正确生成
    std::ifstream file_check(test_file);
    EXPECT_TRUE(file_check.is_open()) << "空跳表文件应该被正确创建";
    file_check.close();

    // 验证文件内容为空或只包含换行符
    std::ifstream content_check(test_file);
    std::string line;
    int line_count = 0;
    while (std::getline(content_check, line))
    {
        if (!line.empty())
        {
            line_count++;
        }
    }
    content_check.close();

    EXPECT_EQ(line_count, 0) << "空跳表保存的文件应该不包含任何数据行";

    // 加载到新跳表
    SkipListV<int> loaded_SkipListV;
    loaded_SkipListV.setStorageFile(test_file);
    loaded_SkipListV.loadFromFile();

    // 验证加载的跳表也是空的
    EXPECT_FALSE(loaded_SkipListV.search(10)) << "加载的空跳表应该不包含10";
    EXPECT_FALSE(loaded_SkipListV.search(20)) << "加载的空跳表应该不包含20";
    EXPECT_FALSE(loaded_SkipListV.search(30)) << "加载的空跳表应该不包含30";

    // 验证多个不存在的元素
    for (int i = 0; i < 10; i++)
    {
        EXPECT_FALSE(loaded_SkipListV.search(i * 100)) << "加载的空跳表不应该包含任何数据";
    }

    // 清理测试文件
    std::remove(test_file.c_str());

    // 验证文件已被清理
    std::ifstream cleanup_check(test_file);
    EXPECT_FALSE(cleanup_check.is_open()) << "测试文件应该已被清理";
}

// 大规模数据文件持久化测试
TEST(SkipListVTest, LargeScaleFileSaveLoadTest)
{
    SkipListV<int> skiplistv;
    const std::string test_file = "/tmp/large_SkipListV_test.txt";
    const int NUM_ELEMENTS = 1000;

    skiplistv.setStorageFile(test_file);

    // 插入大量数据
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        skiplistv.insert(i * 2); // 插入偶数
    }

    // 验证原始数据
    for (int i = 0; i < 10; i++) // 抽样验证
    {
        int test_index = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(skiplistv.search(test_index * 2)) << "原始跳表应该包含数据 " << (test_index * 2);
    }

    // 保存到文件
    skiplistv.saveToFile();

    // 验证文件是否正确生成
    std::ifstream file_check(test_file);
    EXPECT_TRUE(file_check.is_open()) << "大规模数据文件应该被正确创建";
    file_check.close();

    // 验证文件内容
    std::ifstream content_check(test_file);
    std::string line;
    std::vector<int> file_contents;
    int line_count = 0;
    while (std::getline(content_check, line))
    {
        if (!line.empty())
        {
            file_contents.push_back(std::stoi(line));
            line_count++;
        }
    }
    content_check.close();

    // 验证文件中的数据量
    EXPECT_EQ(line_count, NUM_ELEMENTS) << "文件应该包含 " << NUM_ELEMENTS << " 个数据元素";

    // 验证文件中的数据正确性（抽样检查）
    for (int i = 0; i < 20; i++)
    {
        int test_value = (rand() % NUM_ELEMENTS) * 2;
        EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), test_value), file_contents.end())
            << "文件应该包含数据 " << test_value;
    }

    // 创建新的跳表实例并加载数据
    SkipListV<int> loaded_SkipListV;
    loaded_SkipListV.setStorageFile(test_file);
    loaded_SkipListV.loadFromFile();

    // 验证所有数据都正确加载
    int found_count = 0;
    int not_found_count = 0;

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        if (loaded_SkipListV.search(i * 2))
        {
            found_count++;
        }
        else
        {
            not_found_count++;
        }

        // 验证不存在的奇数
        EXPECT_FALSE(loaded_SkipListV.search(i * 2 + 1)) << "加载的跳表不应该包含奇数 " << (i * 2 + 1);
    }

    // 验证数据完整性统计
    EXPECT_EQ(found_count, NUM_ELEMENTS) << "所有 " << NUM_ELEMENTS << " 个偶数数据都应该被正确加载";
    EXPECT_EQ(not_found_count, 0) << "不应该有任何偶数数据丢失";

    // 额外验证：随机抽样检查数据正确性
    for (int i = 0; i < 50; i++)
    {
        int test_index = rand() % NUM_ELEMENTS;
        EXPECT_TRUE(loaded_SkipListV.search(test_index * 2))
            << "随机抽样验证失败：数据 " << (test_index * 2) << " 应该在加载的跳表中";
    }

    // 清理测试文件
    std::remove(test_file.c_str());

    // 验证文件已被清理
    std::ifstream cleanup_check(test_file);
    EXPECT_FALSE(cleanup_check.is_open()) << "测试文件应该已被清理";
}

// 字符串类型文件持久化测试
TEST(SkipListVTest, StringFileSaveLoadTest)
{
    SkipListV<std::string> skiplistv;
    const std::string test_file = "/tmp/string_SkipListV_test.txt";

    skiplistv.setStorageFile(test_file);

    // 插入字符串数据
    skiplistv.insert("apple");
    skiplistv.insert("banana");
    skiplistv.insert("cherry");
    skiplistv.insert("date");
    skiplistv.insert("elderberry");

    // 验证原始数据
    EXPECT_TRUE(skiplistv.search("apple")) << "原始跳表应该包含apple";
    EXPECT_TRUE(skiplistv.search("banana")) << "原始跳表应该包含banana";
    EXPECT_TRUE(skiplistv.search("cherry")) << "原始跳表应该包含cherry";
    EXPECT_TRUE(skiplistv.search("date")) << "原始跳表应该包含date";
    EXPECT_TRUE(skiplistv.search("elderberry")) << "原始跳表应该包含elderberry";

    // 保存到文件
    skiplistv.saveToFile();

    // 验证文件是否正确生成
    std::ifstream file_check(test_file);
    EXPECT_TRUE(file_check.is_open()) << "字符串数据文件应该被正确创建";
    file_check.close();

    // 验证文件内容
    std::ifstream content_check(test_file);
    std::string line;
    std::vector<std::string> file_contents;
    while (std::getline(content_check, line))
    {
        if (!line.empty())
        {
            file_contents.push_back(line);
        }
    }
    content_check.close();

    // 验证文件中的数据
    EXPECT_EQ(file_contents.size(), 5) << "文件应该包含5个字符串数据元素";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), "apple"), file_contents.end()) << "文件应该包含apple";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), "banana"), file_contents.end()) << "文件应该包含banana";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), "cherry"), file_contents.end()) << "文件应该包含cherry";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), "date"), file_contents.end()) << "文件应该包含date";
    EXPECT_NE(std::find(file_contents.begin(), file_contents.end(), "elderberry"), file_contents.end()) << "文件应该包含elderberry";

    // 验证字符串顺序（文件中的顺序可能与插入顺序不同）
    EXPECT_TRUE(std::is_sorted(file_contents.begin(), file_contents.end())) << "文件中的字符串应该按字典序排序";

    // 创建新的跳表实例并加载数据
    SkipListV<std::string> skiplistv1;
    skiplistv1.setStorageFile(test_file);
    skiplistv1.loadFromFile();

    // 验证加载的数据
    EXPECT_TRUE(skiplistv1.search("apple")) << "加载的跳表应该包含apple";
    EXPECT_TRUE(skiplistv1.search("banana")) << "加载的跳表应该包含banana";
    EXPECT_TRUE(skiplistv1.search("cherry")) << "加载的跳表应该包含cherry";
    EXPECT_TRUE(skiplistv1.search("date")) << "加载的跳表应该包含date";
    EXPECT_TRUE(skiplistv1.search("elderberry")) << "加载的跳表应该包含elderberry";
    EXPECT_FALSE(skiplistv1.search("fig")) << "加载的跳表不应该包含fig";

    // 验证多个不存在的字符串
    std::vector<std::string> non_existent = {"fig", "grape", "honeydew", "kiwi", "lemon"};
    for (const auto &fruit : non_existent)
    {
        EXPECT_FALSE(skiplistv1.search(fruit)) << "加载的跳表不应该包含 " << fruit;
    }

    // 验证数据完整性：所有原始数据都应该在加载的跳表中
    std::vector<std::string> original_data = {"apple", "banana", "cherry", "date", "elderberry"};
    for (const auto &fruit : original_data)
    {
        EXPECT_TRUE(skiplistv1.search(fruit)) << "数据 " << fruit << " 应该在加载的跳表中";
    }

    // 清理测试文件
    std::remove(test_file.c_str());

    // 验证文件已被清理
    std::ifstream cleanup_check(test_file);
    EXPECT_FALSE(cleanup_check.is_open()) << "测试文件应该已被清理";
}

// 文件路径未设置测试
TEST(SkipListVTest, NoFilePathTest)
{
    SkipListV<int> skiplistv;

    // 不设置文件路径，调用保存和加载
    skiplistv.insert(10);
    skiplistv.saveToFile();   // 应该不会报错，只是静默返回
    skiplistv.loadFromFile(); // 应该不会报错，只是静默返回

    // 验证数据仍然存在
    EXPECT_TRUE(skiplistv.search(10));
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