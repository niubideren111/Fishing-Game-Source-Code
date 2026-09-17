# 捕魚原始碼｜Cocos 用戶端、C++ 伺服器端與炮台設定

[简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [English](README.en.md) · [产品页面](https://niubideren111.github.io/Fishing-Game-Source-Code/zh-tw/)

面向街機捕魚玩法的用戶端與伺服器程式碼資料，展示房間選擇、炮台养成和 Boss 战斗效果。公開內容包含 JavaScript 炮台設定、遊戲监控文件、C++ 控製逻辑和通信協議文件。

**捕魚原始碼 · 捕魚遊戲原始碼 · Cocos捕魚原始碼 · C++捕魚伺服器端**

## 專案重點

### 炮台與养成設定

從 BYCannonConfig.js、等級和皮肤設定中檢視字段及設定關系。

### 用戶端與伺服器協作

結合 GameMonitor.js 與通信協議文件理解訊息流转。

### 战斗效果與控製逻辑

房間、养成與 Boss 截圖配合 C++ 控製文件，展示玩法與實作資料。

## 資料閱讀與核對方式

1. **先確認產品形態**：依序檢視截圖與圖說，確認產品類型和可見功能流程。
2. **再核對檔案證據**：直接開啟下方列出的原始碼或文件，不只依賴功能描述。
3. **檢查可建置範圍**：確認欲執行的部分是否具備相依套件、資源、設定與啟動腳本。
4. **確認授權**：閱讀儲存庫授權；商業素材及完整工程交付應另行取得書面授權。

## 產品截圖

![捕魚遊戲房間選擇與快速開始介面](docs/assets/seo/fishing-game-source-code-01.jpg)

![捕魚遊戲养成與锻造介面](docs/assets/seo/fishing-game-source-code-02.jpg)

![捕魚遊戲战斗場景與炮台特效](docs/assets/seo/fishing-game-source-code-03.jpg)

## 公開原始碼與資料

| 文件 | 说明 |
|---|---|
| [BYCannonConfig.js](BYCannonConfig.js) | 炮台基礎設定 |
| [BYCannonLevelConfig.js](BYCannonLevelConfig.js) | 炮台等級設定 |
| [GameMonitor.js](GameMonitor.js) | 用戶端遊戲监控 |
| [bombctrl.cpp](bombctrl.cpp) | C++ 控製逻辑片段 |
| [客户端与服务器通信协议.md](%E5%AE%A2%E6%88%B7%E7%AB%AF%E4%B8%8E%E6%9C%8D%E5%8A%A1%E5%99%A8%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE.md) | 用戶端與伺服器端協議 |
| [服务器间通信协议.md](%E6%9C%8D%E5%8A%A1%E5%99%A8%E9%97%B4%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE.md) | 服務間協議 |

## 開始閱讀

```bash
git clone https://github.com/niubideren111/Fishing-Game-Source-Code.git
cd Fishing-Game-Source-Code
```

## 常見問題

### 公開用戶端文件使用什麼語言？

當前公開設定和逻辑文件主要為 JavaScript，伺服器片段為 C++；產品說明采用 Cocos 用戶端定位。

### 想了解炮台系統從哪里開始？

先閱讀 BYCannonConfig.js，再對照等級、列表和皮肤設定；通信流程檢視協議文件。

## 後續資料完善方向

补充 Cocos 具體版本、可公開的目錄結构、設定字段解释及協議请求响應样例；大資源包放 Releases。 後續更新還應加入版本化相依清單、經過驗證的建置或匯入步驟、簡明架構／產品流程圖，以及能對應真實檔案變更的版本記錄。大型授權資源可放入 GitHub Releases 並提供校驗值，不能提交密鑰、生產位址或使用者資料。

## 相關專案

- [Fishing-Game-Art-Assets](https://github.com/niubideren111/Fishing-Game-Art-Assets)
- [Chess-and-Card-Game-Product-Design-Copy](https://github.com/niubideren111/Chess-and-Card-Game-Product-Design-Copy)

## 資料範圍與授權

公開儲存庫提供程式碼片段、設定表、協議文件和產品截圖；不将當前文件集合描述為可直接啟動的完整遊戲工程。 公開內容以實際檔案、相依套件與授權為準，不承諾搜尋排名、直接上線或固定效能結果。

- Telegram: [@fox_lovemyself](https://t.me/fox_lovemyself)
- GitHub: [Fishing-Game-Source-Code](https://github.com/niubideren111/Fishing-Game-Source-Code)
