// app.js
App({
  onLaunch() {
    // 展示本地存储能力
    const logs = wx.getStorageSync('logs') || []
    logs.unshift(Date.now())
    wx.setStorageSync('logs', logs)

    // 登录
    wx.login({
      success: res => {
        // 发送 res.code 到后台换取 openId, sessionKey, unionId
      }
    })
  },
  globalData: {
    userInfo: null,
    userToken:'',
    client: null,
    deviceId: '',
    temp:0,
    humi:0,
    light:0,
    smoke:0,
    co2:0,
    tvoc:0, 
    content:"环境正常",
  }

})
