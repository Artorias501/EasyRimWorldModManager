# EasyRimWorldModManager

简单的RimWorld Mod管理器，可以排序、添加备注

## 功能特性

- 🔍 自动扫描Steam创意工坊Mod和官方DLC
- 📝 为Mod添加自定义类型和备注
- 🔄 管理Mod加载顺序
- 💾 保存多个Mod配置方案
- 🎯 智能识别Mod依赖关系

## 文件读取位置

### 1. 游戏配置文件
程序从以下位置读取RimWorld的Mod配置：

```
C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
```

该文件包含：
- `<version>`: 游戏版本信息
- `<activeMods>`: 当前激活的所有Mod（核心、DLC、创意工坊Mod）
- `<knownExpansions>`: 用户拥有的官方DLC列表

### 2. Steam创意工坊Mod
程序从以下位置扫描Steam创意工坊的Mod：

```
{Steam安装路径}\steamapps\workshop\content\294100\
```

每个Mod目录结构：
```
294100\
├── 3585148922\         # Mod的WorkshopId
│   └── About\
│       └── About.xml   # Mod信息文件
├── 3575567766\
│   └── About\
│       └── About.xml
└── ...
```

### 3. 官方DLC
程序从以下位置扫描官方DLC和核心游戏：

```
{游戏安装路径}\Data\
```

通常游戏安装路径为：
```
{Steam安装路径}\steamapps\common\RimWorld\
```

每个DLC目录结构：
```
Data\
├── Core\               # 核心游戏
│   └── About\
│       └── About.xml
├── Royalty\            # 王权DLC
│   └── About\
│       └── About.xml
├── Ideology\           # 意识形态DLC
│   └── About\
│       └── About.xml
└── ...
```

## 用户数据保存

程序将用户的自定义数据保存在程序所在目录的 `UserData` 文件夹中：

```
EasyRimWorldModManager.exe所在目录\
└── UserData\
    ├── Mod\
    │   ├── mod_data.json       # Mod类型和备注映射
    │   └── custom_types.json   # 自定义类型列表
    └── ModList\
        ├── my_config_1.xml     # 用户保存的Mod配置
        ├── my_config_2.xml
        └── ...
```

### mod_data.json
存储每个Mod的类型和备注（通过PackageId映射）：

```json
{
  "types": {
    "brrainz.harmony": "前置框架",
    "ludeon.rimworld": "核心"
  },
  "remarks": {
    "brrainz.harmony": "Harmony是最重要的前置mod",
    "ludeon.rimworld": "RimWorld核心游戏"
  }
}
```

### custom_types.json
存储用户自定义的Mod类型列表：

```json
{
  "customTypes": [
    "美化mod",
    "剧情mod",
    "战斗增强"
  ]
}
```

## XML文件保存逻辑

### 重要原则：最小化修改

程序在保存Mod配置时，遵循以下原则：

1. **Mod扫描与配置的关系**：
   - `WorkshopScanner`：扫描Steam创意工坊路径，获取所有创意工坊mod（`isOfficialDLC=false`）
   - `OfficialDLCScanner`：扫描游戏Data文件夹，获取Core和所有官方DLC
     - **Core（ludeon.rimworld.core）特殊处理**：`isOfficialDLC=false`，`type="Core"`
     - **其他官方DLC**：`isOfficialDLC=true`，`type="DLC"`
   - `ModConfigManager`：整合所有扫描到的mod，并生成XML配置

2. **XML字段生成规则**：
   - `<activeMods>`：包含**所有**启用的mod（Core + DLC + 创意工坊mod）
   - `<knownExpansions>`：**仅包含官方DLC**（通过`isOfficialDLC=true`筛选）
     - ✅ 包含：`ludeon.rimworld.royalty`、`ludeon.rimworld.ideology`等DLC
     - ❌ 不包含：`ludeon.rimworld.core`（Core不是扩展包）
     - ❌ 不包含：创意工坊mod
   - `<version>`：游戏版本信息（**保持原文件值不变**）

3. **字段修改原则**：
   - ✅ 修改 `<activeMods>` - Mod加载顺序和启用列表
   - ✅ 修改 `<knownExpansions>` - 通过`isOfficialDLC`自动生成
   - ❌ **不修改** `<version>` - 游戏版本信息
   - ❌ **不修改** 其他所有未知字段

