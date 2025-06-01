import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtMultimedia 5.14

Item {

   id: gen
    property string theme: "light"

    Logic{
        id:logic
    }

    Connections {
         target: backend
         onThemeChanged: (type) => {
                             if(type==1){
                                 bgImage.source = backgroundContainer.image4
                             }else if(type==0){
                                 bgImage.source = backgroundContainer.image1
                             }else{
                                 console.log("jinlingll")
                             }

                         }
     }

    Rectangle {
        id: backgroundContainer
        anchors.fill: parent

        // 背景图片资源属性
        property string image1: "qrc:/pitrue/bear.jpg"
        property string image4: "qrc:/pitrue/brid.jpg"

        // 背景图片显示
        Image {
            id: bgImage
            anchors.fill: parent
            source: backgroundContainer.image1
            fillMode: Image.PreserveAspectCrop
            mipmap: true
        }

    }



    // 自定义页面组件
    CustomPage {
        id: mainPage
        anchors.fill: parent
        // 可添加自定义属性绑定...
    }

    // 记忆卡片组件
    MemoryCard{
        id: memoryCard
        anchors.fill: parent
    }


    // 虚拟小人组件
       Image {
           id: character
           property int index_for_switch: 0
           property string character_contenxt: "处理中，请耐心等待。。。。"
           width: parent.width * 0.1  // 绑定父组件宽度的20%
           height: (implicitHeight / implicitWidth) * width
              x: parent.width * 0.5  // 初始位置居中
              y: parent.height * 0.5


              property string image1: "qrc:/pitrue/flag.png"

             source: image1


              // 气泡组件
                 Component {
                     id: bubbleComponent
                     Item {
                         id: bubble
                         width: bubbleBg.width
                         height: bubbleBg.height + textItem.height + 10
                         visible: true

                         // 气泡背景
                         Rectangle {
                             id: bubbleBg

                             width: textItem.width + 30   // 保持原有边距逻辑
                             height: textItem.height + 20
                             color: "white"              // 背景色（原BorderImage可能缺失颜色）

                             // 边框设置
                             border {
                                 color: "red"           // 边框颜色
                                 width: 3               // 边框宽度（像素）
                             }

                             radius: 5                  // 圆角（替代九宫格图片的圆角效果）

                             anchors.top: parent.top
                             anchors.right: parent.right
                         }

                         // 气泡文字ocol "" is unknown
                         Text {
                             id: textItem

                             text: character.character_contenxt
                             color: "black"
                             font.pixelSize: character.height * 0.25
                             width: character.width * 3.0  // 设置固定宽度（按10字宽度估算）
                              wrapMode: Text.Wrap // 超宽时自动换行
                              elide: Text.ElideNone // 禁用省略号（与换行共存时优先换行）
                               maximumLineCount: 0 // 0 表示不限行数（默认值，可不写）
                             anchors {
                                 verticalCenter: bubbleBg.verticalCenter
                                 right: bubbleBg.right
                                 rightMargin: 15
                             }
                         }

                         // 自动消失动画
                         Timer {
                             interval: 10000
                             running: true
                             onTriggered: bubble.destroy()
                         }

                         // 入场动画
                         PropertyAnimation {
                             target: bubble
                             property: "opacity"
                             from: 0
                             to: 1
                             duration: 300
                             running: true
                         }
                     }
                 }

                 function createBubble() {
                     let bubble = bubbleComponent.createObject(character, {
                         x: character.width,  // 右侧偏移
                         y: -30  // 顶部偏移
                     });
                     return bubble;
                 }
           // 拖动逻辑
           MouseArea {
               anchors.fill: parent
               drag {
                        target: parent
                        axis: Drag.XAndYAxis
                        minimumX: 0
                        maximumX: parent.parent.width - character.width
                        minimumY: 0
                        maximumY: parent.parent.height - character.height
                    }

               onClicked: {
                   if(character.index_for_switch==0){
                       character.index_for_switch=1
                       logic.upload_av()
                   }else{
                       character.character_contenxt="处理中，请耐心等待。。。。"
                   }
                      character.createBubble();
               }
           }

       }


}
