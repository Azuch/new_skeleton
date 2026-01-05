#pragma once

#include <stddef.h>

/**
 * @brief Initializes the HTTP transport layer.
 *
 * This function sets up necessary resources, such as TLS credentials.
 * It should be called once before any send operations.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int http_transport_init(void);

/**
 * @brief Sends data over HTTP or HTTPS.
 *
 * This function resolves the hostname, establishes a connection, and sends
 * the provided payload. The transport (HTTP/HTTPS) and IP version (IPv4/IPv6)
 * are determined by the project's Kconfig settings.
 *
 * @param payload The data to send. If NULL or empty, a GET request is sent.
 *                Otherwise, a POST request is sent.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int http_transport_send(const char *payload);
