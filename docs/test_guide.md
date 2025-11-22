# 测试功能说明

## 概述

本项目的 `main.cpp` 实现了完整的功能测试，验证所有核心功能是否正常工作。

## 测试内容

### 测试1：扫描所有已安装的mod和DLC
**文件**：`src/test/test_functions.cpp` - `test_ScanInstalledModsAndDLCs()`

**功能**：
- 自动检测Steam安装路径
- 扫描创意工坊mod（`{Steam}/steamapps/workshop/content/294100`）
- 扫描官方DLC和Core（`{GameInstallPath}/Data`）
- 区分Core、DLC和普通mod

**输出**：
- 创意工坊mod数量和列表
- 官方DLC数量和列表
- 每个mod的详细信息（名称、PackageId、类型、路径）

**验证点**：
- ✅ Core正确标记为 `isOfficialDLC=false`, `type="Core"`
- ✅ 官方DLC标记为 `isOfficialDLC=true`, `type="DLC"`
- ✅ 创意工坊mod标记为 `isOfficialDLC=false`

---

### 测试2：读取当前游戏加载的mod列表
**文件**：`src/test/test_functions.cpp` - `test_ReadCurrentModList()`

**功能**：
- 读取游戏配置文件 `ModsConfig.xml`
- 解析游戏版本、激活mod列表、已知扩展包

**输出**：
- 配置文件路径
- 游戏版本号
- 当前加载的mod数量和列表
- knownExpansions内容

**验证点**：
- ✅ 成功读取配置文件
- ✅ 正确解析所有字段
- ✅ knownExpansions只包含官方DLC

---

### 测试3：修改mod列表排序并添加/移除DLC
**文件**：`src/test/test_functions.cpp` - `test_ModifyModListAndDLCs()`

**功能**：
1. 读取当前游戏配置
2. 扫描所有可用mod
3. 构建新的mod列表（Core + 创意工坊mod + DLC）
4. 使用 `setActiveModsFromList()` 设置新列表
5. 测试mod排序功能
6. 保存到UserData

**输出**：
- 新配置的mod数量
- 自动生成的knownExpansions内容
- 排序前后的mod顺序对比
- 保存的配置文件路径

**验证点**：
- ✅ `setActiveModsFromList()` 正确设置 `activeMods`
- ✅ `knownExpansions` 自动从mod列表中筛选（仅包含`isOfficialDLC=true`的mod）
- ✅ Core不在 `knownExpansions` 中
- ✅ 排序功能正常工作
- ✅ 保存到UserData，不覆盖游戏配置

**生成文件**：
- `UserData/ModList/test_modified_config.xml`
- `UserData/ModList/test_sorted_config.xml`

---

### 测试4：创建空白加载列表并添加mod
**文件**：`src/test/test_functions.cpp` - `test_CreateEmptyModList()`

**功能**：
1. 使用 `loadConfigWithEmptyMods()` 创建空白列表
2. 验证 `activeMods` 和 `knownExpansions` 都为空
3. 从扫描结果选择mod（Core + 创意工坊 + DLC）
4. 使用 `setActiveModsFromList()` 添加mod
5. 测试排序功能
6. 保存到UserData

**输出**：
- 空白列表创建确认（版本保留，列表清空）
- 选择的mod列表
- 自动生成的knownExpansions
- 排序后的结果

**验证点**：
- ✅ `loadConfigWithEmptyMods()` 清空 `activeMods` 和 `knownExpansions`
- ✅ 保留游戏版本信息
- ✅ 保留其他配置字段
- ✅ `setActiveModsFromList()` 自动生成正确的 `knownExpansions`
- ✅ 排序功能正常

**生成文件**：
- `UserData/ModList/test_empty_start_config.xml`

---

## 运行测试

### 编译运行
```bash
mkdir build
cd build
cmake ..
cmake --build .
./EasyRimWorldModManager
```

