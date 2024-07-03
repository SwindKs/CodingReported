// index.js

Page({
  NFCAdapter: null,
  data: {
    userInfo: {
      nickName: '',
    },
      ssid:"",
      pwd:"",
      Ndef:"",
      nfcInfo: {},
      flag:0,

  },
  
  inputSsid(event){
    this.ssid = event.detail.value
  },
  inputPwd(event){
      this.pwd = event.detail.value
  },
  nfcLogin(event){
    wx.showToast({
      title: '写入成功',
      icon:'sucess'
    })
    wx.switchTab({
      url: '../index/index',
    })
  },
 

  
   /**
   * 开始监听 NFC
   */
  NFClistener() {
    this.NFCAdapter.startDiscovery({
      success: () => {
        // 监听 NFC 标签
      },
      fail: error => {
      }
    })
  },

  getNfcInfo(){
    var payload = JSON.stringify({
      "ssid": this.ssid,
      "pwd": this.pwd
    })
    const stringToBuffer = function (str) {
      // 首先将字符串转为16进制
      if (!str) {
          return new Uint8Array().buffer;
      }
      let val = ""
      for (let i = 0; i < str.length; i++) {
          if (val === '') {
              val = str.charCodeAt(i).toString(16)
          } else {
              val += ',' + str.charCodeAt(i).toString(16)
          }
      }
      // 将16进制转化为ArrayBuffer
      return new Uint8Array(val.match(/[\da-f]{2}/gi).map(function (h) {
          return parseInt(h, 16)
      })).buffer
    } 
    var p = stringToBuffer(payload,"02,65,6E");
    console.log(payload)
    console.log(p)
    this.NFCAdapter.onDiscovered(res=>{
      this.NFCAdapter.getNdef().connect({
        success: res => {
          console.log("连接成功");
          const records = [{
            id: new ArrayBuffer(),
            payload: p,
            type: stringToBuffer('T'),
            tnf: 1
          }]
          console.log(this.ssid+"===="+this.pwd)
          this.NFCAdapter.getNdef().writeNdefMessage({
                records: records,               
                success: (res) => {
                  console.log("success!!")
                  console.log(records)
                  console.log(records[0].payload)
                  console.log(records[0].type)
                  console.log('writeNdefMessage success res:', res)
                  this.nfcLogin();
                },
                fail: error => {
                  wx.showToast({
                    title: '写入失败',
                    icon: 'error'
                  })
                  this.nfcLogin();
                },
                complete: (res) => {
                  this.NFCAdapter.stopDiscovery();
                  this.nfcLogin();
                }
          })
    
        },
        fail: error => {
          this.title = '连接标签失败';
          console.error(error);
        },
        complete: res => {
          console.log(res);
        }
      });
    })
    // 连接标签
    this.NFCAdapter.startDiscovery({
      success: () => {
        // 监听 NFC 标签
      },
      fail: error => {
      }
    })
  },

  // discoverHandler(callback) {
  //   console.log("getNfcInfo")
  //   this.getNfcInfo();
  // },


  // closeNFC() {
  //   if (this.NFCAdapter) {
  //     // this.NFCAdapter.offDiscovered(this.discoverHandler)
  //     this.NFCAdapter.stopDiscovery()
  //     this.NFCAdapter = null
  //   }
  // },
 onLoad(){
  this.NFCAdapter = wx.getNFCAdapter()
  this.NFClistener();
 },

   /**
   * 注销 NFCAdapter
   */
  // onHide() {
  //   this.closeNFC()
  // },
  // onUnload: function () {
  //   this.closeNFC()
  // }
})
