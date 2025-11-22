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

1. **只修改必要的字段**：
   - ✅ 修改 `<activeMods>` - Mod加载顺序
   - ✅ 保持 `<knownExpansions>` - 官方DLC列表（通常不变）
   - ❌ **不修改** `<version>` - 游戏版本信息
   - ❌ **不修改** 其他所有未知字段

2. **完整保留原文件结构**：
   ```xml
   <?xml version="1.0" encoding="utf-8"?>
   <ModsConfigData>
     <version>1.6.4633 rev1261</version>        <!-- 保持不变 -->
     <activeMods>
       <li>brrainz.harmony</li>                 <!-- 可修改：调整顺序 -->
       <li>ludeon.rimworld</li>                 <!-- 可修改：增删Mod -->
       ...
     </activeMods>
     <knownExpansions>
       <li>ludeon.rimworld.royalty</li>         <!-- 保持不变 -->
       <li>ludeon.rimworld.ideology</li>        <!-- 保持不变 -->
       ...
     </knownExpansions>
     <!-- 其他任何字段都会被保留 -->
   </ModsConfigData>
   ```

3. **工作流程**：
   ```
   读取 ModsConfig.xml
   ↓
   解析并保存所有字段（包括未知字段）
   ↓
   用户修改 activeMods（调整顺序、增删Mod）
   ↓
   保存时：
   - 写入原始的 version
   - 写入修改后的 activeMods
   - 写入原始的 knownExpansions
   - 写入所有其他保存的字段
   ↓
   生成的文件与原文件结构完全一致（除了 activeMods）
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
