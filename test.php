<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

echo "=== IDCSmart 扩展测试 ===\n\n";

// 1. 检查扩展加载
echo "1. 扩展加载状态: ";
echo extension_loaded('idcsmart') ? "已加载\n" : "未加载\n";

// 2. 检查配置
echo "2. idcsmart.url: " . ini_get('idcsmart.url') . "\n\n";

// 3. 测试公钥替换
echo "3. 测试公钥替换:\n";
$official_key = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDg6DKmQVwkQCzKcFYb0BBW7N2f\r\nI7DqL4MaiT6vibgEzH3EUFuBCRg3cXqCplJlk13PPbKMWMYsrc5cz7+k08kgTpD4\r\ntevlKOMNhYeXNk5ftZ0b6MAR0u5tiyEiATAjRwTpVmhOHOOh32MMBkf+NNWrZA/n\r\nzcLRV8GU7+LcJ8AH/QIDAQAB\r\n-----END PUBLIC KEY-----";

$expected_key = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvI00BCMWGmpDaMiiWfg1VAHAv";

$key = @openssl_pkey_get_public($official_key);
if ($key) {
    $details = openssl_pkey_get_details($key);
    $actual = $details['key'];
    echo "实际公钥:\n$actual\n";
    
    if (strpos($actual, $expected_key) !== false) {
        echo "✓ 公钥替换成功!\n\n";
    } else {
        echo "✗ 公钥未被替换 (仍是官方公钥)\n\n";
    }
} else {
    echo "✗ openssl_pkey_get_public 失败: " . openssl_error_string() . "\n\n";
}

// 4. 测试授权解密
echo "4. 测试授权解密:\n";

// 模拟 _strcode 函数
function _strcode($string='', $operation = 'DECODE', $key = '', $expiry = 0) {
    $ckey_length = 4;   
    $key = md5($key ? $key : 'default_key');
    $keya = md5(substr($key, 0, 16));
    $keyb = md5(substr($key, 16, 16));
    $keyc = $ckey_length ? ($operation == 'DECODE' ? substr($string, 0, $ckey_length): substr(md5(microtime()), -$ckey_length)) : '';
    $cryptkey = $keya.md5($keya.$keyc);
    $key_length = strlen($cryptkey);
    $string = $operation == 'DECODE' ? base64_decode(substr($string, $ckey_length)) : sprintf('%010d', $expiry ? $expiry + time() : 0).substr(md5($string.$keyb), 0, 16).$string;
    $string_length = strlen($string);
    $result = '';
    $box = range(0, 255);
    $rndkey = array();
    for($i = 0; $i <= 255; $i++) {
        $rndkey[$i] = ord($cryptkey[$i % $key_length]);
    }
    for($j = $i = 0; $i < 256; $i++) {
        $j = ($j + $box[$i] + $rndkey[$i]) % 256;
        $tmp = $box[$i];
        $box[$i] = $box[$j];
        $box[$j] = $tmp;
    }
    for($a = $j = $i = 0; $i < $string_length; $i++) {
        $a = ($a + 1) % 256;
        $j = ($j + $box[$a]) % 256;
        $tmp = $box[$a];
        $box[$a] = $box[$j];
        $box[$j] = $tmp;
        $result .= chr(ord($string[$i]) ^ ($box[($box[$a] + $box[$j]) % 256]));
    }
    if($operation == 'DECODE') {
        if((substr($result, 0, 10) == 0 || substr($result, 0, 10) - time() > 0) && substr($result, 10, 16) == substr(md5(substr($result, 26).$keyb), 0, 16)) {
            return substr($result, 26);
        } else {
            return '';
        }
    } else {
        return $keyc.str_replace('=', '', base64_encode($result));
    }
}

