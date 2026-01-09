#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#define PHP_IDCSMART_VERSION "1.0"

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
