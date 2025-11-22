# EasyRimWorldModManager API 文档

## 文档索引

本目录包含了 EasyRimWorldModManager 后端所有核心类的 API 文档和使用指南。

### 核心类文档

1. **[ModManager](ModManager_API.md)** - 中心管理类（推荐使用）
   - 统一的 Mod 管理接口
   - 整合扫描器和用户数据管理
   - 提供缓存机制

2. **[ModItem](ModItem_API.md)** - Mod 数据结构
   - Mod 的基本信息结构
   - 依赖关系管理
   - 加载顺序配置

3. **[ModConfigManager](ModConfigManager_API.md)** - 游戏配置管理
   - 读取/写入 ModsConfig.xml
   - Mod 列表排序和修改
   - knownExpansions 自动管理

4. **[UserDataManager](UserDataManager_API.md)** - 用户数据持久化
   - Mod 备注和类型管理
   - 自定义类型管理
   - Mod 列表保存

5. **[WorkshopScanner](WorkshopScanner_API.md)** - Steam 创意工坊扫描
   - 扫描创意工坊 Mod
   - 解析 About.xml
   - Steam 路径检测

6. **[OfficialDLCScanner](OfficialDLCScanner_API.md)** - 官方内容扫描
   - 扫描官方 DLC 和 Core
   - 区分官方内容和 Mod
   - 游戏路径管理

### 架构图

```
┌──────────────────────────────────────────────────────┐
│                    ModManager                         │
│              (推荐作为唯一入口使用)                    │
│  • 缓存所有 Mod 和 DLC                                │
│  • 自动同步用户数据                                    │
│  • 提供统一查询接口                                    │
└──────────────────────────────────────────────────────┘
           │                  │                │
           ▼                  ▼                ▼
    ┌─────────────┐   ┌──────────────┐  ┌──────────────┐
    │ Workshop    │   │ OfficialDLC  │  │ UserData     │
    │ Scanner     │   │ Scanner      │  │ Manager      │
    └─────────────┘   └──────────────┘  └──────────────┘
           │                  │                │
           ▼                  ▼                ▼
    ┌─────────────┐   ┌──────────────┐  ┌──────────────┐
    │ ModItem     │◄──┤ ModItem      │  │ JSON Files   │
    │ (Workshop)  │   │ (Official)   │  │ (UserData/)  │
    └─────────────┘   └──────────────┘  └──────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │ ModConfig        │
                    │ Manager          │
                    │ (ModsConfig.xml) │
                    └──────────────────┘
```

### 快速开始

#### 基本用法（推荐）

```cpp
#include "data/ModManager.h"

// 1. 创建 ModManager
ModManager modManager;

// 2. 设置路径
modManager.setSteamPath("C:/Program Files (x86)/Steam");

// 3. 扫描所有 Mod（会自动加载用户数据）
modManager.scanAll();

// 4. 获取数据
QList<ModItem *> allMods = modManager.getAllMods();
QList<ModItem *> workshopMods = modManager.getWorkshopMods();
QList<ModItem *> officialDLCs = modManager.getOfficialDLCs();

// 5. 查找特定 Mod
ModItem *mod = modManager.findModByPackageId("ludeon.rimworld.royalty");

// 6. 修改并保存用户数据
mod->remark = "我的备注";
mod->type = "功能性";
modManager.saveModsToUserData();
```

#### 配置管理

```cpp
#include "data/ModConfigManager.h"

// 1. 读取游戏配置
ModConfigManager configManager;
configManager.loadConfig();

// 2. 获取当前加载的 Mod 列表
QStringList activeMods = configManager.getActiveMods();

// 3. 修改列表
QList<ModItem *> newModList = { ... };
configManager.setActiveModsFromList(newModList);

// 4. 保存配置
configManager.saveConfig();
```

### 使用场景

#### 场景 1: 扫描并显示所有 Mod
```cpp
ModManager manager;
manager.setSteamPath(WorkshopScanner::detectSteamPath());
manager.scanAll();

for (ModItem *mod : manager.getAllMods()) {
    qDebug() << mod->name << "-" << mod->packageId;
}
```

