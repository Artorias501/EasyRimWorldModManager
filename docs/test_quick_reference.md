# 测试功能快速参考

## 文件结构

```
src/
├── main.cpp                      # 主入口，调用测试
├── test/
│   ├── test_functions.h         # 测试函数声明
│   └── test_functions.cpp       # 测试函数实现
└── data/                        # 核心功能实现
```

## 测试列表

| 测试 | 函数 | 功能 | 输出文件 |
|------|------|------|----------|
| 1 | `test_ScanInstalledModsAndDLCs()` | 扫描mod和DLC | 无 |
| 2 | `test_ReadCurrentModList()` | 读取游戏配置 | 无 |
| 3 | `test_ModifyModListAndDLCs()` | 修改列表和排序 | `test_modified_config.xml`<br>`test_sorted_config.xml` |
| 4 | `test_CreateEmptyModList()` | 空白列表创建 | `test_empty_start_config.xml` |

## 运行方式

```bash
# 编译
cmake -B build
cmake --build build

# 运行
./build/EasyRimWorldModManager
```

## 关键验证点

### ✅ Core标记
```cpp
isOfficialDLC = false
type = "Core"
```

### ✅ DLC标记
```cpp
isOfficialDLC = true
type = "DLC"
```

### ✅ knownExpansions生成
```
activeMods = [Core, Mod1, DLC1, Mod2, DLC2]
                            ↓ 筛选 isOfficialDLC=true
knownExpansions = [DLC1, DLC2]
```

### ✅ 空白列表
```cpp
loadConfigWithEmptyMods()
→ version: 保留
→ activeMods: []
→ knownExpansions: []
```

## 常见问题

**Q: 测试是否会修改游戏配置？**
A: 不会。所有输出都保存到 `UserData/ModList/` 目录。

**Q: 需要什么前提条件？**
A: 
- 游戏已安装
- 至少运行过一次游戏（生成配置文件）
- （可选）订阅一些创意工坊mod

**Q: 如果扫描失败怎么办？**
A: 检查Steam路径，或修改 `test_functions.cpp` 中的路径。

## 输出示例

```
============================================================
测试3：修改mod列表排序并添加/移除DLC
============================================================
1. 读取当前配置...
   ✓ 原始配置加载成功
   - 激活mod数: 50
   - knownExpansions数: 3

2. 扫描可用mod...
   ✓ 扫描完成

3. 构建新的mod列表...
   ✓ 添加 Core: ludeon.rimworld.core
   添加创意工坊mod...
   ✓ 添加: Harmony
   ✓ 添加: HugsLib
   ✓ 添加: Mod Manager
   添加官方DLC...
   ✓ 添加 DLC: Royalty
   ✓ 添加 DLC: Ideology
   ✓ 添加 DLC: Biotech

4. 应用新的mod列表...
   ✓ 新配置已生成
   - 激活mod数: 7
   - knownExpansions数: 3

   knownExpansions 内容:
     * ludeon.rimworld.royalty
     * ludeon.rimworld.ideology
     * ludeon.rimworld.biotech

5. 保存配置到 UserData...
   ✓ 配置已保存到: UserData/ModList/test_modified_config.xml

6. 测试mod排序功能...
   移动前:
     [0] ludeon.rimworld.core
     [1] brrainz.harmony
   移动后:
     [0] brrainz.harmony
     [1] ludeon.rimworld.core
   ✓ 排序后的配置已保存到: UserData/ModList/test_sorted_config.xml

✓ 测试3完成
```

## 代码结构

### 测试函数模板
```cpp
void test_XXX()
{
    printSeparator("测试X：功能描述");
    
    // 1. 准备工作
    qDebug() << "1. 准备...";
    // ...
    
    // 2. 执行操作
    qDebug() << "\n2. 执行...";
    // ...
    
    // 3. 验证结果
    qDebug() << "\n3. 验证...";
    // ...
    
    // 4. 保存输出
    qDebug() << "\n4. 保存...";
    // ...
    
    qDebug() << "\n✓ 测试X完成";
}
```

### 辅助函数
```cpp
void printSeparator(const QString &title);    // 打印分隔线
void printModInfo(const ModItem *mod, int index);  // 打印mod信息
```

## 核心API使用

### 扫描mod
```cpp
ModManager modManager;
modManager.setSteamPath(steamPath);
modManager.setGameInstallPath(gameInstallPath);
modManager.scanAll();

QList<ModItem*> workshopMods = modManager.getWorkshopMods();
QList<ModItem*> officialDLCs = modManager.getOfficialDLCs();
```

### 读取配置
```cpp
ModConfigManager configManager;
configManager.loadConfig();

QStringList activeMods = configManager.getActiveMods();
QStringList knownExpansions = configManager.getKnownExpansions();
```

### 修改和保存
```cpp
// 修改
configManager.setActiveModsFromList(modList);  // 自动生成knownExpansions
configManager.moveModToPosition(modId, newPos);

// 保存到UserData
UserDataManager userDataManager;
userDataManager.saveModListToPath(
    path,
    configManager.getActiveMods(),
    configManager.getKnownExpansions(),
    configManager.getVersion(),
    configManager.getOtherFields()
);
```

### 空白列表
```cpp
ModConfigManager configManager;
configManager.loadConfigWithEmptyMods();  // 清空activeMods和knownExpansions

configManager.setActiveModsFromList(selectedMods);  // 添加mod
```

## 文件位置

### 输入
- 游戏配置：`C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml`
- 创意工坊：`{Steam}\steamapps\workshop\content\294100\`
- 官方内容：`{GameInstallPath}\Data\`

### 输出
- 测试配置：`UserData/ModList/*.xml`
- 用户数据：`UserData/Mod/mod_data.json`
- 自定义类型：`UserData/Mod/custom_types.json`
