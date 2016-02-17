#!/usr/bin/env python

"""
Listen to some available signals from NetworkManager
"""

import dbus.mainloop.glib; dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
from gi.repository import GObject
import NetworkManager
import pyroute2

c = NetworkManager.const

d_args = ('sender', 'destination', 'interface', 'member', 'path')
d_args = dict([(x + '_keyword', 'd_' + x) for x in d_args])

def main():
    #NetworkManager.NetworkManager.connect_to_signal('CheckPermissions', display_sig, **d_args)
    #NetworkManager.NetworkManager.connect_to_signal('StateChanged', display_sig, **d_args)
    NetworkManager.NetworkManager.connect_to_signal('PropertiesChanged', display_sig, **d_args)
    #NetworkManager.NetworkManager.connect_to_signal('DeviceAdded', display_sig, **d_args)
    #NetworkManager.NetworkManager.connect_to_signal('DeviceRemoved', display_sig, **d_args)
    #NetworkManager.ActiveConnection.connect_to_signal('PropertiesChanged', display_sig, **d_args)

    print("Waiting for signals")
    print("-------------------")

    loop = GObject.MainLoop()
    loop.run()

def displayIp4ConfigInfo(ip4config):
	print("AddressData: {}".format(ip4config.AddressData))
	print("Gateway:     {}".format(ip4config.Gateway))
	print("RouteData:   {}".format(ip4config.RouteData))
	print("Nameservers: {}".format(ip4config.Nameservers))
	print("Domains:     {}".format(ip4config.Domains))
	print("Searches:    {}".format(ip4config.Searches))
	print("WinsServers: {}".format(ip4config.WinsServers))

def displayDhcp4ConfigInfo(dhcp4config):
	print("Options: {}".format(dhcp4config.Options))

def displayDeviceInfo(dev):
    print("Interface:      {}".format(dev.Interface))
    print("IpInterface:    {}".format(dev.IpInterface))
    print("Type:           {}".format(c('device_type', dev.DeviceType)))
    print("PhysicalPortId: {}".format(dev.PhysicalPortId))
    print("Driver:         {}".format(dev.Driver))
    print("Capabilities:   {}".format(dev.Capabilities))
    if dev.Dhcp4Config is NetworkManager.DHCP4Config:
        print("Dhcp4Config:")
        displayDhcp4ConfigInfo(dev.Dhcp4Config)
    if dev.Ip4Config is NetworkManager.IP4Config:
        print("Ip4Config:")
        displayIp4ConfigInfo(dev.Ip4Config)
    print("Dhcp6Config:    {}".format(type(dev.Dhcp6Config)))
    print("Ip6Config:      {}".format(type(dev.Ip6Config)))
    print("Managed:        {}".format(dev.Managed))
    print("State:          {}".format(c('device_state', dev.State)))

def displayConnectionInfo(c):
	pass

def displayActiveConnectionInfo(ac):

        print("Id:         {}".format(ac.Id))
        print("Connection:")
	displayConnectionInfo(ac.Connection)
        print("UUID:       {}".format(ac.Uuid))
        print("Type:       {}".format(ac.Type))
        print("State:      {}".format(c('active_connection_state', ac.State)))
        print("Default:    {}".format(ac.Default))
        print("VPN:        {}".format(ac.Vpn))
        if type(ac.Master) is NetworkManager.Device:
            print("Master: {}".format(ac.Master))
            displayDeviceInfo(ac.Master)

        for dev in ac.Devices:
            displayDeviceInfo(dev)
        print

def display_sig(*args, **kwargs):
    #print("Received signal: %s.%s" % (kwargs['d_interface'], kwargs['d_member']))
    #print kwargs.keys()
    #print "len(args) =", len(args)
    #print "type(args[0]) =", type(args[0])
    #print "dir(args[0]) =", dir(args[0])
    #print "len(kwargs) =", len(kwargs)

    if kwargs['d_member'] == 'DeviceAdded':
        print "DeviceAdded"
        print(kwargs.keys())
        displayDeviceInfo(args[0])

#        devicedetail = dev.SpecificDevice()
#        if not callable(devicedetail.HwAddress):
#            print("   MAC address      %s" % devicedetail.HwAddress)
#        print("   IPv4 config")
#        print("      Addresses")
#        for addr in dev.Ip4Config.Addresses:
#            print("         %s/%d -> %s" % tuple(addr))
#        print("      Routes")
#        for route in dev.Ip4Config.Routes:
#            print("         %s/%d -> %s (%d)" % tuple(route))
#        print("      Nameservers")
#        for ns in dev.Ip4Config.Nameservers:
#            print("         %s" % ns)

        return

    if kwargs['d_member'] == 'StateChanged':
        print "StateChanged"

        print(c('state', args[0]))

    if kwargs['d_member'] == 'PropertiesChanged':
        #print "PropertiesChanged"

        #print(args[0].keys())

        if args[0].has_key('Devices'):
            for dev in args[0]['Devices']:
                displayDeviceInfo(dev)
                print

        if args[0].has_key('ActiveConnections'):
            for ac in args[0]['ActiveConnections']:
                displayActiveConnectionInfo(ac)
                print

    #print("Sender:          (%s)%s" % (kwargs['d_sender'], kwargs['d_path']))
    #print("Arguments:       (%s)" % ", ".join([str(x) for x in args]))
    print

if __name__ == '__main__':
    main()
