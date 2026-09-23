# 捕鱼游戏源码｜街机捕鱼、Cocos 客户端与 C++ 服务端资料

[简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [English](README.en.md) · [产品页面](https://niubideren111.github.io/Fishing-Game-Source-Code/zh-cn/)

面向街机捕鱼玩法的客户端与服务器代码资料，展示房间选择、炮台养成和 Boss 战斗效果。公开内容包含 JavaScript 炮台配置、游戏监控文件、C++ 控制逻辑和通信协议文档。

**捕鱼源码 · 捕鱼游戏源码 · 街机捕鱼源码 · 打鱼游戏源码 · Cocos 捕鱼源码**

## 核心卖点

- **街机捕鱼产品形态**：截图覆盖房间选择、快速开始、炮台养成、锻造和实时战斗场景。
- **炮台配置体系**：公开炮台基础、等级、列表和皮肤配置，便于理解成长字段之间的关系。
- **客户端与服务端资料**：JavaScript 客户端文件配合 C++ 控制逻辑，展示前后端协作线索。
- **通信协议文档**：提供客户端与服务器、服务器之间的二进制消息及 MsgPack 数据说明。
- **战斗控制片段**：包含炸弹、宝石和其他控制逻辑文件，可用于研究奖励和战斗流程。
- **图文多语言页面**：提供简体中文、繁體中文、English README 和 GitHub Pages 展示页。

## 技术架构

| 层级 | 公开技术与文件 |
|---|---|
| Cocos 客户端资料 | JavaScript 配置、`main.js`、`GameMonitor.js` 及 Unity 风格 `.meta` 文件 |
| 游戏配置 | `BYCannonConfig.js`、等级、列表、皮肤和表情配置 |
| C++ 逻辑片段 | `bombctrl.cpp`、`rb_ctrl.cpp`、`wctrl35.cpp`、`wctrlbaoshi.cpp` |
| 通信协议 | 固定包头、二进制字节流、MsgPack 包体及服务编号约定 |
| 构建资料 | `Makefile` 与公开代码文件 |
| 产品资料 | 房间、养成、锻造、炮台和战斗效果截图 |

资料关系可概括为：客户端配置和监控负责表现与状态入口，通信协议连接游戏服务，C++ 文件承接部分战斗及控制逻辑。完整构建仍需确认引擎版本、依赖、资源、入口工程和服务器环境。

## 游戏内容

| 内容 | 公开资料呈现 |
|---|---|
| 房间选择 | 多个房间入口及快速开始界面 |
| 炮台系统 | 炮台基础属性、等级、列表、皮肤和外观配置 |
| 养成与锻造 | 炮台成长及锻造产品界面 |
| 捕鱼战斗 | 鱼群、炮台发射、命中特效及奖励反馈画面 |
| Boss 场景 | 产品截图展示大型目标与战斗效果 |
| 炸弹与特殊奖励 | C++ 炸弹使用、宝石和控制逻辑片段 |
| 社交表达 | `BYEmojiConfig.js` 表情配置资料 |
| 排行资料 | 公开排行榜表格及相关产品数据资料 |

## 玩家体验

- **进入路径清楚**：房间选择和快速开始缩短进入牌局的步骤。
- **成长目标可见**：炮台等级、皮肤、养成和锻造形成持续升级目标。
- **战斗反馈直观**：炮弹、命中、鱼群和奖励特效强化即时反馈。
- **阶段变化丰富**：普通捕鱼、特殊奖励与 Boss 场景形成节奏变化。
- **配置便于调优**：炮台与等级配置可作为体验平衡和版本迭代的阅读入口。

玩家体验描述来自公开截图和配置结构，实际手感、性能与数值平衡需要在完整工程中通过真机和服务器联调验证。

## 项目重点

### 炮台与养成配置

从 BYCannonConfig.js、等级和皮肤配置中查看字段及配置关系。

### 客户端与服务器协作

结合 GameMonitor.js 与通信协议文档理解消息流转。

### 战斗效果与控制逻辑

房间、养成与 Boss 截图配合 C++ 控制文件，展示玩法与实现资料。

## 资料阅读与核对方式

1. **先确认产品形态**：依次查看截图和图注，确认产品类型与可见功能流程。
2. **再核对文件证据**：直接打开下方列出的源码或文档，不只依赖功能描述。
3. **检查可构建范围**：确认准备运行的部分是否具备依赖、资源、配置和启动脚本。
4. **确认授权**：阅读仓库许可；商业素材及完整工程交付应另行取得书面授权。

## 产品截图

![捕鱼游戏房间选择与快速开始界面](docs/assets/seo/fishing-game-source-code-01.jpg)

![捕鱼游戏养成与锻造界面](docs/assets/seo/fishing-game-source-code-02.jpg)

![捕鱼游戏战斗场景与炮台特效](docs/assets/seo/fishing-game-source-code-03.jpg)

## 公开源码与资料

| 文件 | 说明 |
|---|---|
| [BYCannonConfig.js](BYCannonConfig.js) | 炮台基础配置 |
| [BYCannonLevelConfig.js](BYCannonLevelConfig.js) | 炮台等级配置 |
| [GameMonitor.js](GameMonitor.js) | 客户端游戏监控 |
| [bombctrl.cpp](bombctrl.cpp) | C++ 控制逻辑片段 |
| [客户端与服务器通信协议.md](%E5%AE%A2%E6%88%B7%E7%AB%AF%E4%B8%8E%E6%9C%8D%E5%8A%A1%E5%99%A8%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE.md) | 客户端与服务端协议 |
| [服务器间通信协议.md](%E6%9C%8D%E5%8A%A1%E5%99%A8%E9%97%B4%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE.md) | 服务间协议 |

## 开始阅读

```bash
git clone https://github.com/niubideren111/Fishing-Game-Source-Code.git
cd Fishing-Game-Source-Code
```

## 常见问题

### 公开客户端文件使用什么语言？

当前公开配置和逻辑文件主要为 JavaScript，服务器片段为 C++；产品说明采用 Cocos 客户端定位。

### 想了解炮台系统从哪里开始？

先阅读 BYCannonConfig.js，再对照等级、列表和皮肤配置；通信流程查看协议文档。

## 后续资料完善方向

补充 Cocos 具体版本、可公开的目录结构、配置字段解释及协议请求响应样例；大资源包放 Releases。 后续更新还应加入版本化依赖清单、经过验证的构建或导入步骤、简明架构/产品流程图，以及能对应真实文件变化的版本记录。大型授权资源可放入 GitHub Releases 并提供校验值，不能提交密钥、生产地址或用户数据。

## 相关项目

- [Fishing-Game-Art-Assets](https://github.com/niubideren111/Fishing-Game-Art-Assets)
- [Chess-and-Card-Game-Product-Design-Copy](https://github.com/niubideren111/Chess-and-Card-Game-Product-Design-Copy)

## 资料范围与许可

公开仓库提供代码片段、配置表、协议文档和产品截图；不将当前文件集合描述为可直接启动的完整游戏工程。 公开内容以实际文件、依赖和许可为准，不承诺搜索排名、直接上线或固定性能结果。

- Telegram: [@fox_lovemyself](https://t.me/fox_lovemyself)
- GitHub: [Fishing-Game-Source-Code](https://github.com/niubideren111/Fishing-Game-Source-Code)
