import QtQuick 2.0

Item {
    function readfile(fileUrl) {
        var file = new File(fileUrl);
        if (file.open(Qt.ReadOnly)) {
            var content = file.readAll();
            file.close();
            return content; // 返回文件内容
        } else {
            console.error("无法打开文件:", file.errorString());
            return ""; // 返回空字符串表示失败
        }
    }
}
