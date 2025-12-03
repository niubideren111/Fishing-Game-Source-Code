
cc.Class({
    extends: cc.Component,

    properties: {

    },

    onLoad() {
        cc.game.setFrameRate(FPS)
        window.GCanvas = cc.find("Canvas")
        window.Game = new (require("GameMonitor"))()
    },

    start() {
        addPrefabUI("SplashUI", true)

        var cdnHost = CDN_HOST
        var imgUrl = SHARE_IMG.replace(/CDN_HOST/, cdnHost)

        if (QQ_GAME) {
            qq.onShareAppMessage(function () {
                return {
                    title: SHARE_MSG,
                    imageUrl: imgUrl,
                    success: (res) => {
                        Game.tipMsg("分享成功")
                        var today = Game.timer.formatDate()
                        Game.localDB.setItem("share_date", today)
                    },
                    fail: (res) => {
                        Game.tipMsg("分享失败")
                    },
                }
            })

            // 显示转发菜单
            qq.showShareMenu(function () {
                return new Promise((resolve, reject) => {
                    qq.showShareMenu({
                        withShareTicket: true,
                        success: res => {
                            resolve(res)
                        },
                    })
                })
            })

            // 保持屏幕常亮
            qq.setKeepScreenOn({ keepScreenOn: true })

        } else if (cc.sys.platform == cc.sys.WECHAT_GAME) {
            wx.onShareAppMessage(function () {
                return {
                    title: SHARE_MSG,
                    imageUrl: imgUrl,
                    success: (res) => {
                        Game.tipMsg("分享成功")
                        var today = Game.timer.formatDate()
                        Game.localDB.setItem("share_date", today)
                    },
                    fail: (res) => {
                        Game.tipMsg("分享失败")
                    },
                }
            })

            // 显示转发菜单
            wx.showShareMenu(function () {
                return new Promise((resolve, reject) => {
                    wx.showShareMenu({
                        withShareTicket: true,
                        success: res => {
                            resolve(res)
                        },
                    })
                })
            })

            // 保持屏幕常亮
            wx.setKeepScreenOn({ keepScreenOn: true })

        } else if (cc.sys.platform == cc.sys.QQ_PLAY) {
            // 保持屏幕常亮
            BK.Device.keepScreenOn({ isKeepOn: true })
        }
    },

    // update (dt) {},
})
