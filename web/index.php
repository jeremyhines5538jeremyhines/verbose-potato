<?php

//魔方财务系统最新版本号
$finance_last_version = '3.7.5';

//魔方云系统最新正式版本号
$cloud_release_version = '3.9.18';

//魔方云系统最新测试版本号
$cloud_last_version = '3.9.18';


//已购买的应用标识，如果要添加其他第三方应用，可在此添加后使用
$thrid_apps = ['Alimail','Submail','Subemail','Smsbao','Tencentcloud','Huaweicloud','Phonethree','ExpiredAutoDeleteBill','ExpiredIpLog','ExportExcel','ProductDivert','kuaiyunweb'];

//魔方财务系统自带应用标识
$finance_apps = ['seniorConfig','marketingPush','InvoiceContract','Oauth'];

//魔方业务V10应用标识
$business_apps = ['AbnormalInspectionRecords','ClientCare','ClientCustomField','CostPay','CreditLimit','CycleArtificialOrder','EContract','EmailNoticeAdmin','EventPromotion','FlowPacket','HostTransfer','IdcsmartClientLevel','IdcsmartDomain','IdcsmartInvoice','IdcsmartRecommend','IdcsmartSale','IdcsmartStatistics','IdcsmartVoucher','IdcsmartWebhook','ManualResource','NoticeSendMerge','ProductCashback','ProductCertLimit','ProductCycleLimit','ProductDropDownSelect','ProductNumLimit','ProductRelatedLimit','TicketInternalPremium','TicketPremium','WanyunResource','BtVirtualHost','DirectAdmin','MfCloudDisk','MfCloudIp','MfDcimCabinet','WestDomain','ZgsjDomain'];

//魔方云系统自带应用标识
$cloud_apps = ['gpupass','float_ip','random_port','index_msg_show','evacuation_setting','cloud_cron_snap','security_rule_lock','net_queues','AbuseManager','SmartBw','SmartCpu','LocalMigrate','abuse_monitor','advanced_cpu','advanced_bw'];
$cloud_goods = [
	['id'=>140, 'name'=>'Cloud', 'desc'=>'魔方云专业版'],
	['id'=>952, 'name'=>'Service', 'desc'=>'计算节点维护'],
	['id'=>957, 'name'=>'SmartCpu', 'desc'=>'智能CPU'],
	['id'=>958, 'name'=>'SmartBw', 'desc'=>'智能带宽'],
	['id'=>960, 'name'=>'LocalMigrate', 'desc'=>'本地存储热迁移'],
	['id'=>962, 'name'=>'AbuseManager', 'desc'=>'滥用管理'],
];
$cloud_plugins = [
	['name'=>'AutoMount'],
	['name'=>'BootScript'],
	['name'=>'Whitelist'],
	['name'=>'DiskIoLimit'],
	['name'=>'GoogleAuth'],
	['name'=>'BackupTimeLimit'],
	['name'=>'DbRemoteBackup'],
	['name'=>'FlowStatistics'],
	['name'=>'Mikrotik'],
];

$finance_apps = array_merge($finance_apps, $thrid_apps);



$private_key = "-----BEGIN RSA PRIVATE KEY-----\nMIICXQIBAAKBgQCvI00BCMWGmpDaMiiWfg1VAHAvFMMhwFN8v/zfeGllClzuR2SO\nwBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWVCrKXTS1j3E6b8WJEBt3TV38O\n50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCBKrWUjlUqzS4J1sTncwIDAQAB\nAoGBAKgDboD6gDvUFfgsvEE7JeEQrY6NFXFQhwcbSyS66Cj1Wxd/x9kmL815nfoD\n0m3gZMK0yF/AySvhFM7UTdU4+Ywqr99tmcBLQnJNQBeY/qWjWkdnTY0sTseET3UZ\nEF+9VDShYVfywBWb6k1hAirjGsb7rUOBbLHtCh0ybKLgy3XpAkEA5kmmNVHd5C2L\nxX2rLqDMIkVAKvGOmPuBEht/HNVaBRRmwRqLCGB4I8AT8uFIcxgetbq5NOhknEM3\nSv/YWLEoFwJBAMKxS7wEiuOzcjaTKMKh8Q/9ZPbsQC/0n1ESRdq54llkWnAmpVda\n/H07Um6xQ73SA5luSJpNeAa0u91hyDzuOQUCQQDAV29dRX2BYZFGaEob7e3SCeko\ndQ1XNWl+iDzJBmHjsECKtPK/EmQF6B5BVGzAxLqRqt2aFShDY9jA6aP3oDANAkAD\nNVjCKHEBf5uKOrKZQoHueYaIayxDngxiEaMFD7sjLfwvjPN8lQtEJQ4etN0g660T\ngjXAFTa9JY1TAKGEH4jhAkB4MGfT8LpJNqAPZkS+ePvzxyKVTdIP8AvJfm01Kd4N\nqLWf0KzpE7PDkt4o+eAsXh15RPgR7a6nSnf923/o7Do4\n-----END RSA PRIVATE KEY-----";