4. **完整保留原文件结构**：
   ```xml
   <?xml version="1.0" encoding="utf-8"?>
   <ModsConfigData>
     <version>1.6.4633 rev1261</version>        <!-- 保持原文件值 -->
     <activeMods>
       <li>ludeon.rimworld.core</li>            <!-- Core在activeMods中 -->
       <li>brrainz.harmony</li>                 <!-- 可修改：调整顺序 -->
       <li>ludeon.rimworld.royalty</li>         <!-- DLC也在activeMods中 -->
       <li>workshop.mod.12345</li>              <!-- 创意工坊mod -->
       ...
     </activeMods>
     <knownExpansions>
       <li>ludeon.rimworld.royalty</li>         <!-- 仅DLC（isOfficialDLC=true） -->
       <li>ludeon.rimworld.ideology</li>        <!-- Core不在这里 -->
       ...
     </knownExpansions>
     <!-- 其他任何字段都会被保留 -->
   </ModsConfigData>
   ```

5. **工作流程**：
   ```
   1. 扫描阶段：
      WorkshopScanner → 创意工坊mod (isOfficialDLC=false)
      OfficialDLCScanner → Core (isOfficialDLC=false, type="Core")
                         → DLC (isOfficialDLC=true, type="DLC")
   
   2. 配置生成阶段：
      ModConfigManager.setActiveModsFromList(所有mod)
      ↓
      activeMods = 所有mod的packageId
      knownExpansions = 筛选出isOfficialDLC=true的mod
   
   3. 保存阶段：
      读取原始 ModsConfig.xml → 保存version和其他字段
      ↓
      写入新的 activeMods 和 knownExpansions
      ↓
      写入所有保留的字段
   ```

### 保存选项

1. **直接覆盖游戏配置**：
   - 保存路径：游戏配置文件位置
   - 效果：立即改变游戏的Mod加载顺序

2. **另存为自定义配置**：
   - 保存路径：`UserData/ModList/配置名.xml`
   - 效果：创建一个配置快照，可随时加载使用

## 使用示例

### 基本使用流程

```cpp
// 1. 扫描所有Mod
ModManager modManager("D:/Steam");
modManager.scanAll();

// 2. 加载用户数据
UserDataManager userDataManager;
userDataManager.loadAll();

// 3. 读取当前游戏配置
ModConfigManager configManager;
configManager.loadConfig();

// 4. 修改Mod顺序
QStringList mods = configManager.getActiveMods();
// ... 调整mods列表 ...
configManager.setActiveMods(mods);

// 5. 保存配置
// 选项A：直接覆盖游戏配置
configManager.saveConfig();

// 选项B：另存为自定义配置
userDataManager.saveModListToPath(
    "UserData/ModList/my_config.xml",
    configManager.getActiveMods(),
    configManager.getKnownExpansions(),
    configManager.getVersion(),
    configManager.getOtherFields()  // 保留所有其他字段
);

// 6. 保存用户数据（类型、备注）
userDataManager.saveAll();
```

### 创建空白加载列表

如果需要从零开始配置mod列表，可以使用空白加载列表功能：

```cpp
// 创建空白加载列表（从游戏配置读取版本和其他字段，但mod列表为空）
ModConfigManager configManager;
configManager.loadConfigWithEmptyMods();  // 从默认路径读取

// 或指定配置文件路径
configManager.loadConfigWithEmptyMods("path/to/ModsConfig.xml");

// 此时：
// - version: 从原配置读取（如 "1.6.4633 rev1261"）
// - activeMods: 空列表
// - knownExpansions: 空列表（因为没有已加载的官方DLC）
// - 其他字段: 从原配置读取并保留

// 添加想要的mod
configManager.addMod("ludeon.rimworld.core");  // 添加Core
configManager.addMod("brrainz.harmony");        // 添加Harmony
configManager.addMod("ludeon.rimworld.royalty"); // 添加DLC

// 或从ModItem列表设置（推荐方式，会自动生成knownExpansions）
QList<ModItem*> selectedMods = { /* ... */ };
configManager.setActiveModsFromList(selectedMods);
// setActiveModsFromList会：
// - 将所有mod的packageId添加到activeMods
// - 自动将isOfficialDLC=true的mod添加到knownExpansions

// 保存配置
configManager.saveConfig();  // 保存到原路径
// 或
configManager.saveConfig("path/to/custom_config.xml");  // 另存为
```

**空白加载列表的优势：**
- ✅ 保留游戏版本信息
- ✅ 保留所有其他配置字段
- ✅ 从零开始精确控制mod加载顺序
- ✅ 使用setActiveModsFromList时会自动正确生成knownExpansions
- ✅ 适合创建特定用途的mod配置（如测试、特定玩法）

## 技术栈

- C++20
- Qt 6.10.1
- CMake 3.31

## 构建说明

1. 确保已安装Qt 6.10.1
2. 配置CMakeLists.txt中的Qt路径
3. 使用CMake构建项目

## 许可证

MIT License

## 注意事项

⚠️ **重要提示**：
- 修改Mod配置前建议备份原配置文件
- 确保Steam路径和游戏安装路径配置正确
- 首次运行会自动创建UserData目录结构
- 程序会自动保留游戏配置文件中的所有未知字段，确保兼容性
