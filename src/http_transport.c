#include "certs.h"
#include "http_transport.h"

#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/logging/log.h>

#include <string.h>

LOG_MODULE_REGISTER(HTTP_TRANSPORT, LOG_LEVEL_INF);

#define HOST CONFIG_APP_HTTP_HOST
#define HTTP_PORT CONFIG_APP_HTTP_PORT
#define HTTP_IP CONFIG_APP_HTTP_IP
#define HTTPS_PORT CONFIG_APP_HTTPS_PORT
#define PATH CONFIG_APP_HTTP_PATH

#define TLS_TAG 1

static bool tls_init(void) {
	int ret;
	ret = tls_credential_add(
			TLS_TAG,
			TLS_CREDENTIAL_CA_CERTIFICATE,
			ca_certificate,
			sizeof(ca_certificate)
			);
	if (ret < 0 && ret != -EEXIST) {
		LOG_ERR("TLS credential add failed: %d", ret);
		return false;
	}

	return true;
}

static bool send_over_https(const char *payload) {
	int sock;
	struct sockaddr_in addr;
	sec_tag_t sec_tag_list[] = { TLS_TAG };

	if (!tls_init()) {
		return false;
	}

	sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
	if (sock < 0) {
		LOG_ERR("HTTPS socket failed");
		return false;
	}

	zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,
			sec_tag_list, sizeof(sec_tag_list));
	zsock_setsockopt(sock, SOL_TLS, TLS_HOSTNAME,
			HOST, strlen(HOST));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTPS_PORT);
	zsock_inet_pton(AF_INET, HTTP_IP, &addr.sin_addr);

	if (zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		LOG_ERR("HTTPS connect failed");
		zsock_close(sock);
		return false;
	}

	char request[256];

	snprintf(request, sizeof(request),
			"POST %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %d\r\n"
			"Connection: Close\r\n"
			"\r\n"
			"%s",
			PATH, HOST, strlen(payload), payload);

	if (zsock_send(sock, request, strlen(request), 0) < 0) {
		LOG_ERR("HTTPS send failed");
		zsock_close(sock);
		return false;
	}

	zsock_close(sock);
	return true;
}

static bool send_over_http(const char *payload) {
	int sock;
	struct sockaddr_in addr;

	sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		LOG_ERR("HTTP socket create failed");
		return false;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTP_PORT);
	zsock_inet_pton(AF_INET, HTTP_IP, &addr.sin_addr);

	if (zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		LOG_ERR("HTTP connect failed");
		zsock_close(sock);
		return false;
	}

	char request[256];

	snprintf(request, sizeof(request),
			"POST %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %d\r\n"
			"Connection: close\r\n"
			"\r\n"
			"%s",
			PATH, HOST, strlen(payload), payload);
	if (zsock_send(sock, request, strlen(request), 0) < 0) {
		LOG_ERR("HTTP send failed");
		zsock_close(sock);
		return false;
	}
	zsock_close(sock);
	return false;
}

bool http_transport_send(const char *payload) {
	if (send_over_https(payload)) {
		LOG_INF("HTTPS used");
		return true;
	} else {
		LOG_INF("HTTPS failed, fallback to HTTP");
		return send_over_http(payload);
	}
}


