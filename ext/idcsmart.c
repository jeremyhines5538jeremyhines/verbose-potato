#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <string.h>

#define PHP_IDCSMART_VERSION "1.0"
#define CURLOPT_URL 10002

// 官方公钥（需要被替换的）
static const char *official_pubkey = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDg6DKmQVwkQCzKcFYb0BBW7N2fI7DqL4MaiT6vibgEzH3EUFuBCRg3cXqCplJlk13PPbKMWMYsrc5cz7+k08kgTpD4tevlKOMNhYeXNk5ftZ0b6MAR0u5tiyEiATAjRwTpVmhOHOOh32MMBkf+NNWrZA/nzcLRV8GU7+LcJ8AH/QIDAQAB";

// 替换公钥（与你的私钥配对）
static const char *custom_pubkey = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvI00BCMWGmpDaMiiWfg1VAHAvFMMhwFN8v/zfeGllClzuR2SOwBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWVCrKXTS1j3E6b8WJEBt3TV38O50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCBKrWUjlUqzS4J1sTncwIDAQAB";

// 完整的替换公钥 PEM 格式
static const char *custom_pubkey_pem = "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvI00BCMWGmpDaMiiWfg1VAHAv\nFMMhwFN8v/zfeGllClzuR2SOwBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWV\nCrKXTS1j3E6b8WJEBt3TV38O50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCB\nKrWUjlUqzS4J1sTncwIDAQAB\n-----END PUBLIC KEY-----";

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

// Hook openssl_pkey_get_public - 替换公钥
PHP_FUNCTION(idcsmart_openssl_pkey_get_public)
{
    zval *cert;
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    
    // 检查第一个参数是否是字符串
    if (ZEND_NUM_ARGS() >= 1 && Z_TYPE_P(args) == IS_STRING) {
        char *key_str = Z_STRVAL_P(args);
        
        // 检查是否包含官方公钥特征
        if (strstr(key_str, official_pubkey) != NULL) {
            // 替换参数为自定义公钥
            zval_ptr_dtor(args);
            ZVAL_STRING(args, custom_pubkey_pem);
        }
    }
    
    // 调用原始函数
    if (original_openssl_pkey_get_public) {
        original_openssl_pkey_get_public(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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
    
    // 只处理 CURLOPT_URL 选项
    if (options == CURLOPT_URL && Z_TYPE_P(zvalue) == IS_STRING) {
        char *url = Z_STRVAL_P(zvalue);
        char *custom_url = IDCSMART_G(custom_url);
        
        // 检查是否是授权URL
        if (strstr(url, "license.soft13.idcsmart.com") != NULL && custom_url && strlen(custom_url) > 0) {
            // 提取路径部分
            char *path = strstr(url, "/app/");
            if (!path) path = strstr(url, "/api/");
            if (!path) path = strstr(url, "/market/");
            if (!path) path = strstr(url, "/upgrade/");
            if (!path) path = strstr(url, "/tool/");
            
            if (path) {
                // 构造新URL
                size_t url_len = strlen(custom_url);
                size_t path_len = strlen(path);
                char *new_url = emalloc(url_len + path_len + 1);
                
                // 去掉 custom_url 末尾的斜杠
                if (url_len > 0 && custom_url[url_len - 1] == '/') {
                    memcpy(new_url, custom_url, url_len - 1);
                    new_url[url_len - 1] = '\0';
                    strcat(new_url, path);
                } else {
                    strcpy(new_url, custom_url);
                    strcat(new_url, path);
                }
                
                // 修改参数
                zval_ptr_dtor(zvalue);
                ZVAL_STRING(zvalue, new_url);
                efree(new_url);
            }
        }
    }
    
    // 调用原始函数
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
    
    // 检查数组中是否有 CURLOPT_URL
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
                
                // 修改数组中的 URL
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

// 覆盖函数的辅助函数
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
    
    // Hook openssl_pkey_get_public
    php_override_function("openssl_pkey_get_public", sizeof("openssl_pkey_get_public") - 1,
                          PHP_FN(idcsmart_openssl_pkey_get_public), &original_openssl_pkey_get_public);
    
    // Hook curl_setopt
    php_override_function("curl_setopt", sizeof("curl_setopt") - 1,
                          PHP_FN(idcsmart_curl_setopt), &original_curl_setopt);
    
    // Hook curl_setopt_array
    php_override_function("curl_setopt_array", sizeof("curl_setopt_array") - 1,
                          PHP_FN(idcsmart_curl_setopt_array), &original_curl_setopt_array);
    
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
