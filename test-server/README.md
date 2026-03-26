Hardware Communication Protocol Analysis
Based on the analysis of the hardware folder, here are the findings regarding the communication protocols used by the VVM601 module (ESP32-S3 + Quectel EC200U 4G LTE):

1. HTTP vs. HTTPS vs. MQTT
While the 
README.md
 states the Quectel EC200U module supports HTTP and HTTPS, the actual code provided in the Codes folder does not use HTTP or HTTPS.

Instead, the hardware uses MQTT (Message Queuing Telemetry Transport) for communication over the LTE network.

MQTT.ino
: Uses unencrypted, standard MQTT over TCP (Port 1883) connecting to broker.hivemq.com.
AWSIoT.ino
: Uses Secure MQTT (MQTTS) over port 8883 to connect to an AWS IoT broker.
2. TLS 1.2 Confirmation
You mentioned you wanted to confirm if it uses TLS 1.2.

In 
AWSIoT.ino
, the code uses secure communication:

cpp
#include <TinyGsmClient.h>
#include "SSLClient.h"  //https://github.com/govorox/SSLClient/tree/master
#include <PubSubClient.h>
// ...
TinyGsmClient gsmClient(modem);
SSLClient secureClient(&gsmClient);
PubSubClient  mqtt(secureClient);
The underlying TinyGsmClient handles the LTE TCP connection.
It is wrapped with SSLClient (an external library by govorox) which handles the encryption in software on the ESP32.
It passes the Mutual TLS (mTLS) certificates (cacert, client_cert, privkey) securely to the AWS IoT broker.
Conclusion on TLS 1.2: AWS IoT Core enforces TLS 1.2 for connections on port 8883. Because the ESP32 successfully connects to AWS IoT using the SSLClient library, yes, it is using TLS 1.2 to secure the MQTT payload over the LTE connection.

Summary
Protocol: MQTT / MQTTS (Not HTTP/HTTPS)
Security: TLS 1.2 is used specifically in the 
AWSIoT.ino
 implementation via software (SSLClient) on the ESP32 for AWS IoT mutually-authenticated communication.# Deploying to AWS EC2 t3.small (Ubuntu)

Complete step-by-step guide to deploy the test server with Docker, Nginx, and self-signed SSL.

---

## 1. Launch EC2 Instance

- **AMI**: Ubuntu 22.04 LTS
- **Instance type**: t3.small
- **Security Group** — open these ports:
  | Port | Protocol | Source    | Purpose         |
  |------|----------|----------|-----------------|
  | 22   | TCP      | Your IP  | SSH access      |
  | 80   | TCP      | Anywhere | HTTP → redirect |
  | 443  | TCP      | Anywhere | HTTPS           |

---

## 2. SSH Into Instance

```bash
ssh -i your-key.pem ubuntu@<EC2_PUBLIC_IP>
```

---

## 3. Install Docker & Docker Compose

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install Docker
sudo apt install -y docker.io docker-compose-v2

# Add user to docker group (no sudo needed for docker commands)
sudo usermod -aG docker ubuntu

# Log out and back in for group change to take effect
exit
# SSH back in
ssh -i your-key.pem ubuntu@<EC2_PUBLIC_IP>

# Verify
docker --version
docker compose version
```

---

## 4. Copy Project Files to EC2

From your local machine:

```bash
scp -i your-key.pem -r /Users/ankitbhavarthe/Downloads/final-crux/test-server ubuntu@<EC2_PUBLIC_IP>:~/
```

---

## 5. Generate Self-Signed SSL Certificates

On the EC2 instance:

```bash
cd ~/test-server

# Create ssl directory
mkdir -p ssl

# Generate CA private key
openssl genrsa -out ssl/ca.key 4096

# Generate CA certificate (valid 10 years)
openssl req -new -x509 -days 3650 -key ssl/ca.key -out ssl/ca.crt \
  -subj "/C=IN/ST=State/L=City/O=Crux/CN=CruxTestCA"

# Generate server private key
openssl genrsa -out ssl/server.key 2048

# Generate server certificate signing request (CSR)
# IMPORTANT: Replace <EC2_PUBLIC_IP> with your actual EC2 public IP
openssl req -new -key ssl/server.key -out ssl/server.csr \
  -subj "/C=IN/ST=State/L=City/O=Crux/CN=<EC2_PUBLIC_IP>"

# Create extension file for IP SAN (Subject Alternative Name)
# IMPORTANT: Replace <EC2_PUBLIC_IP> with your actual EC2 public IP
cat > ssl/server_ext.cnf << EOF
[v3_req]
subjectAltName = IP:<EC2_PUBLIC_IP>
EOF

# Sign server certificate with our CA (valid 1 year)
openssl x509 -req -days 365 \
  -in ssl/server.csr \
  -CA ssl/ca.crt -CAkey ssl/ca.key -CAcreateserial \
  -out ssl/server.crt \
  -extfile ssl/server_ext.cnf -extensions v3_req

echo "SSL certificates generated successfully!"
```

---

## 6. Copy CA Certificate to Hardware

After generating the certificates, you need to copy the CA certificate content into the ESP32 firmware:

```bash
# Print the CA certificate — copy this entire output
cat ssl/ca.crt
```

Copy the output (including `-----BEGIN CERTIFICATE-----` and `-----END CERTIFICATE-----`) and paste it into `hardware/Codes/HTTPSTest/certs.h` replacing the placeholder.

---

## 7. Create the .env File on EC2

```bash
cd ~/test-server

cat > .env << 'EOF'
PORT=4000
MONGO_URL=mongodb+srv://ankit123bhavarthe_db_user:WuEyiD9sxBBI1wGu@cluster0.ifupv62.mongodb.net/crux-database
EOF
```

---

## 8. Build and Run with Docker Compose

```bash
cd ~/test-server
docker compose up -d --build
```

Check status:
```bash
docker compose ps
docker compose logs -f
```

---

## 9. Verify Deployment

```bash
# Test HTTPS endpoint (use -k to accept self-signed cert)
curl -k https://localhost/health

# Test POST
curl -k -X POST https://localhost/api/test \
  -H "Content-Type: application/json" \
  -d '{"value": 7, "deviceId": "test-device"}'

# Test GET
curl -k https://localhost/api/test
```

From your local machine:
```bash
curl -k https://<EC2_PUBLIC_IP>/health
```

---

## 10. Update Hardware Config

In `hardware/Codes/HTTPSTest/config.h`, set:
```cpp
const char SERVER_HOST[] = "<EC2_PUBLIC_IP>";
```

Then flash the `HTTPSTest.ino` to your VVM601 board.

---

## Useful Commands

```bash
# Stop
docker compose down

# Restart
docker compose restart

# View logs
docker compose logs -f app
docker compose logs -f nginx

# Rebuild after code changes
docker compose up -d --build
```
