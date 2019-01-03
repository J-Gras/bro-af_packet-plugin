
Bro::AF_Packet
==============

This plugin provides native AF_Packet support
(http://man7.org/linux/man-pages/man7/packet.7.html) for Bro.

# Installation

Before installing the plugin, make sure the kernel headers are installed and your
kernel supports PACKET_FANOUT [1]_ and TPACKET_V3.

## Bro Package Manager
The plugin is available as package for the `Bro Package Manager
<https://github.com/bro/package-manager>`_ and can be installed using the
following command::

    bro-pkg install bro-af_packet-plugin

To install uncompiled plugins, Bro's source code must be available to the package manager
(see package manager's `documentation <http://bro-package-manager.readthedocs.io/en/
stable/quickstart.html#basic-configuration>`_ for more information).

## Manual Install
In order to build this module you will have to compile it against the bro source
code and some of the built bro modules. See https://www.bro.org/sphinx/install/install.html
and follow the instructions for installing from source. 
Note: You do not need to run make install, you only need to run `./configure && make`
in order to build
the required bro plugins. You know you have done it correctly when the bro/cmake
folder has populated with the BroPlugin.cmake file.

The following will compile and
install the AF_Packet plugin alongside Bro, assuming it can find the
kernel headers in a standard location:

    # ./configure && make && make install

On RHEL systems you can install headers with `yum install -y kernel-devel` and
on Debian you can install with `sudo apt-get install -y linux-headers-$(uname -r)`.

If the headers are installed somewhere non-standard, add
``--with-kernel=<kernel-header-directory>`` to the ``configure`` command. Furthermore,
``--with-latest-kernel`` will use the latest headers available instead of looking for
the headers matching the running kernel's version. If everything built and installed
correctly, you should see this::

    # bro -NN Bro::AF_Packet
    Bro::AF_Packet - Packet acquisition via AF_Packet (dynamic, version 1.3)
    [Packet Source] AF_PacketReader (interface prefix "af_packet"; supports live input)
    [Type] AF_Packet::FanoutMode
    [Constant] AF_Packet::buffer_size
    [Constant] AF_Packet::enable_hw_timestamping
    [Constant] AF_Packet::enable_fanout
    [Constant] AF_Packet::fanout_mode
    [Constant] AF_Packet::fanout_id

.. [1] Note that some kernel versions between 3.10 and 4.7 might exhibit a bug that
   prevents the required symmetric hashing. The script available at
   https://github.com/JustinAzoff/can-i-use-afpacket-fanout can be used to verify
   whether PACKET_FANOUT works as expected.

# Usage

Once installed, you can use AF_Packet interfaces/ports by prefixing them
with ``af_packet::`` on the command line. For example, to use AF_Packet
to monitor interface ``eth0``::

    # bro -i af_packet::eth0

To use AF_Packet, running Bro without root privileges, the Bro processes needs
the CAP_NET_RAW capability. You can set it with the following command (on each
sensor, after ``broctl install``)::

    # setcap cap_net_raw+eip <path_to_bro>/bin/bro

The AF_Packet plugin automatically enables promiscuous mode on the interfaces.
As the plugin is using PACKET_ADD_MEMBERSHIP to enter the promiscuous mode
without interfering others, the PROMISC flag is not touched. To verify that the
interface entered promiscuous mode you can use ``dmesg``.

To adapt the plugin to your needs, you can set a couple of parameters like
buffer size. See scripts/init.bro for the default values.

# Usage with `broctl`

To use the AF_Packet plugin with ``broctl``, the ``custom`` load balance method can
be utilized. The following shows an exemplary configuration::

    [manager]
    type=manager
    host=localhost

    [proxy-1]
    type=proxy
    host=localhost

    [worker-1]
    type=worker
    host=localhost
    interface=af_packet::eth0
    lb_method=custom
    lb_procs=8
    pin_cpus=0,1,2,3,4,5,6,7
    # Optional parameters for per node configuration:
    af_packet_fanout_id=23
    af_packet_fanout_mode=AF_Packet::FANOUT_HASH
    af_packet_buffer_size=128*1024*1024

If all interfaces using ``lb_method=custom`` should be configured for AF_Packet, the
prefix can be globally definied by adding the following line to ``broctl.conf``::

  lb_custom.InterfacePrefix=af_packet::

# Limitations

* Even using AF_Packet's ``ETH_P_ALL``, the kernel removes VLAN tags. While the tags
  are provided spereately, there is no efficient way to pass them to Bro.
