# 项目构建
此项目使用cmake与qt
qt路径:

# 关于mod
其中有两个全局变量需要关注：
1. 当前加载的mod
位于C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
其中\<activateMods\>是需要关注的标签，里面的mod按照顺序加载
示例：
``` xml
<?xml version="1.0" encoding="utf-8"?>
<ModsConfigData>
  <version>1.6.4633 rev1261</version>
  <activeMods>
    <li>zerowhite.dailyfurniture</li>
    <li>ponpeco.pfkidsroom</li>
    <li>ponpeco.pfkschooldesk</li>
    <li>zh.mzmgow.ponpeco.pf</li>
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
