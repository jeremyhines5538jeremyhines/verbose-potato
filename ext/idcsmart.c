#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <string.h>

#define PHP_IDCSMART_VERSION "1.0"
#define CURLOPT_URL 10002

// 官方公钥特征
static const char *official_pubkey_needle = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDg6DKmQVwkQCzKcFYb0BBW7N2f";

// 替换公钥 PEM 格式
static const char *custom_pubkey_pem = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvI00BCMWGmpDaMiiWfg1VAHAv\r\nFMMhwFN8v/zfeGllClzuR2SOwBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWV\r\nCrKXTS1j3E6b8WJEBt3TV38O50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCB\r\nKrWUjlUqzS4J1sTncwIDAQAB\r\n-----END PUBLIC KEY-----";

// 官方授权 URL
static const char *license_url = "https://license.soft13.idcsmart.com/";

// 模块名
static const char *module_name = "idcsmart";

ZEND_BEGIN_MODULE_GLOBALS(idcsmart)
    char *custom_url;
    char *custom_app;
ZEND_END_MODULE_GLOBALS(idcsmart)

ZEND_DECLARE_MODULE_GLOBALS(idcsmart)

#ifdef ZTS
#define IDCSMART_G(v) TSRMG(idcsmart_globals_id, zend_idcsmart_globals *, v)
#else
#define IDCSMART_G(v) (idcsmart_globals.v)
#endif

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("idcsmart.url", "https://license.soft13.idcsmart.com/", PHP_INI_ALL, OnUpdateString, custom_url, zend_idcsmart_globals, idcsmart_globals)
    STD_PHP_INI_ENTRY("idcsmart.app", "", PHP_INI_ALL, OnUpdateString, custom_app, zend_idcsmart_globals, idcsmart_globals)
PHP_INI_END()

// 原始函数指针
static zif_handler original_curl_setopt = NULL;
static zif_handler original_curl_setopt_array = NULL;
static zif_handler original_openssl_pkey_get_public = NULL;
static zif_handler original_json_decode = NULL;
static zif_handler original_extension_loaded = NULL;
static zif_handler original_get_loaded_extensions = NULL;
static zif_handler original_stream_context_create = NULL;
static zif_handler original_curl_getinfo = NULL;
static zif_handler original_file_get_contents = NULL;

