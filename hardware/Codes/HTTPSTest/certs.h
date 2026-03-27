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
MIIDDTCCAfWgAwIBAgIUV/zKsjDywJzhvrk1ED7zyFKwsB8wDQYJKoZIhvcNAQEL
BQAwFjEUMBIGA1UEAwwLNjUuMi4zMC4xMjIwHhcNMjYwMzI3MTIyODQ4WhcNMjcw
MzI3MTIyODQ4WjAWMRQwEgYDVQQDDAs2NS4yLjMwLjEyMjCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAM7Uy5E8pHNUap3bZY0WRr1M5bmu2vmueBcWrpcb
ShLYcNVRxFUfdup7tdE2Naki1C4eR3U0ozgnv9h7YLqQ9M42b6M5A5dHoH2G9x+U
+0e2Lb1FHKelAX7UMjB1d8TlbEtRmb06jaZgQReAc6iqT6YkN5iwG6D4k6XLZw5Q
9KkkBQTIApaONFrTkQOtO1bmo8mGvxTatS6JToe3FQXv+uX/jmHSO5s9l9F/VMZb
oyWn7HrWykb+tIetNMAHv75IFfPeu3P6JmJvnqeuHm4MOowegOJdS1AcXl0hnwxe
Rueyvt7VXal4iK5P0b8TV2ruLLSxcjD2pumlPUvpe4WwGu8CAwEAAaNTMFEwHQYD
VR0OBBYEFO0mdCKIU3LPtyjyR9Fj609Uypj3MB8GA1UdIwQYMBaAFO0mdCKIU3LP
tyjyR9Fj609Uypj3MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB
ABKmtXY1JdqB/9bAXyM8ce0AndtDQ7+Bf4MCCvSvE0y1eyc1kzf0EQfFryYv4+8Q
KYRc/UXTe2nIcF9rwAlBZspW0G/lj9kaPqyTF98H6mO9vao1WAzagwdrRDG1skGe
ow6HHZk7h+o7lpUI6ZHQVNrahom5CyaM4ozJPtmdjulXTULXcvtWlcyjWbRqIOIv
zjUq2orVmguOm9Sa2v86ytyJ61WpLbh/qCWpSb/6RhxymP4pHx1cSFVZogNH6biT
ka0NuohXoc9qiwE5TL/XU5vNHRAqiAuQLeNCepfdezpfMtX6KO4MZQMSA3/u7rxq
4LLzy1OfJux9We/s/ZIBbRc=
-----END CERTIFICATE-----
)EOF";

#endif
