/******************************************************************************
   Copyright 2018 Google
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************************************************/
// This file contains your configuration used to connect to Cloud IoT Core

// Wifi network details.
//const char *ssid = "TIVOLI";
//const char *password = "kurt525kubik";
const char *ssid = "TIVOLI";
const char *password = "kurt525kubik";



// Cloud iot details.
const char *project_id = "smart-heating-1";
const char *location = "us-central1";
const char *registry_id = "my-registry";
//const char *device_id = "cloud-sql-demo";
const char *device_id = "cloud-sql-demo";

// Configuration for NTP
const char* ntp_primary = "pool.ntp.org";
const char* ntp_secondary = "time.nist.gov";

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// To get the private key run (where private-key.pem is the ec private key
// used to create the certificate uploaded to google cloud iot):
// openssl ec -in <private-key.pem> -noout -text
// and copy priv: part.
// The key length should be exactly the same as the key length bellow (32 pairs
// of hex digits). If it's bigger and it starts with "00:" delete the "00:". If
// it's smaller add "00:" to the start. If it's too big or too small something
// is probably wrong with your key.
const char *private_key_str =
  "56:7b:44:b9:ab:29:35:01:c6:6d:79:b3:e6:ae:d2:9d:e6:4e:c6:d7:15:7b:18:3e:15:75:c8:c9:22:da:1c:b0";

// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 6 * 3600; // Maximum 24H (3600*24)

// To get the certificate for your region run:
//   openssl s_client -showcerts -connect mqtt.googleapis.com:8883
// for standard mqtt or for LTS:
//   openssl s_client -showcerts -connect mqtt.2030.ltsapis.goog:8883
// Copy the certificate (all lines between and including ---BEGIN CERTIFICATE---
// and --END CERTIFICATE--) to root.cert and put here on the root_cert variable.

const char *root_cert =
  "-----BEGIN CERTIFICATE-----\n"
  "MIICCjCCAZGgAwIBAgIQbkepyIuUtui7OyrYorLBmTAKBggqhkjOPQQDAzBHMQsw\n"
  "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
  "MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
  "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
  "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQA\n"
  "IgNiAATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzu\n"
  "hXyiQHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/l\n"
  "xKvRHYqjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud\n"
  "DgQWBBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNnADBkAjBqUFJ0\n"
  "CMRw3J5QdCHojXohw0+WbhXRIjVhLfoIN+4Zba3bssx9BzT1YBkstTTZbyACMANx\n"
  "sbqjYAuG7ZoIapVon+Kz4ZNkfF6Tpt95LY2F45TPI11xzPKwTdb+mciUqXWi4w==\n"
  "-----END CERTIFICATE-----\n";

// In case we ever need extra topics
const int ex_num_topics = 0;
const char* ex_topics[ex_num_topics];
//const int ex_num_topics = 1;
//const char* ex_topics[ex_num_topics] = {
//  "/devices/my-device/tbd/#"
//};