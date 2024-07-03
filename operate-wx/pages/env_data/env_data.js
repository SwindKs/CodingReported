
Page({
  data: {
    ETOKEN:'',
    temp:0,
    humi:0,
    light:0,
    smoke:0,
    co2:0,
    tvoc:0,
    temp_flag:"",
    humi_flag:"",
    smoke_flag:"",
    co2_flag:"",
  },
  getEnv_Data(){
    var app = getApp();
    var that = this;

    wx.request({
      url: 'https://babf0e90df.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/837ed7d0cecd4d8da880df01049c978d/devices/663ddea16bc31504f069b342_12138178/shadow',

      method: 'get', //请求的方式
      header:{ 'Content-Type':'application/json','X-Auth-Token':this.ETOKEN},
        data:{ },
        success: (res) => {//请求成功之后的回调函数
            console.log(res.data.shadow[0].reported.properties)//查看真实数据
            this.setData({
              temp:JSON.stringify(res.data.shadow[0].reported.properties.Temperature),
              light:JSON.stringify(res.data.shadow[0].reported.properties.Light),
              humi:JSON.stringify(res.data.shadow[0].reported.properties.Humidity),
              co2 : JSON.stringify(res.data.shadow[0].reported.properties.CO2),
              tvoc : JSON.stringify(res.data.shadow[0].reported.properties.TVOC),
              smoke : JSON.stringify(res.data.shadow[0].reported.properties.Smoke),
            })
        }
    })
    console.log(this.data.smoke)
    app.globalData.temp = this.data.temp
    app.globalData.light = this.data.light
    app.globalData.humi = this.data.humi
    app.globalData.smoke = this.data.smoke
    app.globalData.co2 = this.data.co2
    app.globalData.tvoc = this.data.tvoc

    if(this.data.smoke<15||this.data.temp>=40||this.data.humi>80||this.data.co2>1500){
      app.globalData.content = "环境异常，请及时查看数据!!!"
    }else{
      app.globalData.content = "环境正常"
    }
    if(this.data.smoke<15){
      this.setData({
        smoke_flag:"red"
      })
    }else{
      this.setData({
        smoke_flag:""
      })
    }
    if(this.data.temp>=40){
      this.setData({
        temp_flag:"red"
      })
    }else{
      this.setData({
        temp_flag:""
      })
    }
    if(this.data.humi>80){
      this.setData({
        humi_flag:"red"
      })
    }else{
      this.setData({
        humi_flag:""
      })
    }
    if(this.data.co2>1500){
      this.setData({
        co2_flag:"red"
      })
    }else{
      this.setData({
        co2_flag:""
      })
    }
    
  },
  startPolling() {
    setInterval(() => {
      this.getEnv_Data();
      this.getDataList();
    }, 3000); // 每8秒请求一次数据
  },

  getDataList(){
    wx.request({
      url: 'http://localhost:8080/data/insert',
      method: 'get', //请求的方式
        data:{ //发送到服务器的数据
            temp: this.data.temp,
            humi: this.data.humi,
            light:this.data.light,
            smoke: this.data.smoke,
            co2 : this.data.co2,
            tvoc:this.data.tvoc
        },
        success: (res) => {//请求成功之后的回调函数
        }
    })
  },

onShow() {

    this.ETOKEN = getApp().globalData.userToken
    this.getEnv_Data()
    this.startPolling()
    // console.log(this.ETOKEN);
},


})
