import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12  // 导入 QtGraphicalEffects 模块
import QtQuick.Particles 2.12


import Qt.labs.folderlistmodel 2.2

Item {

    id: root
    width: parent ? parent.width : 375
    height: parent ? parent.height : 667

    // 可绑定属性
    property string dateTextContent: "UOS播放器"
    property string line1Text: "下拉选项查看你的喜好哦 OvO"
    property string line2Text: ""
    property string line3Text: ""


    // 主卡片
    Rectangle {
        id: card
        width: parent ? parent.width * 0.8 : 320
        height: parent ? parent.height * 0.7 : 480
        radius: 20
        anchors.centerIn: parent
         color: "transparent"
        // 主要内容容器

        Column {

            id: contentColumn
            width: parent.width
            anchors.centerIn: parent
            spacing: 0

            // 日期文本动画
            Text {
                id: dateText
                text: root.dateTextContent/////
                font {
                    pixelSize: parent.width * 0.07
                    family: "Microsoft YaHei"
                    weight: Font.Medium
                }
                color: "#333333"
                opacity: 0
                anchors.horizontalCenter: parent.horizontalCenter

                SequentialAnimation on opacity {
                    running: true
                    NumberAnimation {
                        from: 0; to: 1
                        duration: 3000
                        easing.type: Easing.InOutQuad
                    }
                    PauseAnimation { duration: 300 }
                }
            }

            // 分隔线
            Rectangle {
                width: 200
                height: 1
                color: "#e0d7c6"
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.8
            }


            // 主要文本容器
            Column {

                id: textGroup

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                 spacing: width*0.05
                topPadding: 20
                // 文本行动画控制器
                function startAnimations() {
                    line1Anim.restart()
                    line2Anim.restart()
                    line3Anim.restart()
                }

                // 第一行文字
                Text {
                    id: line1
                    text: root.line1Text
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pixelSize: parent.width * 0.05
                        family: "Microsoft YaHei"
                    }
                    color: "#666666"
                    opacity: 0
                    anchors.horizontalCenter: parent.horizontalCenter

                    SequentialAnimation {
                        id: line1Anim
                        NumberAnimation {
                            target: line1
                            property: "opacity"
                            from: 0; to: 1
                            duration: 600
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                // 第二行文字带气泡框
                Item {
                    width: parent.width
                    height: line2.contentHeight + 40

                    // 气泡框
                    Rectangle {
                        id: bubble2
                        width: line2.contentWidth + 40
                        height: line2.contentHeight + 30
                        color: Qt.rgba(0.4, 0.8, 0.4, 0.15)
                        radius: 20
                        anchors.centerIn: line2
                        border.color: "#66bb6a"
                        border.width: 2
                    }

                    Text {
                        id: line2
                        text: root.line2Text
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        font {
                            pixelSize: parent.width * 0.05
                            family: "Microsoft YaHei"
                            weight: Font.DemiBold
                        }
                        color: "#333333"
                        opacity: 0
                        anchors.centerIn: parent

                        SequentialAnimation {
                            id: line2Anim
                            PauseAnimation { duration: 300 }
                            ParallelAnimation {
                                NumberAnimation {
                                    target: line2
                                    property: "opacity"
                                    from: 0; to: 1
                                    duration: 600
                                }
                                NumberAnimation {
                                    target: line2
                                    property: "anchors.verticalCenterOffset"
                                    from: 10; to: 0
                                    duration: 500
                                }
                            }
                        }


                    }
                }

                // 第三行文字带气泡框
                Item {
                    width: parent.width
                    height: line3.contentHeight + 40

                    Rectangle {
                        id: bubble3
                        width: line3.contentWidth + 40
                        height: line3.contentHeight + 30
                        color: Qt.rgba(0.2, 0.5, 0.8, 0.15)
                        radius: 20
                        anchors.centerIn: line3
                        border.color: "#42a5f5"
                        border.width: 2

                    }

                    Text {

                        id: line3
                        text: root.line3Text
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        font {
                            pixelSize: parent.width * 0.03
                            family: "Microsoft YaHei"
                        }
                        color: "#999999"
                        opacity: 0
                        anchors.centerIn: parent

                        elide: Text.ElideRight // 超出部分显示为“……”
                           wrapMode: Text.WrapAtWordBoundaryOrAnywhere // 自动换行
                           maximumLineCount: 7 // 最大行数限制为7行

                        SequentialAnimation {
                            id: line3Anim
                            PauseAnimation { duration: 600 }
                            ParallelAnimation {
                                NumberAnimation {
                                    target: line3
                                    property: "opacity"
                                    from: 0; to: 1
                                    duration: 600
                                }
                                NumberAnimation {
                                    target: line3
                                    property: "anchors.verticalCenterOffset"
                                    from: 10; to: 0
                                    duration: 500
                                }
                            }
                        }
                    }
                }

            }
        }
    }

    // 初始化动画
    Component.onCompleted: textGroup.startAnimations()
    MouseArea {
        id: mouseArea
         x: (parent.width - Math.min(parent.width, parent.height) * 0.6) / 2
         y: (parent.height - Math.min(parent.width, parent.height) * 0.6) / 2
         width: Math.min(parent.width, parent.height) * 0.6
         height: width // 保持正方形
        onClicked: textGroup.startAnimations()
    }
}

