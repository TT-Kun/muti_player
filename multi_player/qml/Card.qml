import QtQuick 2.12


import QtQuick.Controls 2.12
//import QtQuick.Effects

Rectangle {

    id:rect
    property alias angle: rotation.angle// // 控制旋转角度（外部可访问）
    property alias image: image.source//// 外部设置图片路径
    property alias content: label.text//// 外部设置文字内容
    property int card_id: cardId_ // 直接使用属性，而不是 alias

    antialiasing:true

    width: 200
    height: 100
    radius: 10
    clip:true

    transform:
        // 用于旋转
        Rotation {
        id: rotation
        origin.x: rect.width / 2 //旋转中心在矩形高度中心
        origin.y: rect.height / 2// 旋转中心在矩形的宽度中心
        axis { x: 0; y: 1; z: 0 } // 沿y轴旋转
        Behavior on angle { SpringAnimation { spring: 2; damping: 0.2 } }
    }

    Behavior on x {
       enabled: visible
        SpringAnimation { spring: 2; damping: 0.2 }
    }
    Behavior on y {
        enabled: visible
        SpringAnimation { spring: 2; damping: 0.2 }
    }
    Behavior on width {
        //表示动画行为仅在组件的 visible 属性为 true 时启用。
        enabled: visible
        NumberAnimation {
            duration: 200
        }
    }
    Behavior on height {
        enabled: visible
        NumberAnimation {
            duration: 200
        }
    }
    Column {
        anchors.top: rect.top
        anchors.left: rect.left
        anchors.right: rect.right
        anchors.bottom: rect.bottom
        anchors.margins: 10
        spacing: 10
        Item {
            id:avatarItem
            width: parent.height /2
            anchors.horizontalCenter: parent.horizontalCenter
            height: width
            Image {
                id:image
                anchors.fill: parent
                asynchronous: true
                cache: true
                fillMode: Image.PreserveAspectCrop
                source: rect.image
                visible: true
                antialiasing: true
            }
        }
        Label {
            id: label
             visible: true
            text:rect.content
            wrapMode: Label.WrapAnywhere
            width: parent.width // 明确设置 Label 的宽度
            leftPadding: 10
            rightPadding: 10
            horizontalAlignment: Text.AlignHCenter
            font {
                pixelSize: parent.width * 0.2
                family: "Microsoft YaHei"
                weight: Font.DemiBold
            }
        }
    }
}