// 模拟 de_authorize 函数 (使用官方公钥)
function de_authorize($zjmf_authorize) {
    $_strcode = _strcode($zjmf_authorize, "DECODE", "zjmf_key_strcode");
    $_strcode = explode("|zjmf|", $_strcode);
    
    // 这里用官方公钥，扩展应该会替换它
    $authkey = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDg6DKmQVwkQCzKcFYb0BBW7N2f\r\nI7DqL4MaiT6vibgEzH3EUFuBCRg3cXqCplJlk13PPbKMWMYsrc5cz7+k08kgTpD4\r\ntevlKOMNhYeXNk5ftZ0b6MAR0u5tiyEiATAjRwTpVmhOHOOh32MMBkf+NNWrZA/n\r\nzcLRV8GU7+LcJ8AH/QIDAQAB\r\n-----END PUBLIC KEY-----";
    
    $pu_key = openssl_pkey_get_public($authkey);
    $de_str = "";
    foreach ($_strcode as $v) {
        openssl_public_decrypt(base64_decode($v), $de, $pu_key);
        $de_str .= $de;
    }
    return json_decode($de_str, true);
}

// 生成测试授权数据 (用自定义私钥加密)
function auth_encrypt($data, $private_key){
    $pri_key = openssl_pkey_get_private($private_key);
    $_strcode = '';
    $i=0;
    while($str = substr($data,$i,100)){
        $i+=100;
        openssl_private_encrypt($str,$encrypted,$pri_key);
        $encrypted = base64_encode($encrypted);
        $_strcode .= $encrypted.'|zjmf|';
    }
    $_strcode = substr($_strcode, 0, -6);
    $zjmf_authorize = _strcode($_strcode, "ENCODE", "zjmf_key_strcode");
    return $zjmf_authorize;
}

// 自定义私钥 (与替换公钥配对)
$private_key = "-----BEGIN RSA PRIVATE KEY-----
MIICXQIBAAKBgQCvI00BCMWGmpDaMiiWfg1VAHAvFMMhwFN8v/zfeGllClzuR2SO
wBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWVCrKXTS1j3E6b8WJEBt3TV38O
50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCBKrWUjlUqzS4J1sTncwIDAQAB
AoGBAKgDboD6gDvUFfgsvEE7JeEQrY6NFXFQhwcbSyS66Cj1Wxd/x9kmL815nfoD
0m3gZMK0yF/AySvhFM7UTdU4+Ywqr99tmcBLQnJNQBeY/qWjWkdnTY0sTseET3UZ
EF+9VDShYVfywBWb6k1hAirjGsb7rUOBbLHtCh0ybKLgy3XpAkEA5kmmNVHd5C2L
xX2rLqDMIkVAKvGOmPuBEht/HNVaBRRmwRqLCGB4I8AT8uFIcxgetbq5NOhknEM3
Sv/YWLEoFwJBAMKxS7wEiuOzcjaTKMKh8Q/9ZPbsQC/0n1ESRdq54llkWnAmpVda
/H07Um6xQ73SA5luSJpNeAa0u91hyDzuOQUCQQDAV29dRX2BYZFGaEob7e3SCeko
dQ1XNWl+iDzJBmHjsECKtPK/EmQF6B5BVGzAxLqRqt2aFShDY9jA6aP3oDANAkAD
NVjCKHEBf5uKOrKZQoHueYaIayxDngxiEaMFD7sjLfwvjPN8lQtEJQ4etN0g660T
gjXAFTa9JY1TAKGEH4jhAkB4MGfT8LpJNqAPZkS+ePvzxyKVTdIP8AvJfm01Kd4N
qLWf0KzpE7PDkt4o+eAsXh15RPgR7a6nSnf923/o7Do4
-----END RSA PRIVATE KEY-----";

$test_data = json_encode([
    'id' => 1,
    'ip' => '127.0.0.1',
    'domain' => 'test.com',
    'status' => 'Active',
    'edition' => 1,
    'app' => ['test'],
    'last_license_time' => time()
]);

$encrypted = auth_encrypt($test_data, $private_key);
echo "加密数据长度: " . strlen($encrypted) . "\n";

$decrypted = de_authorize($encrypted);
if ($decrypted && isset($decrypted['status'])) {
    echo "✓ 解密成功!\n";
    echo "解密结果: " . json_encode($decrypted, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE) . "\n";
} else {
    echo "✗ 解密失败\n";
    echo "原始数据: $test_data\n";
}

echo "\n=== 测试完成 ===\n";
