# IDCSmart PHP Extension

PHP扩展，用于重定向魔方系统的授权请求。

## 项目结构

```
├── web/              # 授权API服务（可选）
│   ├── index.php     # 授权接口
│   └── data/         # 数据文件
├── ext/              # PHP扩展源码
│   ├── idcsmart.c
│   └── config.m4
└── .github/          # 自动编译配置
```

## 支持架构
- x86_64 (AMD64)
- aarch64 (ARM64)

## 支持PHP版本
- PHP 7.4

## 使用方法

### 1. 安装PHP扩展

从 [Releases](../../releases) 下载对应架构的 `idcsmart.so`，或使用GitHub Actions自动编译。

手动编译：
```bash
cd ext
phpize
./configure
make
sudo make install
```

### 2. 配置

在 `php.ini` 中添加：

```ini
extension=idcsmart.so
idcsmart.url=https://your-auth-server.com/
idcsmart.app=App1,App2,App3
```

### 3. 重启PHP

```bash
systemctl restart php-fpm
```

### 4. 验证

```bash
php -m | grep idcsmart
php -i | grep idcsmart.url
```

## 自建授权服务器（可选）

将 `web/` 目录部署到Web服务器，配置伪静态后，在 `php.ini` 中设置 `idcsmart.url` 指向该服务器。
