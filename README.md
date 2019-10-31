# Abup FOTA Downloader

## 1、介绍

本软件包是用于 Abup FOTA 升级的固件下载器

 - HTTP/CoAP 协议下载固件

### 1.1 许可证

Abup FOTA Downloader package 遵循 Apache2.0 许可，详见 `LICENSE` 文件。

### 1.2 依赖

- RT-Thread 3.0+
- FAL 软件包支持
- HTTP/CoAP 下载方式依赖于 TCP/UDP

## 2、如何打开 Abup FOTA Downloader

使用 Abup FOTA Downloader package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    IoT - internet of things  --->
        [*]  The firmware downloader which using on RT-Thread OTA component  --->
            [*]  Enable HTTP/CoAP OTA
            [*]  Enable Abup FOTA Downloader debug
            [*]  Enable HTTP/CoAP OTA
                (http://xxx/xxx/rtthread.rbl) HTTP OTA URL
            [*]  Enable Ymodem OTA
```

软件包选项的详细说明如下图：

| 选项 | 说明 |
|-|-|
| Enable Abup FOTA Downloader debug | 使能固件下载器 debug 模式 |
| Enable HTTP/CoAP OTA | 使能 HTTP/CoAP 协议下载固件功能 |
| Enable Ymodem OTA | 使能 Ymodem 协议下载固件功能 |

选择完自己需要的选项后使用 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、使用 Abup FOTA Downloader

在打开 Abup FOTA Downloader package ，选中相应的功能选项后，当进行 BSP 编译时，它会被加入到 BSP 工程中进行编译。
烧录程序到目标开发板，用户可在 FinSH 终端找到对应的命令。目前软件包支持的升级方式如下表：

| 功能 | 函数调用 | 执行命令 |
|---|---|---|
| 使用 HTTP/CoAP 协议固件升级 | `void http_ota(uint8_t argc, char **argv)` | `http_ota` |
| 使用 Ymodem 协议固件升级 | `void ymodem_ota(uint8_t argc, char **argv)` | `ymodem_ota` |

### 3.1 Ymodem 协议固件升级命令行演示

推荐使用支持 Ymodem 协议的 Xshell 。在终端输入 `ymodem_ota` 命令后，鼠标右键然后在菜单栏找到用 YMODEM 发送选项发送文件。具体步骤如下图：

![ymodem_ota](docs/figures/ymodem_ota.png)

### 3.2 HTTP/CoAP 协议固件升级命令行演示

在终端输入 `http_ota http://xxx/xxx/rtthreadf.rbl` 命令，执行该命令后将会从链接 `http://xxx/xxx/rtthreadf.rbl` 处下载固件。具体步骤如下图所示：

![http_ota](docs/figures/http_ota.png)

## 4、注意事项

 1. 确保 FAL 中有 downloader 分区。
 2. Ymodem 协议升级固件时，请使用支持 Ymodem 协议的工具。
 3. HTTP/CoAP 协议升级固件时，需确保远端链接地址可用。

## 5、参考资料

> 《RT-Thread OTA 用户手册》: docs/RT-Thread-OTA 用户手册.pdf

## 6、联系方式 & 感谢

* 维护：艾拉比开发团队
* 主页：https://github.com/RayShen1018/Abup
