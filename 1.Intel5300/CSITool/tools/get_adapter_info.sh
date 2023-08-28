#!/bin/bash

# Get the list of physical interfaces
interfaces=$(ip -o link show | awk '{print $2}' | cut -d':' -f1 | grep -v -e '^lo$')

# Loop through the interfaces and get their details
for interface in $interfaces; do
    # Get the MAC address
    hwaddr=$(ip link show $interface | awk '/ether/ {print $2}')
    # Get the IP address and subnet mask
    ipaddr=$(ip addr show $interface | awk '/inet / {print $2}')
    # Get the gateway address
    gateway=$(ip route show | awk "/default/ && /$interface/ {print \$3}")
    echo "Interface: $interface"
    echo "HWAddr: $hwaddr"
    echo "IPAddr: $ipaddr"
    echo "Gateway: $gateway"
    echo ""
done