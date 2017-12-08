ThingPlug Simple SDK
===
SKT ThingPlug Simple SDK ������ API �� ������ ���� �����Դϴ�.

Architect
---
![](images/sdk_architect.png)

SDK Source Tree
---
+ __Simple__ (ThingPlug Simple SDK root folder)
	+ __middleware__ (middleware sample including makefiles for build)
	+ __lib__ (external libraries - paho, TLS, )
	+ __include__ (Header files of the ThingPlug Simple SDK)
	+ __src__ (Source files of the ThingPlug Simple SDK)
		+ __simple__ (Source files of the ThingPlug Simple API)
		+ __net__ (Source files of the MQTTS)
			
Description
---
Directory | Description
------------ | -------------
Simple | ThingPlug Simple SDK ��Ʈ ����
middleware |	SDK �� �̿��� �̵���� ���� ����
lib | �ܺ� ���̺귯���� �ҽ��� ����
include | ���ۿ� �ʿ��� header ���ϵ� ����
src | ��ü �ҽ� ����
simple | ThingPlug Simple API �ҽ� ����
net | MQTT/TLS ���� �ҽ� ����

Library
---
MQTT TLS ������ ���Ͽ� ���̺귯���� ����մϴ�.

���� ȯ�� | TLS ���� | MQTT ���̺귯�� | TLS ���̺귯��
------------ | ------------- | ------------- | -------------
__ARM/Linux__ | __O__ | paho | OpenSSL(to be installed.)
__ARDUINO__ | __X__ | paho |
__mbed__ |	__O__ | paho | mbed TLS
__FreeRTOS__ |	__O__ | paho | mbed TLS

* [paho Homepage](https://eclipse.org/paho/)
* [OpenSSL Homepage](https://www.openssl.org/)
* [mbedTLS Homepage](https://tls.mbed.org/)


API Documentation
---
* [Download](docs/ThingPlug_Simple_SDK_API-20171206.pdf)

Middleware
---
SDK�� ����� �̵���� ������ ������ �����ϴ�.

* __ThingPlug_Simple_SDK.c__ or __ThingPlug_Simple_SDK.ino__ : ThingPlug Simple SDK ���� �����ϴ� API�� ��� �׽�Ʈ�غ� �� �ֽ��ϴ�.
	+ __MA__ (�̵������ ManagementAgent ����)
	+ __SRA__ (�̵������ SensorManagementAgent ����)
	+ __SMA__ (�̵������ ServiceReadyAgent ����)

