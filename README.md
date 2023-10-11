# FullIME

update 2023.10.12.00:51

![](https://i.imgur.com/T5gFzko.png)

绘图更新。

---

基于 Windows hook 机制开发的一款中文输入法。

窗口是基于古老而又高效利落的 Win32 窗口技术。

图形的绘制使用了 Dierct2D，文字的渲染使用了 DirectWrite。

词库使用的是 SQLite 进行存取。

开发套件采用的方案是：

- VSCode
- CMake
- C/C++
- msvc-x64(VS2022)

目前还在完善中。

## 如何使用

最好和我保持一样的目录，当然，也可以选择修改在源码中修改 `fanyciku.db` 的路径，我自己的路径信息是下面这样，

![](https://i.imgur.com/fvhnWco.png)

把项目根目录下的 `libs/sqlite/sqlite3.dll` 复制到 `fullcpp.exe` 的相同目录下，

效果展示，

![](https://i.imgur.com/vkPSR08.png)

## 目前支持的功能

- 小鹤双拼
- 候选框竖排
- 候选框跟随文本编辑光标
- 大词库(100万左右)
- 字体渲染优美，适配高分屏
- 不含任何添加剂

## 如何编译

第一步，配置 Windows 开发的 sdk，Windows10 以及 Windows11 都可以，

![](https://i.imgur.com/FEsaO1h.png)

第二步，配置 CMake 环境。

第三步，clone 本仓库的最新分支。

第四步，使用 CMake 编译运行。

## 后续功能开发

- 词库记忆
- 词库自动调频
- 支持小雨点辅助码
- 词库优化
- SQLite 查询优化

......
