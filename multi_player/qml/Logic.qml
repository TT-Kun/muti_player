import QtQuick 2.12
import Qt.labs.folderlistmodel 2.2
import Qt.labs.folderlistmodel 2.2

import Custom.IO 1.0 // 对应注册的模块名
import av_upload 1.0
import Database 1.0


Item {
    // 范围内取随机数
    function generateRandomNumber(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }
    // 取随机数时排除某些数
    function getRandomExcluding(min, max, exclusions) {
        const exclusionSet = new Set(exclusions);
        let randomNumber;

        do {
            randomNumber = generateRandomNumber(min, max);
        } while (exclusionSet.has(randomNumber));

        return randomNumber;
    }
    // 取 QGradient 的预设值
    function getRandomGradient(){
        return getRandomExcluding(1,180,[27, 39, 40, 45, 71, 74, 105, 111, 119, 130, 135, 141])
    }

    FileIO {
            id: fileIO
            onFileReadSuccess: console.log("文件读取成功")
            onErrorOccurred: console.error("错误:", msg)
        }
    // 公开的读取文件函数
     function readFile_fuc(fileUrl) {
         return fileIO.readFile(fileUrl)
     }

     SqlHandler{
         id:sql
     }
     function  write(path,jsonStr){
          return av_upload.openFilePicker(path,jsonStr)
      }

    FileUploader{
        id :av_upload
        onSuccess: (responseData) => {//信号
                       const parsedData = JSON.parse(responseData);
                       console.log(parsedData.data.report);
               if (parsedData.status === "success") {
                     if (parsedData.data && parsedData.data.report) {
                           character.character_contenxt = parsedData.data.report.report
                             character.index_for_switch=0
                     } else {
                        character.character_contenxt  = "⚠️ 报告内容不存在"
                           character.index_for_switch=0
                     }
                 } else {
                     character.character_contenxt  = "❌ 上传失败："
                     character.index_for_switch=0
                 }

                character.createBubble();
             resetTimer.start()
         }

        onFailure: (error) => {//信号
                    character.character_contenxt="❌ 上传失败"
                     console.error("上传失败:", error)
                       character.index_for_switch=0
                        character.createBubble();
        }

    }

   function  upload_av(){
        return av_upload.openFilePicker()
    }

}


