// ============================================================
// certs.h — Self-Signed CA Certificate
// ============================================================
// IMPORTANT: After generating SSL certificates on your EC2 instance,
// run: cat ssl/ca.crt
// and paste the ENTIRE output below, replacing the placeholder.

#ifndef CERTS_H
#define CERTS_H

#include <pgmspace.h>

// Self-signed CA certificate (PEM format)
// Replace the content between BEGIN and END with your actual CA certificate
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
PASTE_YOUR_CA_CERTIFICATE_HERE
-----END CERTIFICATE-----
)EOF";

#endif