// Hook openssl_pkey_get_public - 替换公钥
PHP_FUNCTION(idcsmart_openssl_pkey_get_public)
{
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    
    if (ZEND_NUM_ARGS() >= 1 && Z_TYPE_P(args) == IS_STRING) {
        char *key_str = Z_STRVAL_P(args);
        
        if (strstr(key_str, official_pubkey_needle) != NULL) {
            zend_string *new_str = zend_string_init(custom_pubkey_pem, strlen(custom_pubkey_pem), 0);
            zend_string_release(Z_STR_P(args));
            ZVAL_NEW_STR(args, new_str);
        }
    }
    
    if (original_openssl_pkey_get_public) {
        original_openssl_pkey_get_public(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

// Hook stream_context_create - 禁用 SSL 验证
PHP_FUNCTION(idcsmart_stream_context_create)
{
    zval *options = NULL;
    zval *params = NULL;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a!a!", &options, &params) == FAILURE) {
        if (original_stream_context_create) {
            original_stream_context_create(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
        return;
    }
    
    // 如果有 options 参数，注入 ssl 配置
    if (options && Z_TYPE_P(options) == IS_ARRAY) {
        HashTable *ht = Z_ARRVAL_P(options);
        zval *ssl = zend_hash_str_find(ht, "ssl", sizeof("ssl") - 1);
        
        if (ssl && Z_TYPE_P(ssl) == IS_ARRAY) {
            // ssl 配置已存在，更新它
            zval false_val;
            ZVAL_FALSE(&false_val);
            zend_hash_str_update(Z_ARRVAL_P(ssl), "verify_peer", sizeof("verify_peer") - 1, &false_val);
            ZVAL_FALSE(&false_val);
            zend_hash_str_update(Z_ARRVAL_P(ssl), "verify_peer_name", sizeof("verify_peer_name") - 1, &false_val);
        } else {
            // 创建 ssl 配置
            zval ssl_arr;
            array_init(&ssl_arr);
            add_assoc_bool(&ssl_arr, "verify_peer", 0);
            add_assoc_bool(&ssl_arr, "verify_peer_name", 0);
            zend_hash_str_add(ht, "ssl", sizeof("ssl") - 1, &ssl_arr);
        }
    }
    
    if (original_stream_context_create) {
        original_stream_context_create(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

// Hook curl_getinfo - 替换返回的 URL
PHP_FUNCTION(idcsmart_curl_getinfo)
{
    if (original_curl_getinfo) {
        original_curl_getinfo(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    
    // 如果返回数组，检查并替换 url 字段
    if (Z_TYPE_P(return_value) == IS_ARRAY) {
        zval *url = zend_hash_str_find(Z_ARRVAL_P(return_value), "url", sizeof("url") - 1);
        if (url && Z_TYPE_P(url) == IS_STRING) {
            char *url_str = Z_STRVAL_P(url);
            char *custom_url = IDCSMART_G(custom_url);
            
            // 如果 URL 以自定义 URL 开头，替换回官方 URL
            if (custom_url && strlen(custom_url) > 0 && strncmp(url_str, custom_url, strlen(custom_url)) == 0) {
                size_t custom_len = strlen(custom_url);
                if (custom_url[custom_len - 1] == '/') custom_len--;
                
                char *path = url_str + custom_len;
                size_t license_len = strlen(license_url);
                size_t path_len = strlen(path);
                
                char *new_url = emalloc(license_len + path_len + 1);
                strcpy(new_url, license_url);
                if (new_url[license_len - 1] == '/' && path[0] == '/') {
                    strcat(new_url, path + 1);
                } else {
                    strcat(new_url, path);
                }
                
                zval new_url_zval;
                ZVAL_STRING(&new_url_zval, new_url);
                zend_hash_str_update(Z_ARRVAL_P(return_value), "url", sizeof("url") - 1, &new_url_zval);
                efree(new_url);
            }
        }
    }
}

// Hook file_get_contents - 阻止访问官方授权 URL
PHP_FUNCTION(idcsmart_file_get_contents)
{
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    
    if (ZEND_NUM_ARGS() >= 1 && Z_TYPE_P(args) == IS_STRING) {
        char *filename = Z_STRVAL_P(args);
        
        // 如果是访问官方授权 URL，返回空字符串
        if (strncmp(filename, license_url, strlen(license_url)) == 0) {
            RETURN_EMPTY_STRING();
        }
    }
    
    if (original_file_get_contents) {
        original_file_get_contents(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

// Hook json_decode - 注入 app 列表
PHP_FUNCTION(idcsmart_json_decode)
{
    if (original_json_decode) {
        original_json_decode(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    
    if (Z_TYPE_P(return_value) == IS_ARRAY) {
        HashTable *ht = Z_ARRVAL_P(return_value);
        
        zval *system_token = zend_hash_str_find(ht, "system_token", sizeof("system_token") - 1);
        zval *install_version = zend_hash_str_find(ht, "install_version", sizeof("install_version") - 1);
        zval *app = zend_hash_str_find(ht, "app", sizeof("app") - 1);
        
        if (system_token && install_version && app && Z_TYPE_P(app) == IS_ARRAY) {
            char *custom_app = IDCSMART_G(custom_app);
            
            if (custom_app && strlen(custom_app) > 1) {
                HashTable *app_ht = Z_ARRVAL_P(app);
                char *app_copy = estrdup(custom_app);
                char *token = strtok(app_copy, ",");
                
                while (token != NULL) {
                    while (*token == ' ') token++;
                    char *end = token + strlen(token) - 1;
                    while (end > token && *end == ' ') *end-- = '\0';
                    
                    if (strlen(token) > 0) {
                        zval app_item;
                        ZVAL_STRING(&app_item, token);
                        zend_hash_next_index_insert(app_ht, &app_item);
                    }
                    token = strtok(NULL, ",");
                }
                efree(app_copy);
            }
        }
    }
}

// Hook extension_loaded - 隐藏扩展
PHP_FUNCTION(idcsmart_extension_loaded)
{
    if (original_extension_loaded) {
        original_extension_loaded(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    if (ZEND_NUM_ARGS() >= 1 && Z_TYPE_P(args) == IS_STRING) {
        if (strcasecmp(Z_STRVAL_P(args), module_name) == 0) {
            RETURN_FALSE;
        }
    }
}

// Hook get_loaded_extensions - 从列表中移除扩展
PHP_FUNCTION(idcsmart_get_loaded_extensions)
{
    if (original_get_loaded_extensions) {
        original_get_loaded_extensions(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    
    if (Z_TYPE_P(return_value) == IS_ARRAY) {
        HashTable *ht = Z_ARRVAL_P(return_value);
        zend_ulong idx;
        zval *val;
        
        ZEND_HASH_FOREACH_NUM_KEY_VAL(ht, idx, val) {
            if (Z_TYPE_P(val) == IS_STRING && strcasecmp(Z_STRVAL_P(val), module_name) == 0) {
                zend_hash_index_del(ht, idx);
                break;
            }
        } ZEND_HASH_FOREACH_END();
    }
}

// Hook curl_setopt - 重定向 URL
PHP_FUNCTION(idcsmart_curl_setopt)
{
    zval *zid, *zvalue;
    zend_long options;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &zid, &options, &zvalue) == FAILURE) {
        if (original_curl_setopt) {
            original_curl_setopt(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
        return;
    }
    
    if (options == CURLOPT_URL && Z_TYPE_P(zvalue) == IS_STRING) {
        char *url = Z_STRVAL_P(zvalue);
        char *custom_url = IDCSMART_G(custom_url);
        
        if (strstr(url, "license.soft13.idcsmart.com") != NULL && custom_url && strlen(custom_url) > 0) {
            char *path = strstr(url, "/app/");
            if (!path) path = strstr(url, "/api/");
            if (!path) path = strstr(url, "/market/");
            if (!path) path = strstr(url, "/upgrade/");
            if (!path) path = strstr(url, "/tool/");
            
            if (path) {
                size_t url_len = strlen(custom_url);
                size_t path_len = strlen(path);
                char *new_url = emalloc(url_len + path_len + 1);
                
                if (url_len > 0 && custom_url[url_len - 1] == '/') {
                    memcpy(new_url, custom_url, url_len - 1);
                    new_url[url_len - 1] = '\0';
                    strcat(new_url, path);
                } else {
                    strcpy(new_url, custom_url);
                    strcat(new_url, path);
                }
                
                zval_ptr_dtor(zvalue);
                ZVAL_STRING(zvalue, new_url);
                efree(new_url);
            }
        }
    }
    
    if (original_curl_setopt) {
        original_curl_setopt(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

// Hook curl_setopt_array
PHP_FUNCTION(idcsmart_curl_setopt_array)
{
    zval *zid, *options;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra", &zid, &options) == FAILURE) {
        if (original_curl_setopt_array) {
            original_curl_setopt_array(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
        return;
    }
    
    zval *url_val = zend_hash_index_find(Z_ARRVAL_P(options), CURLOPT_URL);
    if (url_val && Z_TYPE_P(url_val) == IS_STRING) {
        char *url = Z_STRVAL_P(url_val);
        char *custom_url = IDCSMART_G(custom_url);
        
        if (strstr(url, "license.soft13.idcsmart.com") != NULL && custom_url && strlen(custom_url) > 0) {
            char *path = strstr(url, "/app/");
            if (!path) path = strstr(url, "/api/");
            if (!path) path = strstr(url, "/market/");
            if (!path) path = strstr(url, "/upgrade/");
            if (!path) path = strstr(url, "/tool/");
            
            if (path) {
                size_t url_len = strlen(custom_url);
                size_t path_len = strlen(path);
                char *new_url = emalloc(url_len + path_len + 1);
                
                if (url_len > 0 && custom_url[url_len - 1] == '/') {
                    memcpy(new_url, custom_url, url_len - 1);
                    new_url[url_len - 1] = '\0';
                    strcat(new_url, path);
                } else {
                    strcpy(new_url, custom_url);
                    strcat(new_url, path);
                }
                
                zval new_url_zval;
                ZVAL_STRING(&new_url_zval, new_url);
                zend_hash_index_update(Z_ARRVAL_P(options), CURLOPT_URL, &new_url_zval);
                efree(new_url);
            }
        }
    }
    
    if (original_curl_setopt_array) {
        original_curl_setopt_array(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

// 覆盖函数
static int php_override_function(const char *name, size_t name_len, zif_handler new_handler, zif_handler *original_handler)
{
    zend_function *func;
    
    func = zend_hash_str_find_ptr(CG(function_table), name, name_len);
    if (func != NULL && func->type == ZEND_INTERNAL_FUNCTION) {
        *original_handler = func->internal_function.handler;
        func->internal_function.handler = new_handler;
        return SUCCESS;
    }
    return FAILURE;
}

static PHP_MINIT_FUNCTION(idcsmart)
{
    REGISTER_INI_ENTRIES();
    
    php_override_function("openssl_pkey_get_public", sizeof("openssl_pkey_get_public") - 1,
                          PHP_FN(idcsmart_openssl_pkey_get_public), &original_openssl_pkey_get_public);
    
    php_override_function("json_decode", sizeof("json_decode") - 1,
                          PHP_FN(idcsmart_json_decode), &original_json_decode);
    
    php_override_function("extension_loaded", sizeof("extension_loaded") - 1,
                          PHP_FN(idcsmart_extension_loaded), &original_extension_loaded);
    
    php_override_function("get_loaded_extensions", sizeof("get_loaded_extensions") - 1,
                          PHP_FN(idcsmart_get_loaded_extensions), &original_get_loaded_extensions);
    
    php_override_function("curl_setopt", sizeof("curl_setopt") - 1,
                          PHP_FN(idcsmart_curl_setopt), &original_curl_setopt);
    
    php_override_function("curl_setopt_array", sizeof("curl_setopt_array") - 1,
                          PHP_FN(idcsmart_curl_setopt_array), &original_curl_setopt_array);
    
    php_override_function("stream_context_create", sizeof("stream_context_create") - 1,
                          PHP_FN(idcsmart_stream_context_create), &original_stream_context_create);
    
    php_override_function("curl_getinfo", sizeof("curl_getinfo") - 1,
                          PHP_FN(idcsmart_curl_getinfo), &original_curl_getinfo);
    
    php_override_function("file_get_contents", sizeof("file_get_contents") - 1,
                          PHP_FN(idcsmart_file_get_contents), &original_file_get_contents);
    
    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(idcsmart)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

static PHP_RINIT_FUNCTION(idcsmart)
{
    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(idcsmart)
{
    return SUCCESS;
}

static PHP_MINFO_FUNCTION(idcsmart)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "idcsmart support", "enabled");
    php_info_print_table_row(2, "Version", PHP_IDCSMART_VERSION);
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

const zend_function_entry idcsmart_functions[] = {
    PHP_FE_END
};

zend_module_entry idcsmart_module_entry = {
    STANDARD_MODULE_HEADER,
    "idcsmart",
    idcsmart_functions,
    PHP_MINIT(idcsmart),
    PHP_MSHUTDOWN(idcsmart),
    PHP_RINIT(idcsmart),
    PHP_RSHUTDOWN(idcsmart),
    PHP_MINFO(idcsmart),
    PHP_IDCSMART_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IDCSMART
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(idcsmart)
#endif
