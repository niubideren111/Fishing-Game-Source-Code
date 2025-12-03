// FPS
window.FPS = 30

// 游戏版本号
window.GAME_VERSION = "1.00.00"
window.GAME_AGENT = 100

// 平台,马甲,包名,刘海宽度）
window.MARKET = "wechat"
window.CHANNEL = "wechat"
window.APPNAME = "捕鱼圣手H5"
window.DEBUG_SIDEMARGIN = 0
window.DEBUG_LANDSCAPELEFT = true
window.MIDAS_APPID = "1450017776"
window.MIDAS_RATE = 10
window.MIDAS_SHOPITEM = 0 // 101
window.MIDAS_ENV = 0 // 0:正式  1:沙箱

// 单机模式调试 (PC微信登录开启)
window.DEBUG_OFFLINE = false
window.QQ_GAME = (typeof qq != "undefined")

// 是否为开发模式 [正式包false,开发测试包true]
window.DEVELOP_MODE = false
// GM调试
window.GM_DEBUG = false
// 渔场显示击杀数据
window.FISH_DEBUG = false
// 显示鱼的碰撞区域
window.SHOW_FISH_FRAME = false

// 显示Cocos状态信息
window.CC_SHOW_FPS = false

// 热更新相关 (框架ID,主游戏ID,资源后缀)
window.GAME_FRAME_ID = 1000
window.GAME_MAIN_ID = 1018
window.GAME_MAIN_KEY = "fish"
window.GAME_RES_EXT = ".x"

// APK静默下载
window.APK_SILENT_DOWN = false

// 优先检测TP纹理集
window.CHECK_PLIST_TEX = true //true

// 预设服务器HOST
window.SERVER_HOST = "wss://server.fishwx.poker3a.com:12510" // 外网
// window.SERVER_HOST = "wss://test-server.fishwx.poker3a.com:12510" // 外网测试
// window.SERVER_HOST = "ws://192.168.30.192:20720" // 曾毅服
// window.SERVER_HOST = "ws://192.168.60.31:38720" // 内网测试
window.PHP_HOST = "https://sharepage.fish.51738.com/"
// window.PHP_HOST = "http://cver.fish.poker3a.com:12199/"
// window.PHP_HOST = "http://test-cver.fish.mb1768.cn:12199/"
window.CDN_HOST = "https://client.fish.poker3a.com/cdn_host/wgame"
window.SHARE_IMG = "CDN_HOST/share/share1.jpg"
window.SHARE_PAGE = "http://by.mmcy808.com/"
window.SHARE_MSG = "千炮一响，黄金万两！各种弹头爆不停！"
window.CHEAT_TOKEN = "46D6C2CF850EABEF76C371D792E6968D"

// 全局
window.slg_cmd = {}
window.slg_protocol = {}
// 刘海宽度
window.BANG_WIDTH = 56
window.CC_DESIGN_RESOLUTION = {
    width : 1280,
    height : 720,
    cx : 640,
    cy : 360,
    autoscale : "UNKNOWN"
}