#### 场景 2: 管理 Mod 列表
```cpp
ModConfigManager config;
config.loadConfig();

// 移动 Mod 到指定位置
config.moveModToPosition("some.packageid", 5);

// 保存到自定义位置
UserDataManager userMgr;
userMgr.saveModListToPath("UserData/ModList/my_list.xml",
                          config.getActiveMods(),
                          config.getKnownExpansions(),
                          config.getVersion());
```

#### 场景 3: 用户数据管理
```cpp
ModManager manager;
manager.scanAll();

// 通过 ModManager 访问 UserDataManager
UserDataManager *userMgr = manager.getUserDataManager();

// 设置 Mod 类型
userMgr->setModType("some.packageid", "功能性");

// 添加备注
userMgr->setModRemark("some.packageid", "这是我最喜欢的 Mod");

// 保存数据
userMgr->saveAll();
```

### 数据流程

#### 扫描流程
```
用户 → ModManager.scanAll()
         ├→ WorkshopScanner.scanAllMods()
         │   └→ 返回 QList<ModItem*>
         ├→ OfficialDLCScanner.scanAllDLCs()
         │   └→ 返回 QList<ModItem*>
         ├→ 缓存到 ModManager
         └→ loadUserDataToMods()
             └→ 从 UserDataManager 加载备注和类型
```

#### 配置修改流程
```
用户 → ModConfigManager.loadConfig()
         └→ 解析 ModsConfig.xml

用户 → ModConfigManager.setActiveModsFromList()
         ├→ 设置 activeMods
         └→ 自动生成 knownExpansions

用户 → ModConfigManager.saveConfig()
         └→ 写入 ModsConfig.xml（保留所有字段）
```

#### 用户数据流程
```
扫描时：
  UserDataManager.loadAll()
    ├→ 加载 mod_data.json (备注和类型)
    └→ 加载 custom_types.json (自定义类型)
  
  ModManager.loadUserDataToMods()
    └→ 将数据写入 ModItem

使用时：
  用户修改 ModItem.remark 或 ModItem.type

保存时：
  ModManager.saveModsToUserData()
    └→ UserDataManager.saveAll()
        ├→ 保存 mod_data.json
        └→ 保存 custom_types.json
```

### 目录结构

```
UserData/
├── ModData/
│   ├── mod_data.json         # Mod 备注和类型
│   └── custom_types.json     # 自定义类型列表
└── ModList/
    ├── list1.xml             # 用户保存的 Mod 列表
    ├── list2.xml
    └── ...
```

### 注意事项

1. **推荐使用 ModManager**
   - ModManager 是中心管理类，整合了所有功能
   - 直接使用 WorkshopScanner 或 OfficialDLCScanner 不会自动同步用户数据

2. **内存管理**
   - ModItem 对象由扫描器创建和管理
   - ModManager 的缓存只存储指针
   - 不要手动 delete ModItem，调用 clear() 会自动清理

3. **配置文件安全**
   - ModConfigManager 默认读取/写入游戏的 ModsConfig.xml
   - 测试时建议使用 UserDataManager.saveModListToPath() 保存到 UserData/

4. **数据持久化**
   - UserDataManager 会在 ModManager 析构时自动保存
   - 也可以手动调用 saveModsToUserData() 和 saveAll()

5. **路径检测**
   - WorkshopScanner::detectSteamPath() 是静态方法，可以独立调用
   - 支持多个常见 Steam 安装位置

### 版本信息

- **当前版本**: 1.0
- **最后更新**: 2025年11月22日
- **支持的 Qt 版本**: Qt 6.x
- **支持的 C++ 标准**: C++20
- **目标游戏**: RimWorld 1.5+

### 相关文档

- [测试指南](../test_guide.md)
- [如何运行测试](../HOW_TO_RUN_TESTS.md)
- [ModManager 重构文档](../ModManager_Refactoring.md)
- [空白 Mod 列表功能](../empty_modlist_feature.md)

### 联系方式

- **项目地址**: https://github.com/Artorias501/EasyRimWorldModManager
- **问题反馈**: GitHub Issues
