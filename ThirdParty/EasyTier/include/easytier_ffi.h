/**
 * @file easytier_ffi.h
 * @brief EasyTier FFI C/C++ 接口头文件
 * @author GLM5
 * @version 0.1.0
 * 
 * 本头文件提供了EasyTier FFI库的C/C++调用接口，
 * 允许外部程序通过FFI方式创建和管理EasyTier网络实例。
 * 
 * @note 所有字符串参数使用UTF-8编码，调用者需确保字符串有效性。
 * @note 返回的字符串指针需要调用 free_string() 释放内存。
 */

#ifndef EASYTIER_FFI_H
#define EASYTIER_FFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief 键值对结构体
 * 
 * 用于存储网络实例名称与实例信息的映射关系。
 * key 为实例名称字符串指针。
 * value 为JSON格式的实例信息字符串指针。
 */
typedef struct KeyValuePair {
    const char* key;   /**< 实例名称（C字符串，需调用 free_string 释放） */
    const char* value; /**< 实例信息JSON字符串（需调用 free_string 释放） */
} KeyValuePair;

/**
 * @brief 解析配置字符串
 * 
 * 验证TOML格式的配置字符串是否有效。此函数仅进行语法检查，
 * 不会创建或启动网络实例。
 * 
 * @param[in] cfg_str TOML格式的配置字符串，UTF-8编码，不能为NULL
 * @return 成功返回 0，失败返回 -1
 * 
 * @note 配置字符串格式示例：
 * @code
 * inst_name = "my_instance"
 * network = "my_network"
 * network_secret = "secret123"
 * @endcode
 * 
 * @see run_network_instance() 启动网络实例
 * @see get_error_msg() 获取错误信息
 */
int parse_config(const char* cfg_str);

/**
 * @brief 运行网络实例
 * 
 * 根据提供的TOML配置字符串创建并启动一个新的网络实例。
 * 每个实例通过配置中的 inst_name 字段唯一标识。
 * 
 * @param[in] cfg_str TOML格式的配置字符串，UTF-8编码，不能为NULL
 * @return 成功返回 0，失败返回 -1
 * 
 * @note 如果实例名称已存在，将返回错误。
 * @note 配置字符串必须包含 inst_name 和 network 字段。
 * 
 * @see parse_config() 验证配置
 * @see retain_network_instance() 管理实例生命周期
 * @see get_error_msg() 获取错误信息
 */
int run_network_instance(const char* cfg_str);

/**
 * @brief 保留指定的网络实例
 * 
 * 保留指定的网络实例，终止不在列表中的实例。
 * 这是一个批量管理函数，可用于清理不再需要的实例。
 * 
 * @param[in] inst_names 要保留的实例名称数组，每个元素为C字符串指针
 * @param[in] length 实例名称数组的长度，若为0则终止所有实例
 * @return 成功返回 0，失败返回 -1
 * 
 * @warning 此操作不可逆，被终止的实例将无法恢复。
 * @warning 传入 length=0 将终止所有运行中的实例。
 * 
 * @see run_network_instance() 启动实例
 * @see get_error_msg() 获取错误信息
 */
int retain_network_instance(const char** inst_names, size_t length);

/**
 * @brief 收集网络实例信息
 * 
 * 收集所有运行中的网络实例的状态信息，返回实例名称与详细信息的映射。
 * 实例信息以JSON格式返回，包含节点、路由、对等连接等详细信息。
 * 
 * @param[out] infos 接收结果的 KeyValuePair 数组，由调用者分配内存
 * @param[in] max_length infos数组的最大容量
 * @return 成功返回实际填充的数量，失败返回 -1
 * 
 * @note 返回的 KeyValuePair 中的 key 和 value 指针需要调用 free_string() 释放。
 * @note 如果实例数量超过 max_length，将只返回前 max_length 个实例的信息。
 * 
 * @see free_string() 释放字符串内存
 * @see get_error_msg() 获取错误信息
 */
int collect_network_infos(KeyValuePair* infos, size_t max_length);

/**
 * @brief 设置 TUN 文件描述符
 * 
 * 为指定的网络实例设置 TUN 设备文件描述符。
 * 此函数主要用于 Android/iOS 等需要从应用层提供 TUN 设备的平台。
 * 
 * @param[in] inst_name 实例名称，不能为 NULL
 * @param[in] fd TUN 设备的文件描述符
 * @return 成功返回 0，失败返回 -1
 * 
 * @note 在 Windows 平台上此函数通常不需要调用。
 * @note 实例必须已通过 run_network_instance() 创建。
 * 
 * @see run_network_instance() 创建网络实例
 */
int set_tun_fd(const char* inst_name, int fd);

/**
 * @brief 获取最后的错误信息
 * 
 * 获取最近一次 FFI 调用失败时的详细错误信息。
 * 错误信息在下次调用失败时会被覆盖。
 * 
 * @param[out] out 接收错误信息字符串指针的指针，不能为 NULL
 * 
 * @note 如果没有错误，*out 将被设置为 NULL。
 * @note 返回的字符串需要调用 free_string() 释放。
 * 
 * @see free_string() 释放字符串内存
 */
void get_error_msg(const char** out);

/**
 * @brief 释放由 FFI 分配的字符串内存
 * 
 * 释放 FFI 函数返回的字符串指针所指向的内存。
 * 包括 get_error_msg() 和 collect_network_infos() 返回的字符串。
 * 
 * @param[in] s 要释放的字符串指针，可以为 NULL（此时函数不执行任何操作）
 * 
 * @warning 不要释放非 FFI 函数返回的字符串指针。
 * @warning 每个字符串指针只能释放一次。
 */
void free_string(const char* s);

#ifdef __cplusplus
}
#endif

#endif /* EASYTIER_FFI_H */