### 预期输出
```
========================================
  EasyRimWorldModManager - 功能测试
========================================

本测试将执行以下操作：
  1. 扫描所有已安装的mod和DLC
  2. 读取当前游戏加载的mod列表
  3. 修改mod列表排序并添加/移除DLC
  4. 创建空白加载列表并添加mod

⚠️  注意：所有配置都保存到UserData目录
   不会修改游戏的实际配置文件！

============================================================
开始运行所有测试
============================================================
测试时间: 2025-11-22 14:30:00

============================================================
测试1：扫描所有已安装的mod和DLC
============================================================
Steam路径: C:/Program Files (x86)/Steam
游戏安装路径: C:/Program Files (x86)/Steam/steamapps/common/RimWorld

正在扫描...

✅ 扫描完成
   创意工坊mod数量: 150
   官方内容数量: 8

前5个创意工坊mod:
[1] Harmony
    PackageId: brrainz.harmony
    Type: 
    IsOfficialDLC: No
    Path: ...

所有官方内容:
[1] Core
    PackageId: ludeon.rimworld.core
    Type: Core
    IsOfficialDLC: No
    Path: ...

[2] Royalty
    PackageId: ludeon.rimworld.royalty
    Type: DLC
    IsOfficialDLC: Yes
    Path: ...

✓ 测试1完成

[... 其他测试输出 ...]

============================================================
所有测试完成
============================================================

生成的文件:
  - UserData/ModList/test_modified_config.xml
  - UserData/ModList/test_sorted_config.xml
  - UserData/ModList/test_empty_start_config.xml

提示：所有配置都保存到了UserData目录，没有修改游戏的实际配置。

========================================
  测试完成
========================================
```

---

## 生成的文件检查

### UserData/ModList/test_modified_config.xml
修改后的配置，包含：
- 原始游戏版本
- 新的mod列表（Core + 部分创意工坊mod + DLC）
- 自动生成的knownExpansions（只包含DLC）

### UserData/ModList/test_sorted_config.xml
排序后的配置，验证 `moveModToPosition()` 功能

### UserData/ModList/test_empty_start_config.xml
从空白列表创建的配置，验证 `loadConfigWithEmptyMods()` 功能

---

## 重要验证点

### knownExpansions字段验证
所有生成的配置文件中，`knownExpansions` 应该：
- ✅ 只包含官方DLC（`isOfficialDLC=true`）
- ✅ 不包含Core（`ludeon.rimworld.core`）
- ✅ 不包含创意工坊mod
- ✅ 与 `activeMods` 中的DLC对应

示例：
```xml
<activeMods>
  <li>ludeon.rimworld.core</li>      <!-- Core -->
  <li>brrainz.harmony</li>           <!-- 创意工坊mod -->
  <li>ludeon.rimworld.royalty</li>   <!-- DLC -->
  <li>ludeon.rimworld.ideology</li>  <!-- DLC -->
</activeMods>

<knownExpansions>
  <li>ludeon.rimworld.royalty</li>   <!-- 只有DLC -->
  <li>ludeon.rimworld.ideology</li>  <!-- 只有DLC -->
</knownExpansions>
```

---

## 注意事项

1. **不会修改游戏配置**
   - 所有测试输出都保存到 `UserData/ModList/` 目录
   - 游戏的实际配置文件不会被修改
   - 可以安全运行测试

2. **需要游戏配置文件**
   - 测试2和测试4需要读取 `ModsConfig.xml`
   - 请至少运行过一次游戏以生成配置文件
   - 路径：`C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml`

3. **Steam路径自动检测**
   - 程序会尝试从注册表读取Steam路径
   - 如果检测失败，会使用默认路径
   - 可以手动修改 `test_functions.cpp` 中的路径

4. **扫描可能失败的原因**
   - Steam路径不正确
   - 游戏未安装或路径不在默认位置
   - 没有安装任何创意工坊mod

---

## 故障排除

### 错误：无法读取ModsConfig.xml
**原因**：游戏配置文件不存在

**解决**：运行一次游戏以生成配置文件

### 错误：扫描失败
**原因**：Steam或游戏路径不正确

**解决**：
1. 检查Steam是否正确安装
2. 检查游戏是否安装在默认位置
3. 修改 `test_functions.cpp` 中的路径

### 没有输出创意工坊mod
**原因**：没有订阅任何创意工坊mod

**解决**：
1. 在Steam创意工坊订阅一些mod
2. 测试仍会继续运行，只是显示数量为0

---

## 扩展测试

如果需要单独运行某个测试：

```cpp
// 在main.cpp中
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 只运行测试1
    test_ScanInstalledModsAndDLCs();
    
    // 或只运行测试3
    // test_ModifyModListAndDLCs();
    
    return 0;
}
```

---

## 总结

这套测试完整覆盖了：
- ✅ Mod扫描（创意工坊 + 官方）
- ✅ 配置读取和解析
- ✅ Mod列表修改和排序
- ✅ knownExpansions自动生成
- ✅ 空白列表创建
- ✅ 配置保存到UserData

所有测试都**不会修改游戏的实际配置**，可以安全运行。
