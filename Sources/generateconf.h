/*
 * @author: YMHuang
 * @date: 2026-01-27 15:20:00
 *
 * @brief: 生成EasyTier配置文件
 * @note: 目前的实现是生成Easytier的配置参数，在运行程序时添加到命令行参数中
 *        后续可能考虑实现直接生成配置文件的功能并支持导入配置文件
 */

#ifndef QTEASYTIER_GENERATECONF_H
#define QTEASYTIER_GENERATECONF_H

#include <QString>
#include "setting.h"

// 前向声明
class NetPage;
class Settings;

/**
 * @brief: 启动方式枚举类
 */
enum class StartMode {
    Normal,         // 常规管理（默认）
    WebConsole,     // Web 控制台管理
    ConfigFile      // 配置文件启动
};

/**
 * @brief: 生成EasyTier配置参数（常规启动）
 * @param netPage: 网络设置页面指针
 * @return: EasyTier配置参数字符串列表
 */
QStringList generateConfCommand(NetPage *netPage);

/**
 * @brief: 生成EasyTier配置参数（配置文件启动 - 选择文件模式）
 * @param netPage: 网络设置页面指针
 * @param configFilePath: 配置文件路径
 * @return: EasyTier配置参数字符串列表
 */
QStringList generateConfFile(NetPage *netPage, const QString& configFilePath);

/**
 * @brief: 生成EasyTier配置参数（配置文件启动 - 下方输入模式）
 * @param netPage: 网络设置页面指针
 * @param configContent: 配置文件内容
 * @param tempConfigFilePath: 输出参数，临时配置文件路径
 * @return: EasyTier配置参数字符串列表
 */
QStringList generateConfFile(NetPage *netPage, const QString& configContent, QString& tempConfigFilePath);

/**
 * @brief: 生成EasyTier配置参数（Web管理启动）
 * @param netPage: 网络设置页面指针
 * @param webConnectAddr: Web控制台连接地址
 * @param connectToLocal: 是否连接到本地控制台
 * @param localConfigPort: 本地控制台配置端口（仅当 connectToLocal 为 true 时使用）
 * @param localConfigProtocol: 本地控制台配置协议（仅当 connectToLocal 为 true 时使用）
 * @return: EasyTier配置参数字符串列表
 */
QStringList generateConfWeb(NetPage *netPage, const QString& webConnectAddr,
                            bool connectToLocal, int localConfigPort = 55668,
                            const QString& localConfigProtocol = "udp");

/// @brief: 检测端口是否被占用
/// @param port: 端口号
/// @return: 如果端口被占用则返回true，否则返回false
///
/// @note: 该函数用于检测指定端口是否被其他进程占用
bool isPortOccupied(const int &port);

/// @brief: 生成随机端口号
/// @return: 10000-50000 范围内的随机端口
int getRandomPort();


/**
 * @brief: Base32编码函数
 * @param data: 要编码的数据
 * @return: Base32编码后的字符串
 */
QString base32Encode(const QByteArray& data);

/**
 * @brief: Base32解码函数
 * @param encoded: Base32编码的字符串
 * @return: 解码后的数据
 */
QByteArray base32Decode(const QString& encoded);

/**
 * @brief: 生成房间号和密码
 * @return: QPair<房间号, 密码>
 */
QPair<QString, QString> generateRoomCredentials();

/**
 * @brief: 编码房间信息为联机码
 * @param networkId: 网络号
 * @param password: 密码
 * @return: 格式化的联机码 XXXX-XXXX-XXXX-XXXX
 */
QString encodeConnectionCode(const QString& networkId, const QString& password);

/**
 * @brief: 解码联机码获取房间信息
 * @param code: 联机码
 * @return: QPair<网络号, 密码>，如果解码失败返回空pair
 */
QPair<QString, QString> decodeConnectionCode(const QString& code);

#endif //QTEASYTIER_GENERATECONF_H