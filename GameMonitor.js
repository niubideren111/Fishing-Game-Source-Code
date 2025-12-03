/**
 * 游戏控制类
 */

var Game = cc.Class({
    extends: cc.Component,

    properties: {
        // 场景入口
        _sceneEntry: [],
        // 前台
        _isFore: true,
        // 场景
        _sceneIdx: 0,
        // 自动拉回
        _autoStart: false,
        // 等待UI
        _waitingLayer: null,
        // 更新函数列表
        _updateFunc: [],
        // 延迟回调
        _delayStamp: 0,
        _delayFunc: [],
        // 注册API列表
        _pluginAPI: [],
        _pluginInit: [],
        // API字典
        _apiMap: [],

        // 登录成功后预先获取的数据执行函数
        _preDataList: [],
        _preDataIdx: -1,

        // 功能限制判断函数列表
        _limitFuncList: [],

        // 图集
        _spriteFrames: [],
        preloadState: 0,

        // 连接状态
        _connecting: false,
    },

    ctor() {
        this.init()
    },

    init() {
        this._sceneEntry[ENUM.SCENCE.LOGIN] = "LoginUI"
        this._sceneEntry[ENUM.SCENCE.PLATEFORM] = "HallUI"
        this._sceneEntry[ENUM.SCENCE.FISH] = "FPond"

        this.localDB = cc.sys.localStorage
        this.eventMgr = require("EventManager").getInstance()
        // this.networkMgr = require("NetworkManager").getInstance()
        this.uiManager = require("UIManager").getInstance()
        this.effManager = require("EffectManager").getInstance()
        this.timer = require("Timer").getInstance()
        this.httpCom = require("HttpCom").getInstance()
        this.network = require("Network").getInstance()

        if (QQ_GAME) {
            qq.onShow(this.onComeToForeGround.bind(this))
            qq.onHide(this.onComeToBackGround.bind(this))
            CHANNEL = "qqgame"

        } else if (cc.sys.platform == cc.sys.WECHAT_GAME) {
            wx.onShow(this.onComeToForeGround.bind(this))
            wx.onHide(this.onComeToBackGround.bind(this))

            // var launchOption = wx.getLaunchOptionsSync()
            // console.log(launchOption)
            // if (launchOption.path && launchOption.path !== "index") {
            //     CHANNEL = launchOption.path
            // }
            
        } else if (cc.sys.platform == cc.sys.QQ_PLAY) {
            BK.onEnterForeground(this.onComeToForeGround.bind(this))
            BK.onEnterBackground(this.onComeToBackGround.bind(this))
        }

        this.scheduleUpdate()
    },

    onComeToForeGround() {
        this.dispatchCustomEvent(cc.game.EVENT_SHOW)
    },

    onComeToBackGround() {
        this.dispatchCustomEvent(cc.game.EVENT_HIDE)
    },

    onLoad () {

    },

    start() {

    },

    // Update
    scheduleUpdate() {
        var self = this
        this._updateEntry = this.schedule((dt) => {
            self.update(dt)
        }, 1 / FPS)
    },

    update(dt) {
        // 更新
        for (const key in this._updateFunc) {
            if (this._updateFunc.hasOwnProperty(key) && this._updateFunc[key]) {
                this._updateFunc[key](dt)
            }
        }
        // 延迟执行
        while (this._delayFunc[0] && this._delayFunc[0].stamp < this._delayStamp) {
            this._delayFunc[0].func()
            this._delayFunc.shift()
        }
        // 计时器
        this._delayStamp += dt
        if (this.timer) {
            this.timer.update(dt)
        }
        // todo
        // 声音播放序列
        // if (Audio) {
        //     Audio.playSound_()
        // }
    },

    /**
     * 添加/注册更新函数
     * @param {String} key 关键字
     * @param {Function} func 执行函数
     */
    registerUpdateFunc(key, func) {
        if (key) {
            this._updateFunc[key] = func
        }
    },

    /**
     * 删除/注销更新函数
     * @param {String} key 关键字
     */
    unregisterUpdateFunc(key) {
        if (key) {
            delete this._updateFunc[key]
        }
    },

    /**
     * 添加/注册账号登出重置时执行函数
     * 账号登出时清空数据
     * @param {cc.Class} target 类对象
     * @param {Function} func 执行函数
     */
    registerLogoutReset(target, func) {
        this.addEventListenerWithSceneGraphPriority(target, GEvent("GAME_ON_LOGOUT_EVENT"), func)
    },

    /**
     * 延时回调函数
     * @param {Function} func 关键字
     * @param {Number} delay 执行函数
     * @param {String} key 预留关键字(取消时使用)
     */
    performDelay(func, delay, key) {
        delay || (delay = 0.001)
        var idx = bsearch(this._delayFunc, this._delayStamp + delay, "stamp")
        this._delayFunc.insert(idx + 1, {
            stamp: this._delayStamp + delay,
            func: func,
            key: key
        })
    },

    unperformDelay(key) {
        if (key) {
            for (let i = 0; i < this._delayFunc.length; i++) {
                if (this._delayFunc[i].key == key) {
                    this._delayFunc.remove(i)
                }
            }
        }
    },

    unperformAll() {
        this._delayFunc = []
    },

    // 进入大厅前预先执行的函数
    registerPrepareData(func, toHead) {
        if (toHead) {
            this._preDataList.unshift(func)
        } else {
            this._preDataList.push(func)
        }
    },

    registerPrepareList(funcList, toHead) {
        if (toHead) {
            this._preDataList = funcList.concat(this._preDataList)
        } else {
            this._preDataList = this._preDataList.concat(funcList)
        }
    },

    prepareNext() {
        this.destroyNetBadUI()
        if (this.funcIsOpen("loading")) {
            this.destroyWaitUI()
        }

        this._preDataIdx++
        if (this._preDataIdx >= this._preDataList.length) {
            var self = this
            this.doPluginAPI("update", "loading", 100)
            this.performDelay(() => {
                self.prepareFinish()
            }, 0.1)
        } else {
            this.doPluginAPI("update", "loading", 100 * this._preDataIdx / this._preDataList.length)
            this._preDataList[this._preDataIdx]()
        }
    },

    prepareFinish() {
        this.destroyWaitUI()
        this.unlockTouch()
        if (this._handlePPF) {
            this.unschedule(this._handlePPF)
            this._handlePPF = null
        }
        this._preDataIdx = -1
        if (this._sceneIdx != ENUM.SCENCE.PLATEFORM) {
            this.showWaitUI("正在加载场景", true)
            this.enterScene(ENUM.SCENCE.PLATEFORM)
        } else if (this.funcIsOpen("loading")) {
            this.uiManager.removeLayer("LoadingUI")
        }
    },

    ////////////////////////////////////
    // Background || foreground
    onBack() {
        this._isFore = false
    },

    onFore() {
        this._isFore = true
    },

    isFore() {
        return this._isFore
    },

    showCapture() {

    },

    hideCapture() {

    },

    // 退出游戏
    exitGame(ignoreTip) {

    },

    exitByGame(ignoreTip) {

    },

    // 功能入口开放检测
    checkFuncLimit(funcKey, limit) {
        return {
            open: true,
            tip: FuncListKeyConfig[funcKey].limit_tip
        }
    },

    setFuncLimit(funcKey, limitFunc) {
        if (typeof funcKey == "number" && FuncListConfig[funcKey]) {
            funcKey = FuncListConfig[funcKey].key
        }
        this._limitFuncList[funcKey] = limitFunc
    },

    funcIsOpen(funcKey, params, def) {
        if (typeof funcKey == "number" && FuncListConfig[funcKey]) {
            funcKey = FuncListConfig[funcKey].key
        }
        if (this._limitFuncList[funcKey]) {
            return this._limitFuncList[funcKey](params)
        }

        if (!FuncListKeyConfig[funcKey]) {
            def || (def = true)
            return def
        }

        var state = FuncListKeyConfig[funcKey].state
        return state > 0
    },

    // 资源预加载
    preloadSpriteFrame() {
        var self = this
        cc.loader.loadResDir("atlas", (err, assets) => {
            for (let i = 0; i < assets.length; i++) {
                const v = assets[i];
                if (v instanceof cc.SpriteFrame) {
                    self._spriteFrames[v.name] = v;
                }
            }
            self.preloadState = 2
        })
    },

    preloadSpine(sIdx) {

    },

    preloadShader() {

    },

    preloadMusic() {

    },

    preloadEffect() {

    },

    // 清除所有缓存
    purgeAllCache(ignoreReload) {
        if (this.eventMgr) {
            this.eventMgr.removeAllEvent()
        }

        if (ignoreReload) return

        // todo purge
    },

    getSpriteFrame(icon) {
        if (icon.startWith("res")) {
            icon = icon.slice(4)
        }
        if (icon.endWith("png") || icon.endWith("jpg")) {
            icon = icon.slice(0, -4)
        }
        icon = icon.replace(/\//g, "-")

        return this._spriteFrames[icon]
    },

    // 通信接口
    initNetWork(cbConnSucc, cbConnFail) {
        this.network.initNetWork(SERVER_HOST, cbConnSucc, cbConnFail)
        this.network.startHeartBeat()
    },

    closeNetWork() {
        this.dispatchCustomEvent(GEvent("NET_READY_RECONNECT"), {
            reconnect: true
        })
        this.showWaitUI("服务器连接中")
        this.network.closeNetWork()

        var self = this
        this.performDelay(() => {
            self.initNetWork(() => {
                self.destroyWaitUI()
                self.doPluginAPI("login", "visitor")
            })
        }, 1)
    },

    reconnect() {
        this.network.initNetWork()
    },

    sendData(data, cmd, seqCallback, timeoutCallback) {
        this.network.send(data, cmd, seqCallback, timeoutCallback)
    },

    reqTimeout(cmd) {
        if (this._connecting) return
        this._connecting = true

        cmd = cmd || 0

        if (cmd == ENUM.CMD.HEART_BEAT || true) {
            this.tipNetError()
        } else {
            Game.tipMsg("网络不稳定")
            this._connecting = false
        }
    },

    tipNetError(tip) {
        this.onComeToBackGround()
        this.onComeToForeGround()

        var self = this
        var args = {
            tip: tip || "无法连接网络",
            btnName1: "重连",
            cb1: () => {
                self.closeNetWork()
                self._connecting = false
            },
            btnHide2: true,
            cd1: 15,
        }
        showConfirmTip(args)
    },

    // 登入登出
    preLogin() {
        this.initPlugin("chat")
        this.initPlugin("loading")
        this.initPlugin("service")
        this.initPlugin("set")

        this.initPlugin("login")
    },

    login(callback, updated) {
        // 初始化登录所依赖的其他模块
        this.preLogin()
        this.uiManager.hideLoading()
        this.enterScene(ENUM.SCENCE.LOGIN)

        if (typeof callback == "function") {
            callback()
        }
    },

    onLoginFinished(info) {
        this.dispatchCustomEvent(GEvent("GAME_ON_LOGOUT_EVENT"), {
            reconnect: true
        })
        this.uiManager.hideLoading()

        if (this.funcIsOpen("loading")) {
            this.doPluginAPI("enter", "loading")
        } else {
            this.showWaitUI("正在加载数据", true)
        }

        this.initPlugin()
        this.timer.setCurTimeStamp(info.tick)

        this._preDataIdx = -1
        this.prepareNext()
    },

    onLoginFail(msg) {
        this.destroyNetBadUI()
        this.destroyWaitUI()
        this.uiManager.hideLoading()

        var self = this
        var _resetLogin_ = function () {
            if (self.loginUI) {
                self.loginUI.reset()
            }
        }

        if (parseInt(msg)) {
            this.tipError(msg, 2, _resetLogin_)
        } else if (typeof msg == "string") {
            this.tipMsg(msg, 2, _resetLogin_)
        } else {
            _resetLogin_()
        }
    },

    logOut() {
        this.destroyWaitUI()
        this.network.closeNetWork(true)
        this.dispatchCustomEvent(GEvent("GAME_ON_LOGOUT_EVENT"), {})
    },

    // 插件(功能模块)初始化
    _requireInit(k, init_path) {
        if (init_path === "1") {
            init_path = capitalize(k) + "Init"
        }
        if (!this._pluginInit[init_path]) {
            try {
                var pluginInit = require(init_path)
                if (pluginInit) {
                    pluginInit.init()
                    this._pluginInit[init_path] = pluginInit
                }
            } catch (error) { }
        }
    },

    initPlugin(k) {
        if (k) {
            var v = FuncListKeyConfig[k]
            if (v && !isEmpty(v.init_path)) {
                this._requireInit(k, v.init_path)
            }
        } else {
            for (const key in FuncListKeyConfig) {
                if (FuncListKeyConfig.hasOwnProperty(key)) {
                    const v = FuncListKeyConfig[key]
                    if (v && !isEmpty(v.init_path)) {
                        this._requireInit(key, v.init_path)
                    }
                }
            }
        }
    },

    // 场景流
    preStart(reload) {
        // 预加载
        if (this.preloadState == 0) {
            this.preloadSpriteFrame()
            this.preloadShader()
            this.preloadMusic()
            this.preloadEffect()
        }
    },

    preCheckFunc() {
        if (this.funcIsOpen("lottery") && !ItemFilter.indexOf(ENUM.ITEM_ID.LOTTERY) < 0) {
            ItemFilter.push(ENUM.ITEM_ID.LOTTERY)
        }
    },

    gameStart() {
        cc.log("========GameStart========")
        this.preStart()
        this.preCheckFunc()
        this.login()
    },

    // 视图（场景）切换控制
    registerSceneEntry(idx, path) {
        this._sceneEntry[idx] = path
    },

    enterScene(idx, mix) {
        cc.log("===Enter Scene: " + idx)
        if (this._sceneChanging) return
        this._sceneChanging = true

        this.uiManager.clearAllLayer()
        this.destroyNetBadUI()
        if (this._sceneIdx > ENUM.SCENCE.LOGIN && idx == ENUM.SCENCE.LOGIN) {
            this.resetMonitor()
        }

        var currSceneIdx = this._sceneIdx
        this._sceneIdx = idx
        if (currSceneIdx < ENUM.SCENCE.PLATEFORM || idx != ENUM.SCENCE.PLATEFORM) {
            this.preloadSpine(idx)
            // 进入场景UI
            var uiName = this._sceneEntry[idx]
            addPrefabUI(uiName, true)
        } else if (idx == ENUM.SCENCE.PLATEFORM) {
            if (this.hallUI) {
                this.hallUI.animateIn = true
                this.hallUI.toFront(true)
            }
        }

        // 派发场景切换事件
        this.dispatchCustomEvent(GEvent("CHANGE_SCENE_EVENT"))

        // 回调
        if (typeof mix == "string") {
            addPrefabUI(mix, true)
        } else if (typeof mix == "function") {
            this.performDelay(() => {
                mix()
            }, 0.05)
        }

        var self = this
        this.performDelay(() => {
            self._sceneChanging = false
        }, 0.1)
    },

    setSceneIdx(idx) {
        this._sceneIdx = idx
    },

    getSceneIdx() {
        return this._sceneIdx
    },

    setScene(s) {
        this._scene = s || cc.director.getScene()
    },

    getScene() {
        if (!this._scene) {
            this.setScene()
        }
        return this._scene
    },

    resetMonitor() {
        this.doPluginAPI("clear", "marquee")
    },

    ////////////////////////////////////
    // 是否拉回游戏场景
    isAutoStart() {
        return this._autoStart
    },

    setAutoStart(autoStart) {
        this._autoStart = autoStart
    },

    // 提示网络状态
    showNetCloseTips() {

    },

    // 通用提示（等待）视图
    setWaitLayer(layer) {
        this._waitingLayer = layer
    },

    getWaitLayer() {
        return this._waitingLayer
    },

    // 遮罩提示（等待）对话框
    showWaitUI(tips, ignoreTipNet, callback, spineRes, timeout, ignoreTouch) {
        this.destroyWaitUI()
        this.destroyNetBadUI()

        var self = this
        var args = {
            tips: tips,
            ignoreTipNet: ignoreTipNet,
            callback: callback,
            spineRes: spineRes,
            timeout: timeout,
            ignoreTouch: ignoreTouch,
        }
        addPrefabUI("WaitUI", true, args, (script, view) => {
            self._waitingLayer = script
        })
    },

    destroyWaitUI() {
        if (this._waitingLayer) {
            this._waitingLayer.onClose()
            this._waitingLayer = null
        }
    },

    rebackNetBadUI() {

    },

    destroyNetBadUI() {

    },

    // 通用提示
    tipMsg(tip, dur, cbExit, cbFinish, delay) {
        if (delay) {
            var self = this
            this.performDelay(() => {
                self.tipMsg(tip, dur, cbExit, cbFinish)
            }, delay)
        } else {
            var args = {
                dur: dur,
                tip: tip,
                callback: cbFinish || cbExit,
                zorder: 9999
            }
            addPrefabUI("MsgTipUI", true, args)
        }
    },

    tipError(errCode, dur, cbExit, cbFinish) {
        if (MsgConfig[errCode]){
            this.tipMsg(MsgConfig[errCode].text, dur, cbExit, cbFinish)
        }else{
            cc.log("MsgConfig have not this error Code == " + errCode)
        }
    },

    // 事件分发
    dispatchCustomEvent(eventName, eventData) {
        this.eventMgr.dispatchEvent(eventName, eventData)
    },

    addEventListenerWithFixedPriority(eventName, callback) {
        this.eventMgr.addEventListener(eventName, this, callback)
    },

    addEventListenerWithSceneGraphPriority(node, eventName, callback) {
        this.eventMgr.addEventListener(eventName, node, callback)
    },

    removeEventListener(target) {
        this.eventMgr.removeEventByTarget(target)
    },

    // 添加UI视图，实现对UI的规范管理
    addLayer(layer, zOrder, layerName, isCenter, isRepeat, unManage) {

    },

    // API桥
    getDB(key) {
        key = key.toString()
        if (key.endWith("DB")) {
            return this[key]
        } else {
            return this[key + "DB"]
        }
    },

    getCom(key) {
        key = key.toString()
        if (key.endWith("Com")) {
            return this[key]
        } else {
            return this[key + "Com"]
        }
    },

    registerAPI(key, name, func) {
        var api = key + "_" + name
        this._pluginAPI[api] = func
    },

    registerAPIList(list) {
        if (!isEmpty(list)) {
            for (let i = 0; i < list.length; i++) {
                const v = list[i]
                this.registerAPI(v[0], v[1], v[2])
            }
        }
    },

    // 执行插件(功能模块)API
    doPluginAPI(key, name, ...args) {
        if (isEmpty(key) || isEmpty(name)) {
            return
        }
        if (key === "enter" && !this.funcIsOpen(name)) {
            var limit_tip = FuncListKeyConfig[name] && FuncListKeyConfig[name].limit_tip
            if (isEmpty(limit_tip)) {
                limit_tip = "功能暂未开放，加班制作中。。。"
            }
            this.tipMsg(limit_tip, 2)
            return
        }

        var api = key + "_" + name
        // 通过映射表获取key, name
        if (this._apiMap[api]) {
            key = this._apiMap[api].key
            name = this._apiMap[api].name
        }
        cc.log("doPluginAPI: " + api)
        if (this._pluginAPI[api]) {
            return this._pluginAPI[api](...args)
        }

        if (key != "get") {
            var com_ = this.getCom(key)
            if (com_ && com_[name]) {
                return com_[name](com_, ...args)
            }
            var db_ = this.getDB(key)
            if (db_ && db_[name]) {
                return db_[name](db_, ...args)
            }
        }

        cc.log("API not found: " + api)
    },

    // 执行特效API
    doEffectAPI(effType, ...args) {
        if (this.effManager) {
            this.effManager.doEffectAPI(effType, ...args)
        }
    },

    doEffectList(effType, nodes, inv, delay, ...args) {
        if (!isEmpty(nodes) && this.effManager) {
            if (typeof (nodes) != "table" && nodes.getChildren) {
                nodes = nodes.getChildren()
            }
            if (typeof (inv) != "number") {
                inv = 0.034
            }
            if (typeof (delay) == "number") {
                for (let i = 0; i < nodes.length; i++) {
                    const node = nodes[i]
                    this.effManager.doEffectAPI(effType, node, delay + i * inv, ...args)
                }
            } else {
                var self = this
                if (delay == null) {
                    for (let i = 0; i < nodes.length; i++) {
                        const node = nodes[i]
                        this.performDelay(() => {
                            self.effManager.doEffectAPI(effType, node, ...args)
                        }, i * inv)
                    }
                } else {
                    for (let i = 0; i < nodes.length; i++) {
                        const node = nodes[i]
                        this.performDelay(() => {
                            self.effManager.doEffectAPI(effType, node, delay, ...args)
                        }, i * inv)
                    }
                }
            }
        }
    },

    // 通信桥
    registerParsePack(mixCmd, packKey) {
        if (!isEmpty(mixCmd)) {
            this.network.registerParsePack(mixCmd, packKey)
        }
    },

    unregisterParsePack(mixCmd) {
        if (!isEmpty(mixCmd)) {
            this.network.unRegisterParsePack(mixCmd)
        }
    },

    registerPushMsg(mixCmd, callback) {
        if (!isEmpty(mixCmd)) {
            if (mixCmd instanceof Array) {
                for (let i = 0; i < mixCmd.length; i++) {
                    const v = mixCmd[i]
                    this.network.registerCallBack(v[0], v[1], true)
                }
            } else {
                this.network.registerCallBack(mixCmd, callback, true)
            }
        }
    },

    unregisterPushMsg(mixCmd) {
        if (!isEmpty(mixCmd)) {
            if (mixCmd instanceof Array) {
                for (let i = 0; i < mixCmd.length; i++) {
                    this.network.unRegisterCallBack(mixCmd[i])
                }
            } else {
                this.network.unRegisterCallBack(mixCmd)
            }
        }
    },

    // 全局触摸限制
    lockTouch(duration) {

    },

    unlockTouch() {

    },

    // 敏感词检测
    checkSensitive(text) {

    },

    openGMView() {

    },

    sendGM(cmd) {
        if (GM_DEBUG || this.funcIsOpen("gm")) {
            cmd || (cmd = "coin 100000 1000 100")
            this.sendData([cmd], 22002, (pack, info) => {
                cc.log("GM操作成功")
            })
        }
    },

})

// window.Game || (window.Game = new Game())