#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

/**
 * @file test_functions.h
 * @brief 测试函数声明
 */

/**
 * @brief 测试1：扫描所有已安装的mod和DLC
 */
void test_ScanInstalledModsAndDLCs();

/**
 * @brief 测试2：读取当前游戏加载的mod列表
 */
void test_ReadCurrentModList();

/**
 * @brief 测试3：修改mod列表排序并添加/移除DLC
 */
void test_ModifyModListAndDLCs();

/**
 * @brief 测试4：创建空白加载列表并添加mod
 */
void test_CreateEmptyModList();

/**
 * @brief 运行所有测试
 */
void runAllTests();

#endif // TEST_FUNCTIONS_H
