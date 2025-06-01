import QtQuick 2.12
import QtQuick.Controls 2.12

Page{

    id:page
    clip: true//超过部分裁剪
    property alias gradient: row.gradient
    property var grad
      background: Rectangle {
          color: "transparent"
      }
    contentItem:
        CardRow {
        color: "transparent"
        id:row
        anchors.verticalCenter: parent.verticalCenter//垂直中心线与父组件的垂直中心线对齐
        model:[
            {img:"qrc:/pitrue/image5.jpg",text:"风格",id:1},
            {img:"qrc:/pitrue/image4.jpg",text:"能量",id:2},
        ]
        //用户拖拽卡片
        onCanPull:(cardItem)=> {
                      // 覆盖卡片位置 记录梯度
                      transitionRect.fillItem(cardItem)
                      grad = cardItem.gradient
                      transitionRect.canPush = false
                  }
        // 取消拖拽
        onCancelPull: {
            // 还原标志 设为不可见
            transitionRect.canPush = false
            transitionRect.visible = false//// 隐藏过渡卡片
        }
        //​拖拽达到阈值
        onCanPush :{
            // 调整大小覆盖parent
            transitionRect.fillParent()
            transitionRect.canPush = true
        }
        //用户松手
        onPushing:(id) => {
                      if( id===1){
                          memoryCard.line1Text=  "你喜欢的音乐风格："
                          memoryCard.line2Text = logic.readFile_fuc("qrc:/file_for_read/genre.txt").split('\n').find(line => line.trim() !== '')
                           memoryCard.line3Text=  "独属于你自己的品味"
                      }else{
                          memoryCard.line1Text=  "你喜欢的音乐能量值："
                          memoryCard.line2Text = logic.readFile_fuc("qrc:/file_for_read/ennergy.txt").split('\n').find(line => line.trim() !== '')
                          memoryCard.line3Text=  "你的能量值有点低，伤心的时候应该多听元气满满的歌哦~~~"
                      }
                  }
    }

    Card{
        // 能否push的标志
        property bool canPush: false
        visible: false
        id:transitionRect
        border.width: 4
        border.color: "blue"
        radius: 10
        content: ""
        image: ""
        opacity: 0.5
        gradient: grad

        //方法1：覆盖整个父容器
        function fillParent(){
            x=0
            y=0
            angle = 0
            width = parent.width
            height = parent.height
        }

        // 方法2：匹配指定卡片位置
        function fillItem(card){
            x = card.x
            y = card.y
            angle = card.angle
            width = card.width
            height = card.height
            visible = true
        }
    }
}
