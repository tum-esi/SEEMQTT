#!/bin/bash

# This script is used to generate certificates for CA, server and client
# Change the server_ip and client_ip according to your own setup
cadir='../CA'
serverdir='../server_cert'
clientdir='../client_cert'
server_ip='192.168.178.37'
client_ip='192.168.178.36'

# Remove old certificate files
rm -rf $cadir && mkdir $cadir
rm -rf $serverdir && mkdir $serverdir
rm -rf $clientdir && mkdir $clientdir

# Substitute IP address in configuration files
cp ca_cert.conf ca_cert.final.conf && sed -i "s/@server_ip@/$server_ip/g" ca_cert.final.conf
cp server_cert.conf server_cert.final.conf && sed -i "s/@server_ip@/$server_ip/g" server_cert.final.conf
cp client_cert.conf client_cert.final.conf && sed -i "s/@client_ip@/$client_ip/g" client_cert.final.conf

# Generate CA key, CA csr and CA crt
openssl genrsa -out $cadir/ca.key 2048
openssl req -new -config ca_cert.final.conf -key $cadir/ca.key -out $cadir/ca.csr
openssl x509 -req -days 365 -in $cadir/ca.csr -signkey $cadir/ca.key -out $cadir/ca.crt

# Generate server key, server csr and server crt signed by CA
openssl genrsa -out $serverdir/server.key 2048
openssl req -new -config server_cert.final.conf -key $serverdir/server.key -out $serverdir/server.csr
openssl x509 -req -days 365 -in $serverdir/server.csr -CA $cadir/ca.crt -CAkey $cadir/ca.key -set_serial 01 -out $serverdir/server.crt

# Generate client key, client csr and client crt signed by CA
openssl genrsa -out $clientdir/client.key 2048
openssl req -new -config client_cert.final.conf -key $clientdir/client.key -out $clientdir/client.csr
openssl x509 -req -days 365 -in $clientdir/client.csr -CA $cadir/ca.crt -CAkey $cadir/ca.key -set_serial 02 -out $clientdir/client.crt

# Clean up after run
rm ca_cert.final.conf
rm server_cert.final.conf
rm client_cert.final.conf
