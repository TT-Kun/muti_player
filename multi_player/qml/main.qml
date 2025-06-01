import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.14



Item {
//    id: rootItem  // 给根元素命名
//       width: parent ? parent.width : 0  // 安全访问父属性
//       height: parent ? parent.height : 0




//    Image {//悬浮人物
//        id: character
//        width: parent.width * 0.1  // 绑定父组件宽度的20%
//        height: (implicitHeight / implicitWidth) * width
//           x: parent.width * 0.5  // 初始位置居中
//           y: parent.height * 0.5



//        source: "qrc:/pitrue/flag.png"  // 替换为你的图片路径


//           // 气泡组件
//              Component {
//                  id: bubbleComponent
//                  Item {
//                      id: bubble
//                      width: bubbleBg.width
//                      height: bubbleBg.height + textItem.height + 10
//                      visible: true

//                      // 气泡背景
//                      Rectangle {
//                          id: bubbleBg

//                          width: textItem.width + 30   // 保持原有边距逻辑
//                          height: textItem.height + 20
//                          color: "white"              // 背景色（原BorderImage可能缺失颜色）

//                          // 边框设置
//                          border {
//                              color: "red"           // 边框颜色
//                              width: 3               // 边框宽度（像素）
//                          }

//                          radius: 5                  // 圆角（替代九宫格图片的圆角效果）

//                          anchors.top: parent.top
//                          anchors.right: parent.right
//                      }

//                      // 气泡文字
//                      Text {
//                          id: textItem
//                          text: "getmefadksuhfkahsdkfjhakdshfkasdhfklasdwo de htian  WLFJA 艾米诺士 打飞机了多少积分都发大水阿迪斯发"
//                          color: "black"
//                          font.pixelSize: character.height * 0.25
//                          width: character.width * 3.0  // 设置固定宽度（按10字宽度估算）
//                           wrapMode: Text.Wrap // 超宽时自动换行
//                           elide: Text.ElideNone // 禁用省略号（与换行共存时优先换行）
//                            maximumLineCount: 0 // 0 表示不限行数（默认值，可不写）
//                          anchors {
//                              verticalCenter: bubbleBg.verticalCenter
//                              right: bubbleBg.right
//                              rightMargin: 15
//                          }
//                      }

//                      // 自动消失动画
//                      Timer {
//                          interval: 3000
//                          running: true
//                          onTriggered: bubble.destroy()
//                      }

//                      // 入场动画
//                      PropertyAnimation {
//                          target: bubble
//                          property: "opacity"
//                          from: 0
//                          to: 1
//                          duration: 300
//                          running: true
//                      }
//                  }
//              }

//              MouseArea {
//                      anchors.fill: parent
//                      drag {
//                          target: character
//                          axis: Drag.XAndYAxis
//                          minimumX: 0
//                          maximumX: rootItem.width - character.width  // 使用根元素尺寸
//                          minimumY: 0
//                          maximumY: rootItem.height - character.height
//                      }
//                      onClicked: {


//                          let bubble = bubbleComponent.createObject(character, {
//                              x: character.width,
//                              y: -30
//                          })
//                      }
//                  }
//    }


}














