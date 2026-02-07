# Open CMD Anywhere

一个简单的小程序，它能够帮助你通过快捷键以“当前窗口的可执行文件所在目录”为工作路径打开命令行程序，此外若当前窗口是文件管理器，它能够以文件管理器所在文件夹作为工作路径，而不用右键点击选择“在此处打开命令行”。

## 依赖

- Qt5.0+（开发环境是Qt6.4.3）：用于GUI界面和注册表编辑
- [global_hokety](https://github.com/JaderoChan/global_hotkey)：用于全局热键
- [nlohmann/json](https://github.com/nlohmann/json)：`easy_translate`库的依赖
- [easy_translate](https://github.com/JaderoChan/easy_translate)：用于界面翻译
- [minlog](https://github.com/JaderoChan/minilog)：用于日志记录

## 平台

仅Windows
