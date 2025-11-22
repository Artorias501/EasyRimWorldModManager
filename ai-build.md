# 项目构建

此项目使用cmake与qt
qt路径:

# 关于mod

其中有两个全局变量需要关注：

1. 当前加载的mod
   位于C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
   
   **重要说明：**
   - `<version>`: 游戏版本信息，不可修改，必须保留原样
   - `<activeMods>`: 包含所有激活的mod（核心、官方DLC、创意工坊mod），按加载顺序排列
   - `<knownExpansions>`: 仅包含官方DLC的packageId，用于标识用户拥有哪些DLC，不可修改
   
   **注意：** 
   - activeMods中包含的官方DLC也会出现在knownExpansions中
   - knownExpansions不包含核心mod（ludeon.rimworld）和创意工坊mod
   - 保存配置时必须保留原始的version和knownExpansions，只修改activeMods

    示例：

``` xml
<?xml version="1.0" encoding="utf-8"?>
<ModsConfigData>
  <version>1.6.4633 rev1261</version>
  <activeMods>
    <li>brrainz.harmony</li>
    <li>ludeon.rimworld</li>
    <li>ludeon.rimworld.royalty</li>
    <li>ludeon.rimworld.ideology</li>
    <li>ludeon.rimworld.biotech</li>
    <li>ludeon.rimworld.odyssey</li>
    <li>zerowhite.dailyfurniture</li>
    <li>ponpeco.pfkidsroom</li>
  </activeMods>
  <knownExpansions>
    <li>ludeon.rimworld.royalty</li>
    <li>ludeon.rimworld.ideology</li>
    <li>ludeon.rimworld.biotech</li>
    <li>ludeon.rimworld.odyssey</li>
  </knownExpansions>
</ModsConfigData>
```

2. steam创意工坊加载的mod
   位于：{steam安装路径}\steamapps\workshop\content\294100
   该目录下有许多文件夹，每个文件夹是一个mod
   mod文件夹中有有一个about文件夹，其中的About.xml是需要读取的mod信息
   例如：D:\Steam\steamapps\workshop\content\294100\3585148922\About\About.xml
   D:\Steam\steamapps\workshop\content\294100\3575567766\About\About.xml

3. 官方DLC
   位于：{游戏安装路径}\Data
   游戏安装路径通常为：{steam安装路径}\steamapps\common\RimWorld
   完整路径示例：D:\Steam\steamapps\common\RimWorld\Data
   
   该目录下有多个文件夹，每个文件夹是一个官方DLC（包括核心游戏）
   DLC文件夹中也有About文件夹，其中的About.xml包含DLC信息
   官方DLC需要与普通mod区分，以便在加载列表中正确处理

About.xml中标签的作用：
参照https://rimworld.huijiwiki.com/wiki/Mod_%E5%88%B6%E4%BD%9C_-_About.xml

# 数据结构

## mod项

需要存储：

- 标识符(唯一标识)
- 名称
- 描述
- 作者名
- 依赖
- 前置mod(在~之前加载)
- 后置mod(在~之后加载)
- 不兼容模组
- 备注（用户自定义）
- 类型（核心、DLC、前置框架、逻辑mod、功能性mod、种族mod、种族扩展mod、单一功能mod、汉化、优化等，可扩展）
- 是否为官方DLC（布尔值）
- 来源路径（用于区分不同来源的mod）

# 用户数据管理

## UserDataManager

用户数据管理器负责持久化用户的自定义数据，存储在程序同目录的 `UserData` 文件夹下。

### 目录结构
```
UserData/
├── Mod/
│   ├── mod_data.json          # Mod类型和备注映射
│   └── custom_types.json      # 自定义类型列表
└── ModList/
    ├── my_modlist_1.xml       # 用户保存的Mod列表
    ├── my_modlist_2.xml
    └── ...
```

### 功能

#### 1. Mod类型和备注管理
- 为每个Mod设置类型（通过PackageId映射）
- 为每个Mod添加备注
- 管理自定义类型列表（可增删）

#### 2. Mod列表管理
- 保存编辑后的Mod加载列表
- 支持另存为（保存到指定路径）
- 支持直接覆盖游戏配置文件
- 自动保留原文件的所有字段（除 activeMods 和 knownExpansions）

### 数据格式

#### mod_data.json
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

#### custom_types.json
```json
{
  "customTypes": [
    "美化mod",
    "剧情mod",
    "战斗增强"
  ]
}
```

### 使用示例

```cpp
// 创建管理器
UserDataManager manager;

// 加载所有数据
manager.loadAll();

// 设置Mod类型和备注
manager.setModType("brrainz.harmony", "前置框架");
manager.setModRemark("brrainz.harmony", "重要的前置mod");

// 添加自定义类型
manager.addCustomType("美化mod");

// 保存所有数据
manager.saveAll();

// 保存Mod列表
manager.saveModListToPath(
    "path/to/modlist.xml",
    activeMods,
    knownExpansions,
    version,
    otherFields
);
```

### 注意事项

1. **ModConfigManager 的改进**：
   - 现在会自动保存除 `activeMods` 和 `knownExpansions` 外的所有XML字段
   - 使用 `getOtherFields()` 获取其他字段，用于完整保存

2. **数据持久化**：
   - 程序启动时应调用 `loadAll()` 加载用户数据
   - 数据修改后应调用 `saveAll()` 保存
   - Mod列表可以按需保存，支持多个不同的配置

3. **路径管理**：
   - 所有数据自动保存在程序目录下的 `UserData` 文件夹
   - 首次运行会自动创建必要的目录结构
