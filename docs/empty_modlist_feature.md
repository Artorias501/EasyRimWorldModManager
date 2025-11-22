# 空白加载列表功能说明

## 功能概述

`ModConfigManager::loadConfigWithEmptyMods()` 提供了创建空白mod加载列表的能力，同时保留游戏配置中的重要元数据。

## 主要特性

### 保留的信息
从原始 `ModsConfig.xml` 读取并保留：
- ✅ `<version>` - 游戏版本信息
- ✅ 所有其他未知字段 - 保证兼容性

### 清空的信息
- ❌ `<activeMods>` - 清空为空列表
- ❌ `<knownExpansions>` - 清空为空列表（因为它是从activeMods中筛选的官方DLC）

### 重要说明
`knownExpansions` 也会被清空，因为它的作用是列出**已加载mod中**的官方DLC，而不是列出所有拥有的DLC。当使用 `setActiveModsFromList()` 添加mod时，会自动根据 `isOfficialDLC=true` 重新生成正确的 `knownExpansions`。

## API说明

```cpp
// 方法1：从默认配置路径加载
bool loadConfigWithEmptyMods();

// 方法2：从指定路径加载
bool loadConfigWithEmptyMods(const QString &configPath);
```

### 返回值
- `true`: 成功加载并创建空白列表
- `false`: 加载失败（文件不存在或解析错误）

## 使用场景

### 场景1：创建测试配置
从零开始创建一个用于测试的最小mod配置：

```cpp
ModConfigManager config;
config.loadConfigWithEmptyMods();

// 只添加必要的mod
config.addMod("ludeon.rimworld.core");
config.addMod("brrainz.harmony");

config.saveConfig("test_minimal.xml");
```

### 场景2：特定玩法配置
为特定玩法（如纯战斗、纯建造等）创建专门的mod配置：

```cpp
ModConfigManager config;
config.loadConfigWithEmptyMods();

// 添加战斗相关mod
QList<ModItem*> combatMods = getCombatRelatedMods();
config.setActiveModsFromList(combatMods);

config.saveConfig("combat_playthrough.xml");
```

### 场景3：性能测试
逐个添加mod以测试性能影响：

```cpp
ModConfigManager config;
config.loadConfigWithEmptyMods();

// 基准：只有Core
config.addMod("ludeon.rimworld.core");
testPerformance("baseline");

// 添加Harmony
config.addMod("brrainz.harmony");
testPerformance("with_harmony");

// 逐个添加更多mod...
```

### 场景4：Mod冲突排查
通过二分法排查mod冲突：

```cpp
ModConfigManager config;
config.loadConfigWithEmptyMods();

// 只添加前半部分mod
QList<ModItem*> firstHalf = allMods.mid(0, allMods.size() / 2);
config.setActiveModsFromList(firstHalf);

// 测试是否有问题
```

## 与普通加载的对比

| 特性 | `loadConfig()` | `loadConfigWithEmptyMods()` |
|------|----------------|----------------------------|
| 版本信息 | ✅ 保留 | ✅ 保留 |
| knownExpansions | ✅ 保留 | ❌ 清空 |
| activeMods | ✅ 保留 | ❌ 清空 |
| 其他字段 | ✅ 保留 | ✅ 保留 |
| 适用场景 | 修改现有配置 | 从零创建新配置 |

**重要说明**：`knownExpansions` 也会被清空，因为它应该是从 `activeMods` 中筛选出的官方DLC列表。使用 `setActiveModsFromList()` 时会自动重新生成正确的 `knownExpansions`。

## 完整工作流程示例

```cpp
// 1. 扫描所有可用mod
ModManager modManager("D:/Steam");
modManager.setGameInstallPath("D:/Steam/steamapps/common/RimWorld");
modManager.scanAll();

// 2. 创建空白配置（保留游戏版本等信息）
ModConfigManager configManager;
if (!configManager.loadConfigWithEmptyMods())
{
    qDebug() << "无法读取游戏配置";
    return;
}

// 3. 从扫描结果选择需要的mod
QList<ModItem*> selectedMods;
for (ModItem *mod : modManager.getAllMods())
{
    // 自定义筛选逻辑
    if (shouldIncludeMod(mod))
    {
        selectedMods.append(mod);
    }
}

// 4. 设置选中的mod
configManager.setActiveModsFromList(selectedMods);
// 此时：
// - activeMods = 选中mod的packageId列表
// - knownExpansions = 自动从selectedMods中筛选出isOfficialDLC=true的mod

// 5. 调整顺序（可选）
configManager.moveModToPosition("brrainz.harmony", 0);  // Harmony放在最前

// 6. 保存配置
configManager.saveConfig("UserData/ModList/custom_config.xml");

// 7. 或直接覆盖游戏配置
// configManager.saveConfig();  // 保存到默认位置
```

## 注意事项

1. **必须先调用扫描器**：
   如果使用 `setActiveModsFromList()`，需要先扫描可用mod

2. **版本兼容性**：
   保留的版本信息来自原配置文件，确保与当前游戏版本匹配

3. **knownExpansions的处理**：
   - ⚠️ `loadConfigWithEmptyMods()` 会清空 `knownExpansions`
   - ✅ 使用 `setActiveModsFromList()` 会自动重新生成（推荐）
   - ⚠️ 使用 `addMod()` 手动添加时，需要注意：
     - 如果添加的是官方DLC，需要手动更新 `knownExpansions`
     - 或在最后统一调用 `setActiveModsFromList()` 来自动生成

4. **文件必须存在**：
   原始的 `ModsConfig.xml` 必须存在，否则加载失败

## 错误处理

```cpp
ModConfigManager config;

if (!config.loadConfigWithEmptyMods())
{
    // 失败可能的原因：
    // 1. 游戏配置文件不存在
    // 2. 文件无法读取（权限问题）
    // 3. XML格式错误
    
    // 处理方案：
    // - 检查游戏是否正确安装
    // - 检查文件路径是否正确
    // - 至少运行过一次游戏以生成配置文件
    
    qDebug() << "配置路径:" << ModConfigManager::getDefaultConfigPath();
    return false;
}

// 成功后可以继续操作
config.addMod("ludeon.rimworld.core");
// ...
```

## 实现细节

```cpp
bool ModConfigManager::loadConfigWithEmptyMods(const QString &configPath)
{
    m_configPath = configPath;

    // 读取文件
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QString xmlContent = file.readAll();
    file.close();

    // 解析配置文件（包括version、knownExpansions、activeMods和其他字段）
    if (!parseXml(xmlContent))
    {
        return false;
    }

    // 清空activeMods和knownExpansions列表，保留其他所有信息
    m_activeMods.clear();
    m_knownExpansions.clear();  // 因为knownExpansions是从activeMods中筛选的

    return true;
}
```

### parseXml() 的作用
- 读取 `<version>`
- 读取 `<knownExpansions>`（稍后会被清空）
- 读取 `<activeMods>`（稍后会被清空）
- 读取所有其他未知字段到 `m_otherFields`

### generateXml() 的作用
保存时会写入：
- 保留的 `<version>`
- 修改后的 `<activeMods>`（可能为空，或通过addMod/setActiveModsFromList填充）
- 重新生成的 `<knownExpansions>`（通过setActiveModsFromList自动生成，或为空）
- 所有保留的其他字段

## 总结

`loadConfigWithEmptyMods()` 是一个强大的功能，允许用户：
- 从干净的状态开始配置mod
- 保持与游戏配置的兼容性
- 灵活创建各种用途的mod配置
- 精确控制mod加载顺序

特别适合需要精确控制mod列表的高级用户和mod测试场景。
