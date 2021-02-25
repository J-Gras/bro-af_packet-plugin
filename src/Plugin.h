
#ifndef ZEEK_PLUGIN_ZEEK_AF_PACKET
#define ZEEK_PLUGIN_ZEEK_AF_PACKET

#include "zeek/plugin/Plugin.h"

namespace plugin {
namespace Zeek_AF_Packet {

class Plugin : public zeek::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	zeek::plugin::Configuration Configure() override;
};

extern Plugin plugin;

}
}

#endif