$public_key = "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvI00BCMWGmpDaMiiWfg1VAHAv\nFMMhwFN8v/zfeGllClzuR2SOwBafKEWxIs/XW7yhyciuq4BHDfDPzFKyaiGeuUWV\nCrKXTS1j3E6b8WJEBt3TV38O50f0hZ9OTtIcWdy2vg3o4IhRpdK1Duy3xeGQLFCB\nKrWUjlUqzS4J1sTncwIDAQAB\n-----END PUBLIC KEY-----";

$licensekey = 'kapDFjLjO0nn8IeSPkYwl8MJwbxTGtqoiNbbvoqMi6OJiVedUJ';

error_reporting(0);

header('Content-Type: application/json; charset=UTF-8');

$url = $_SERVER['REQUEST_URI'];
if(strpos($url, '/app/api/auth_image_download') !== false && isset($_REQUEST['image'])){
	$image_version_file = './data/images.json';
	$image_version_data = json_decode(file_get_contents($image_version_file), true);
	$image = null;
	foreach($image_version_data as $row){
		if($_REQUEST['image'] == $row['name']){
			$image = $row;break;
		}
	}
	if(is_array($image)){
		$data = ['status'=>200, 'msg'=>'授权成功', 'download_server'=>['mirror.cloud.idcsmart.com','mirror1.cloud.idcsmart.com','mirror2.cloud.idcsmart.com'], 'version'=>$image['version'], 'support_init'=>$image['support_init']];
	}else{
		$data = ['status'=>400, 'msg'=>'下载镜像不存在'];
	}
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/auth_update') !== false || strpos($url, '/app/api/auth_complete') !== false){
	$type = isset($_REQUEST['type'])?$_REQUEST['type']:'finance';
	if($type == 'finance'){
		$auth_data = [
			'id'=>1,
			'ip'=>$_REQUEST['ip'],
			'domain'=>$_REQUEST['domain'],
			'system_token'=>$_REQUEST['system_token'],
			'install_version'=>$_REQUEST['install_version'],
			'license'=>$_REQUEST['license'],
			'type'=>'finance',
			'edition'=>1,
			'create_time'=>date("Y-m-d H:i:s"),
			'due_time'=>null,
			'update_time'=>date("Y-m-d H:i:s"),
			'version_type'=>'beta',
			'installation_path'=>$_REQUEST['installation_path'],
			'auth_time'=>date("Y-m-d H:i:s"),
			'status'=>'Active',
			'auth_due_time'=>'2038-12-31 23:59:59',
			'high_availability' => 1,
			'app'=>$finance_apps,
			'last_license_time'=>time()
		];
	}else{
		$auth_data = [
			'id'=>1,
			'ip'=>$_REQUEST['ip'],
			'domain'=>$_REQUEST['domain'],
			'system_token'=>$_REQUEST['system_token'],
			'install_version'=>$_REQUEST['install_version'],
			'license'=>$_REQUEST['license'],
			'type'=>'cloud',
			'edition'=>1,
			'create_time'=>date("Y-m-d H:i:s"),
			'due_time'=>'2039-12-31 23:59:59',
			'update_time'=>date("Y-m-d H:i:s"),
			'version_type'=>'beta',
			'installation_path'=>$_REQUEST['installation_path'],
			'node_num' => 0,
			'node_ip' => '',
			'max_node' => 9999,
			'hyperv_max' => 9999,
			'smart_cpu_node_max' => 9999,
			'smart_cpu_node_num' => 0,
			'smart_bw_node_max' => 9999,
			'smart_bw_node_num' => 0,
			'abuse_manager_node_max' => 9999,
			'abuse_manager_node_num' => 0,
			'local_migrate_node_max' => 9999,
			'local_migrate_node_num' => 0,
			'mysql_database_node_max' => 9999,
			'mysql_database_node_num' => 0,
			'ceph_storage_node_max' => 9999,
			'ceph_storage_node_num' => 0,
			'auth_time'=>date("Y-m-d H:i:s"),
			'status'=>'Active',
			'auth_due_time'=>'2038-12-31 23:59:59',
			'high_availability' => 1,
			'app'=>$cloud_apps,
			'plugin'=>$cloud_plugins,
			'last_license_time'=>time()
		];
	}
	$data = ['status'=>200, 'msg'=>'授权成功', 'data'=>auth_encrypt(json_encode($auth_data), $private_key), 'ip'=>auth_encrypt($auth_data['ip'], $private_key)];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/auth_rc_plugin') !== false){
	$data = ['status'=>200, 'msg'=>'请求成功', 'data'=>[]];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/auth_rc') !== false){
	$auth_data = [
		'id'=>1,
		'ip'=>$_REQUEST['ip'],
		'domain'=>$_REQUEST['domain'],
		'system_token'=>'',
		'install_version'=>$_REQUEST['install_version'],
		'license'=>$_REQUEST['license'],
		'type'=>'business',
		'edition'=>1,
		'create_time'=>date("Y-m-d H:i:s"),
		'due_time'=>'2039-12-31 23:59:59',
		'update_time'=>date("Y-m-d H:i:s"),
		'version_type'=>'beta',
		'installation_path'=>'',
		'auth_time'=>date("Y-m-d H:i:s"),
		'status'=>'Active',
		'auth_due_time'=>'2039-12-31 23:59:59',
		'high_availability' => 1,
		'business_version' => 0,
		'app'=>$business_apps,
		'last_license_time'=>time()
	];
	$data = ['status'=>200, 'msg'=>'授权成功', 'data'=>auth_encrypt_n(json_encode($auth_data), $private_key), 'due_time'=>$auth_data['due_time'], 'auth_due_time'=>$auth_data['auth_due_time']];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/auth') !== false || strpos($url, '/app/api/toggle_version') !== false){
	$type = isset($_REQUEST['type'])?$_REQUEST['type']:'finance';
	if($type == 'cloud') $version = $cloud_last_version;
	else $version = $finance_last_version;
	$data = ['status'=>200, 'msg'=>'授权成功', 'professional'=>true, 'version'=>$version, 'last_version'=>$cloud_last_version, 'release_version'=>$cloud_release_version, 'remote_ip'=>$_SERVER['REMOTE_ADDR']];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/ip') !== false){
	$data = ['status'=>200, 'msg'=>'时间获取成功', 'ip'=>$_SERVER['REMOTE_ADDR'], 'country_code'=>''];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/get_new_version') !== false && isset($_REQUEST['update_type'])){
	if($_REQUEST['update_type'] == '1'){
		$version_file = './data/cloud_'.$cloud_release_version.'.json';
	}else{
		$version_file = './data/cloud_'.$cloud_last_version.'.json';
	}
	if(file_exists($version_file)){
		$version_data = json_decode(file_get_contents($version_file), true);
	}else{
		$version_data = ['version'=>$_REQUEST['version'],'update_type'=>$_REQUEST['update_type'],'description'=>'无此版本'];
	}
	$data = ['status'=>200, 'msg'=>'更新包获取成功', 'data'=>[$version_data]];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/get_version') !== false && isset($_REQUEST['version'])){
	$version = $_REQUEST['version'];
	$version_file = './data/cloud_'.$version.'.json';
	if($version && file_exists($version_file)){
		$version_data = json_decode(file_get_contents($version_file), true);
	}else{
		$version_data = ['version'=>$version,'description'=>'无此版本'];
	}
	$data = ['status'=>200, 'msg'=>'更新包获取成功', 'data'=>$version_data];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/get_image_version') !== false){
	$version = $_REQUEST['version'];
	$image_version_file = './data/image_version.json';
	$image_version_data = json_decode(file_get_contents($image_version_file), true);
	$data = ['status'=>200, 'msg'=>'获取成功', 'data'=>$image_version_data];
	echo json_encode($data);
}
elseif(strpos($url, '/app/api/get_images') !== false){
	$version = $_REQUEST['version'];
	$image_version_file = './data/images.json';
	$image_version_data = json_decode(file_get_contents($image_version_file), true);
	$data = ['status'=>200, 'msg'=>'镜像获取成功', 'data'=>$image_version_data];
	echo json_encode($data);
}
elseif(strpos($url, '/market/index') !== false){
	$hostid = 1;
	$apps = [];
	foreach($cloud_goods as $goods){
		$apps[] = ['hostid'=>$hostid, 'id'=>$goods['id'], 'qty'=>1, 'uuid'=>$goods['name'], 'nextduedate'=>2208959999];
	}
	$data = ['status'=>200, 'jwt'=>'eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9', 'msg'=>'登录成功', 'hostid'=>1, 'productid'=>1, 'son_host'=>[], 'apps'=>$apps, 'goods'=>$cloud_goods, 'data'=>['bind'=>1]];
	echo json_encode($data);
}
elseif(strpos($url, '/api/auth/version') !== false){
	$text = urldecode($_POST['text']);
	preg_match('/<ver>(.*?)<\/ver>/', $text, $match);
	if($match[1]) $version = $match[1];
	else $version = '2.0.1';
	$text = '<new>'.$version.'</new><onlynew>n</onlynew><stop></stop>';
	$rand = randStr(8);
	$time = (string)time();
	$sign = md5($text.$licensekey.$rand.$time);
	echo '<xml><text>'.$text.'</text><rand>'.$rand.'</rand><time>'.$time.'</time><sign>'.$sign.'</sign></xml>';
}
elseif(strpos($url, '/api/auth/check') !== false){
	$text = urldecode($_POST['text']);
	preg_match('/<ma>(.*?)<\/ma>/', $text, $match);
	$ma = $match[1];
	preg_match('/<sn>(.*?)<\/sn>/', $text, $match);
	$sn = $match[1];
	$time = (string)time();
	$verify1 = md5($ma.$sn.$time.'%g%fxGSwGz%s774NxWU7vs37S%I!i*R9');
	$text = '<msg></msg><verify1>'.$verify1.'</verify1><verify2>'.md5(time().rand(111,999)).'</verify2>';
	$rand = randStr(8);
	$sign = md5($text.$licensekey.$rand.$time);
	echo '<xml><text>'.$text.'</text><rand>'.$rand.'</rand><time>'.$time.'</time><sign>'.$sign.'</sign></xml>';
}
else{
	//file_put_contents('logs.txt', json_encode($_POST));
	$data = ['status'=>400, 'msg'=>'404 Not Found'];
	echo json_encode($data);
}


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
function auth_decrypt($data, $public_key){
	$_strcode = _strcode($data, "DECODE", "zjmf_key_strcode");
	$_strcode = explode("|zjmf|", $_strcode);
	$pu_key = openssl_pkey_get_public($public_key);
	$de_str = '';
	foreach ($_strcode as $v) {
		openssl_public_decrypt(base64_decode($v), $de, $pu_key);
		$de_str .= $de;
	}
	return $de_str;
}
function randStr($length = 8 , $lowwer = false)
{
	$str='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
	$len=strlen($str)-1;
	$randstr='';
	for($i=0;$i<$len;$i++){
		$num=mt_rand(0,$len);
		$randstr .= $str[$num];
	}
	if ($lowwer){
		return strtolower(substr($randstr,0,$length));
	}
	return substr($randstr,0,$length);
}
function auth_encrypt_n($data, $private_key){
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
	return base64_encode($_strcode);
}
function auth_decrypt_n($data, $public_key){
	$_strcode = explode("|zjmf|", base64_decode($data));
	$pu_key = openssl_pkey_get_public($public_key);
	$de_str = '';
	foreach ($_strcode as $v) {
		openssl_public_decrypt(base64_decode($v), $de, $pu_key);
		$de_str .= $de;
	}
	return $de_str;
}