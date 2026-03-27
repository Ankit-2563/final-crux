// ============================================================
// certs.h — CA certificate used by SSLClient
// ============================================================
#ifndef CERTS_H
#define CERTS_H

#include <pgmspace.h>

static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDEzCCAfugAwIBAgIUSysa+3G0YmNCD/YzlS018094JXAwDQYJKoZIhvcNAQEL
BQAwGTEXMBUGA1UEAwwOMTUuMjA2LjEyNS4yNDcwHhcNMjYwMzI3MTcyMDE5WhcN
MjcwMzI3MTcyMDE5WjAZMRcwFQYDVQQDDA4xNS4yMDYuMTI1LjI0NzCCASIwDQYJ
KoZIhvcNAQEBBQADggEPADCCAQoCggEBAMzkHR7rsywUpRWtGW+zVXV3OrQtskv8
vaw4KkDikr174F2pFehobCeHJGFt6Z2ZYdacVtsR+gLQuyvE2FMWsgux0kuY6//t
w0cumn/nrOklvV0q4eUwxv2DSAO6Kduu7q4D4nR4V38dP5roa7O4S+oVd7JEHJ+0
bvpgag7ccazLhOy/uS2J4di57iAsLEtFbSbIb2kbKR2IEY4zWLG+wxkV+eQ8uSNV
QrxJU/z9qavC53WhMPIwrs2U8CmUlSVm6mD/zPUzIF3aLAYePdI+HZo5WcvszCpA
NiOITF+papyFVARaaCPVlkc5t5I4T3Y3MdYWacAzxQQYHxssTXUhe3UCAwEAAaNT
MFEwHQYDVR0OBBYEFOi7hw2/z1xvG0A8Vo8KhOP1gqN5MB8GA1UdIwQYMBaAFOi7
hw2/z1xvG0A8Vo8KhOP1gqN5MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL
BQADggEBAIkLTFZjGRjZDMzOIqudm0rBb7LZJG90ZvajdAKFogH0klqOdbvA9vRB
kUDai1BBA5N8MhMdCku1NIriJFot4HezHMaEdI7NWvkPJGFb/Cec83fO7W2CQziq
A2NgvabqIuXr4/x+Eg6pvixk/SVlG9lvJO7I7HhB4GsxsnbV3xNCKi1HKChMMwh1
x1b6exKqvoyXN0uV4BkpXfw5EmVqe/+Bb4D3jZX+Vd2mjXq0OFJOBqba7Kdm1bB0
xKd7pd3I0k4Vs7idZPLeYVPgaFDoWuOJGP8GZ53t/kqFFrloPU11T3fDgni9Pebm
SnduzvpHdtXyJRVJvbMe+AZYHJ1vezo=
-----END CERTIFICATE-----
)EOF";

#endif