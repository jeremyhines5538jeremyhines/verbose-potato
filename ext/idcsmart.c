#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#define PHP_IDCSMART_VERSION "1.0"
#define CURLOPT_URL 10002

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

static zif_handler original_curl_setopt = NULL;

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
        
        if ((strstr(url, "idcsmart.com") != NULL || strstr(url, "license.soft13") != NULL) && custom_url && strlen(custom_url) > 0) {
            char *path = strstr(url, "/app/");
            if (!path) path = strstr(url, "/api/");
            if (!path) path = strstr(url, "/market/");
            
            if (path) {
                size_t new_len = strlen(custom_url) + strlen(path) + 1;
                char *new_url = emalloc(new_len);
                snprintf(new_url, new_len, "%s%s", custom_url, path);
                ZVAL_STRING(zvalue, new_url);
                efree(new_url);
            }
        }
    }
    
    if (original_curl_setopt) {
        original_curl_setopt(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
}

static PHP_MINIT_FUNCTION(idcsmart)
{
    zend_function *func;
    REGISTER_INI_ENTRIES();
    
    if ((func = zend_hash_str_find_ptr(CG(function_table), "curl_setopt", sizeof("curl_setopt")-1)) != NULL) {
        original_curl_setopt = func->internal_function.handler;
        func->internal_function.handler = PHP_FN(idcsmart_curl_setopt);
    }
    
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
