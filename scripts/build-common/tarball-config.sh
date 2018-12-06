#!/bin/bash
set -o errexit
set -o nounset

SQLITE_BASE_URL="https://www.sqlite.org/2018";
SQLITE_VERSION="autoconf-3250300";
SQLITE_NAME="sqlite-$SQLITE_VERSION";
SQLITE_TARBALL="$SQLITE_NAME.tar.gz";

OPENSSL_BASE_URL="https://www.openssl.org/source";
OPENSSL_VERSION="1.1.1a";
OPENSSL_NAME="openssl-$OPENSSL_VERSION";
OPENSSL_TARBALL="$OPENSSL_NAME.tar.gz";

CURL_BASE_URL="https://curl.haxx.se/download";
CURL_VERSION="7.62.0";
CURL_NAME="curl-$CURL_VERSION";
CURL_TARBALL="$CURL_NAME.tar.gz";
