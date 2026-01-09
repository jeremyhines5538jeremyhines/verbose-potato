#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#define PHP_IDCSMART_VERSION "1.0"

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("idcsmart.url", "https://license.soft13.idcsmart.com/", PHP_INI_ALL, OnUpdateString, custom_url, zend_idcsmart_globals, idcsmart_globals)
    STD_PHP_INI_ENTRY("idcsmart.app", "", PHP_INI_ALL, OnUpdateString, custom_app, zend_idcsmart_globals, idcsmart_globals)
PHP_INI_END()

ZEND_BEGIN_MODULE_GLOBALS(idcsmart)
    char *custom_url;
    char *custom_app;
ZEND_END_MODULE_GLOBALS(idcsmart)

ZEND_DECLARE_MODULE_GLOBALS(idcsmart)

#define IDCSMART_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(idcsmart, v)

PHP_FUNCTION(curl_setopt_wrapper);
PHP_FUNCTION(curl_setopt_array_wrapper);

static PHP_MINIT_FUNCTION(idcsmart)
{
    REGISTER_INI_ENTRIES();
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

PHP_FUNCTION(curl_setopt_wrapper)
{
    zval *zid, *zvalue;
    zend_long options;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &zid, &options, &zvalue) == FAILURE) {
        return;
    }
    
    if (options == CURLOPT_URL && Z_TYPE_P(zvalue) == IS_STRING) {
        char *url = Z_STRVAL_P(zvalue);
        char *custom_url = IDCSMART_G(custom_url);
        
        if (strstr(url, "idcsmart.com") != NULL || strstr(url, "license.soft13") != NULL) {
            if (custom_url && strlen(custom_url) > 0) {
                char *new_url = emalloc(strlen(custom_url) + strlen(url) + 256);
                char *path = strstr(url, "/app/");
                if (path == NULL) path = strstr(url, "/api/");
                if (path == NULL) path = strstr(url, "/market/");
                
                if (path) {
                    sprintf(new_url, "%s%s", custom_url, path);
                } else {
                    strcpy(new_url, custom_url);
                }
                
                ZVAL_STRING(zvalue, new_url);
                efree(new_url);
            }
        }
    }
    
    zend_function *orig_func;
    if ((orig_func = zend_hash_str_find_ptr(CG(function_table), "curl_setopt", sizeof("curl_setopt")-1)) != NULL) {
        orig_func->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
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
ZEND_GET_MODULE(idcsmart)
#endif
