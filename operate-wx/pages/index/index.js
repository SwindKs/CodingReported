// index.js

Page({
  data: {
    UTOKEN:'',
    flag_container:false,
    btn_flag:true,
    flag_bottom:false,
    flag_top:false,
    webUrl:0,
    content:"",
    html:'<img style=" width:100%;height:auto;display:block" src="http://192.168.233.5:81/stream" />',
    },
    getToken(){
      wx.request({
        url: 'https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens',
        method: 'post', //请求的方式
        header:{'Content-Type': 'application/json'},
          data:{ //发送到服务器的数据
              "auth": {
                "identity": {
                  "methods": ["password"],
                  "password": {
                    "user": {
                      "domain": {
                        "name": "swinks"
                      },
                      "name": "iXOC",
                      "password": "ochuawei123++"
                    }
                  }
                },
                "scope": {
                  "domain": {
                    "name": "swinks"
                  }
                }
              }
          },
          success: (res) => {//请求成功之后的回调函数
              this.UTOKEN=res.header["X-Subject-Token"]
              getApp().globalData.userToken = res.header["X-Subject-Token"]
              wx.showToast({
                title:'小车连接成功!',
                icon:'success',
                mask:true
              })
              //  console.log(this.UTOKEN)
          },
          fail:()=>{
            wx.showToast({
              title:'小车连接失败!',
              icon:'error',
              mask:true
            })
          }
      })
    },

  operate(event){
    wx.request({
      url: 'https://babf0e90df.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/837ed7d0cecd4d8da880df01049c978d/devices/663ddea16bc31504f069b342_12138178/commands',
      method: 'post', //请求的方式
      header:{ 'Content-Type': 'application/json','X-Auth-Token':this.UTOKEN},
        data:{ //发送到服务器的数据
          "service_id": "qq123",
          "command_name": "Car_Controller",
          "paras": {
           "Control_cmd": event.currentTarget.id
          }
        },
        success: (res) => {//请求成功之后的回调函数
            console.log(res)//查看真实数据 console.log(res.data)            
        }
         
    })
  },
  HandOperate(event){
    wx.request({
      url: 'https://babf0e90df.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/837ed7d0cecd4d8da880df01049c978d/devices/663ddea16bc31504f069b342_12138178/commands',
      method: 'post', //请求的方式
      header:{ 'Content-Type': 'application/json','X-Auth-Token':this.UTOKEN},
        data:{ //发送到服务器的数据
          "service_id": "qq123",
          "command_name": "Work_Mode",
          "paras": {
           "Mode_cmd": "CONTROL"
          }
        },
        success: (res) => {//请求成功之后的回调函数
            console.log(res)//查看真实数据 console.log(res.data)            
        }
    })
  },
  AutoOperate(event){
    wx.request({
      url: 'https://babf0e90df.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/837ed7d0cecd4d8da880df01049c978d/devices/663ddea16bc31504f069b342_12138178/commands',
      method: 'post', //请求的方式
      header:{ 'Content-Type': 'application/json','X-Auth-Token':this.UTOKEN},
        data:{ //发送到服务器的数据
          "service_id": "qq123",
          "command_name": "Work_Mode",
          "paras": {
           "Mode_cmd": "AUTO"
          }
        },
        success: (res) => {//请求成功之后的回调函数
            console.log(res)//查看真实数据 console.log(res.data)            
        }
    })
  },
  showFlag(event){
    if(event.currentTarget.id=='auto'){
      this.setData({
        btn_flag:false,
        flag_container:true,
        flag_top:true,
      })
      this.AutoOperate();
    }
    if(event.currentTarget.id=='hand'){
      this.setData({
        btn_flag:false,
        flag_container:true,
        flag_top:true,
        flag_bottom:true,
      })
      this.HandOperate();
      console.log(event.currentTarget.id)
    }
  },

  startPolling() {
    setInterval(() => {
      this.setData({
        content:getApp().globalData.content
      })
    }, 8000); // 每8秒请求一次数据
  },
  backMain(){
    this.setData({
      btn_flag:true,
      flag_bottom:false,
      flag_top:false,
    })
  },
  onLoad: function (options) {
     this.getToken()
     this.setData({
      content:getApp().globalData.content
    })
     this.startPolling();
  }
})
